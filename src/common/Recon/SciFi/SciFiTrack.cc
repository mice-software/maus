/* 
** Written for the MICE proposal simulation
** Some examples from the ROOT project at CERN used
** Ed McKigney - Aug 21 2002
** Rewritten
** Ed McKigney - Mar 9 2003
*/

#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
#include "Calib/SciFiReconstructionParams.hh"
#include "Recon/SciFi/SciFiTrack.hh"

//#include "TFitter.h"
//#include "TVirtualFitter.h"
//#include "TMath.h"

#include <algorithm>

using std::vector;

#ifndef M_PI

// for g++ 4.x
static const double M_PI = 3.141592654;

#endif

SciFiTrack::SciFiTrack()
{
  	miceMemory.addNew( Memory::SciFiTrack );

	spacePoints = vector<SciFiSpacePoint*>(0);

	track_x0 = 0.0;
	track_y0 = 0.0;
	track_r = 0.0;
	track_alpha = 0.0;
	track_phi0 = 0.0;
	track_pt = 0.0;
	track_rms_alpha = 0.0;
	track_chi2 = 0.0;
	for(int i = 0; i<5; i++) 
	{
		for (int j = 0; j< 5; j++)
		{
			track_cov[i][j] = 0;
		}
	}
	track_npoints = 0;

	mc_track_x0 = 0.0;
	mc_track_y0 = 0.0;
	mc_track_r = 0.0;
	mc_track_alpha = 0.0;
	mc_track_phi0 = 0.0;
	mc_track_pt = 0.0;

	nChans = 0;
	nMuChans = 0;

	recStatus = 0;
}

void SciFiTrack::operator=(SciFiTrack right)
{
	spacePoints.clear();
	for (vector<SciFiSpacePoint*>::const_iterator it = right.spacePoints.begin();
	it != right.spacePoints.end(); it++)
		spacePoints.push_back(*it);

	track_x0 = right.track_x0;
	track_y0 = right.track_y0;
	track_r = right.track_r;
	track_alpha = right.track_alpha;
	track_phi0 = right.track_phi0;
	track_pt = right.track_pt;
	track_rms_alpha = right.track_rms_alpha;
	track_chi2 = right.track_chi2;
	for(int i = 0; i<5; i++) 
	{
		for (int j = 0; j< 5; j++)
		{
			track_cov[i][j] = right.track_cov[i][j];
		}
	}
	track_npoints = right.track_npoints;

	mc_track_x0 = right.mc_track_x0;
	mc_track_y0 = right.mc_track_y0;
	mc_track_r = right.mc_track_r;
	mc_track_alpha = right.mc_track_alpha;
	mc_track_phi0 = right.mc_track_phi0;
	mc_track_pt = right.mc_track_pt;

	nChans = right.nChans;
	nMuChans = right.nMuChans;

	recStatus = right.recStatus;
}


// Nasty global variables for use with Minuit objective function
SciFiTrack *GlobalTrack;
double GlobalZRefs[5];
double GlobalBCharge;
double BField;
vector<Hep3Vector> calcPoints(double *, double *, double, double);
double GetMomentum(double, double, double);
double GetMomentumLoss(double , double);

void tpchi2(int &npar, double *gin, double &f, double *x, int iflag)
{
	vector <SciFiSpacePoint>GlobalSpacePoints = vector <SciFiSpacePoint>(0);
	for (int i = 0; i <GlobalTrack->getNSpacePoints(); i++)
		GlobalSpacePoints.push_back(GlobalTrack->getSpacePoint(i));

	// Get the calculated track points
	vector<Hep3Vector> calcVec = calcPoints(GlobalZRefs, x, BField, GlobalBCharge);
	// Hard coded constant McKigney
	double msErr = 0.022; // error induced in the calculated track per plane
	double facErr = 0.22;
	vector<double> calcErr(0);
	for (int i=0; i<5; i++)
	{
		calcErr.push_back(i*msErr);
	}

	double chi2 = 0;
	for(unsigned int i = 0; i<GlobalSpacePoints.size(); i++)
	{
		for(unsigned int j = 0; j<calcVec.size(); j++)
		{
			if (fabs(GlobalSpacePoints[i].getPos().z()-calcVec[j].z()) < 1.0)
			{
				double xdiff = GlobalSpacePoints[i].getPos().x()-calcVec[j].x();
				xdiff *= xdiff;
				double ydiff = GlobalSpacePoints[i].getPos().y()-calcVec[j].y();
				ydiff *= ydiff;
				double terr = calcErr[j]*calcErr[j];
				double xerr =  facErr*GlobalSpacePoints[i].getPosE().x();
				xerr *= xerr;
				xerr += terr;
				double yerr =  facErr*GlobalSpacePoints[i].getPosE().y();
				yerr *= yerr;
				yerr += terr;
				chi2 += xdiff/xerr + ydiff/yerr;
			}
		}
	}

	double r = x[2];
	double alpha = x[3];
	double pt, pz, psqr;
	double zdiff, tdiff, len, tdiffc;
	double beta;
	double c=300, mmu=105.658389; //[mm/ns], [MeV]
	int iref = 0;
	// and now the chi2 for the time differences
	for (unsigned int i=0; i<GlobalSpacePoints.size(); i++)
	{
		zdiff = GlobalSpacePoints[i].getPos().z()-GlobalSpacePoints[iref].getPos().z();
		len = zdiff*(sqrt(1+r*r*alpha*alpha));
		pt=0.2998*r*BField;
		pz = fabs(pt/(r*alpha));
		psqr = pt*pt + pz*pz;
		beta = sqrt(psqr/(psqr+mmu*mmu));
		tdiffc = len/(beta*c);
		tdiff = GlobalSpacePoints[i].getTime()-GlobalSpacePoints[iref].getTime();
		chi2 += (tdiff-tdiffc)*(tdiff-tdiffc)/(
		GlobalSpacePoints[i].getTimeE()*GlobalSpacePoints[i].getTimeE()+
		GlobalSpacePoints[iref].getTimeE()*GlobalSpacePoints[iref].getTimeE());
	}
		
	f = chi2;
}

double SciFiTrack::extrapolate( const SciFiSpacePoint& ext )
{
	// Ignore the time difference for now
	double chi2 = 0;
	double zref;
	double xm, ym;
	double xc, yc;
	double xerr, yerr;

	zref = ext.getPos().z();
	xc = track_r * cos( track_alpha * ( zref - z_ref ) + track_phi0 ) + track_x0; 
	yc = track_r * sin( track_alpha * ( zref - z_ref ) + track_phi0 ) + track_y0;
	xm = ext.getPos().x();
	ym = ext.getPos().y();
	xerr = ext.getPosE().x();
	yerr = ext.getPosE().y();

	chi2 += (xc-xm)*(xc-xm)/(xerr*xerr) + (yc-ym)*(yc-ym)/(yerr*yerr);

        // determine chi2 due to time difference

        //double time = ext.getTime();
        int best = -1;
        double dz = 1e9;

        for( unsigned int i = 0; i < spacePoints.size(); ++i )
          if( fabs( ext.getPos().z() - spacePoints[i]->getPos().z() ) < dz )
          {
            dz = fabs( ext.getPos().z() - spacePoints[i]->getPos().z() );
	    best = i;
          }

        //double dt = time - spacePoints[best]->getTime();

	//ME timE = 0 for some reason... chi2 += ( dt * dt ) / ext.getTimeE();

	return chi2;
}

void SciFiTrack::fit(SciFiReconstructionParams &recPars)
{
#ifdef USE_OLD_STYLE_ROOT_FIT

	double zsign = spacePoints[0]->getTrackerNo() == 0 ? -1.0 : 1.0;
	int tid = zsign < 0 ? 0 : 1;

	// Do the next step in the processing, using Minuit to
	// fit the parameters using all available points and
	// the previously calculated parameters as a starting point

	if (recStatus == 1)
	{
		// Count number of fiber hits and number of muon hits
		nChans = 0;
		nMuChans = 0;
		for (unsigned int i = 0; i<spacePoints.size(); i++)
		{
			nChans += spacePoints[i]->getNChans();
			nMuChans += spacePoints[i]->getNMuChans();
		}

		GlobalTrack = this;
		GlobalBCharge=recPars.getCharge();
		for (int i = 0; i<5; i++)
			GlobalZRefs[i] = recPars.getTrackerPosition(tid).z()+
				         recPars.getZPos(tid, zsign<0?4-i:i);

		// Initialize TMinuit via generic fitter interface 
		// with a maximum of 5 params
		static TFitter *minuit = new TFitter(5);

		double arglist[100];
		arglist[0] = -5;
		minuit->ExecuteCommand("SET PRINT", arglist, 1);

		minuit->SetFCN(tpchi2);
		BField = recPars.getBField();

		minuit->SetParameter(0, "x0",    track_x0,    0.1,   0, 0);
		minuit->SetParameter(1, "y0",    track_y0,    0.1,   0, 0);
		minuit->SetParameter(2, "r",     track_r,     0.1,   0, 0);
		minuit->SetParameter(3, "alpha", track_alpha, 0.001, 0, 0);
		minuit->SetParameter(4, "phi0",  track_phi0,  0.001, 0, 0);
		minuit->ExecuteCommand("MIGRAD", arglist, 0);
		//minuit->ExecuteCommand("MINOS", arglist, 0);

		char junk[256];
		Double_t junk1, junk2;
		Double_t mnx0, mny0, mnr, mnalpha, mnphi0;
		Double_t mnx0e, mny0e, mnre, mnalphae, mnphi0e;
		minuit->GetParameter(0,&junk[0],mnx0,mnx0e,junk1,junk2);
		minuit->GetParameter(1,&junk[0],mny0,mny0e,junk1,junk2);
		minuit->GetParameter(2,&junk[0],mnr,mnre,junk1,junk2);
		minuit->GetParameter(3,&junk[0],mnalpha,mnalphae,junk1,junk2);
		minuit->GetParameter(4,&junk[0],mnphi0,mnphi0e,junk1,junk2);

		// Need to swith from TFitter to TMinuit to get the params
		//minuit->mnemat(&track_cov[0][0],5);

		Double_t mnchi2;
		Int_t junk3, junk4;
		Int_t ierr=minuit->GetStats(mnchi2, junk1, junk2, junk3, junk4);

		if (ierr==3)
		{

			track_x0    = mnx0;
			track_y0    = mny0;
			track_r     = mnr;
			track_alpha = mnalpha;
			track_phi0  = mnphi0;
			track_chi2  = mnchi2;
			//cerr << "**** chi2 = " << mnchi2 << endl;
			track_pt = track_r*(0.2998*recPars.getBField());
			/*
			double old_r = track_r;

			// Hard Coded constant - McKigney nstation*fraxX0
			double ploss = GetMomentumLoss(5*0.2228775e-2);
			double tpz = fabs(track_pt/(track_r*track_alpha));
			double tptot = sqrt(track_pt*track_pt+tpz*tpz);
			tpz = (tptot+zsign*ploss)*(tpz/tptot);
			track_pt = (tptot+zsign*ploss)*(track_pt/tptot);
			track_r = track_pt/(0.2998*recPars.getBField());
			track_alpha=-1*bcharge*fabs(track_pt/(track_r*tpz));

			double txn=1., tyn=1., tzn=1000000.0*zsign;
			for (unsigned int i=0; i<spacePoints.size(); i++)
			{
				if (zsign*spacePoints[i]->getPos().z()<zsign*tzn)
				{
					txn = spacePoints[i]->getPos().x();
					tyn = spacePoints[i]->getPos().y();
					tzn = spacePoints[i]->getPos().z();
				}
			}
			double tacphi1 = (txn-track_x0)/old_r;
			double tphi1 = -10.0;
			if (tacphi1 < -1.0 && tacphi1 >= -1.1) tacphi1 = -1;
			if (tacphi1 >  1.0 && tacphi1 <=  1.1) tacphi1 = 1;
			if (tacphi1 <= 1.0 && tacphi1 >= -1.0)
				tphi1 = acos(tacphi1);
			if ((tyn-track_y0)<0) tphi1 = 2*M_PI-tphi1;

			track_phi0 = fmod(tphi1-track_alpha*tzn, 2*M_PI);
			if (track_phi0 < 0.0) track_phi0 += 2*M_PI;
			*/
	
			track_npoints = spacePoints.size();
			recStatus = 2;
		}

	}
#endif
}

void SciFiTrack::solve3(SciFiReconstructionParams &recPars)
{
	double bcharge=recPars.getCharge();

	track_x0 = 0.0;
	track_y0 = 0.0;
	track_r = 0.0;
	track_alpha = 0.0;
	track_rms_alpha = 0.0;
	track_phi0 = 0.0;
	track_pt = 0.0;
	track_chi2 = 0.0;

	recStatus = 0;

	// Make sure we have enough points...
	if (spacePoints.size() < 3)
	{
		recStatus = -1;
		return;
	}

	// ...but not too many
	if (spacePoints.size() > 3)
	{
		recStatus = -2;
		return;
	}

	// Make sure they are all in different stations
	if (spacePoints[0]->getStationNo() == spacePoints[1]->getStationNo() 
	 || spacePoints[0]->getStationNo() == spacePoints[2]->getStationNo() 
	 || spacePoints[1]->getStationNo() == spacePoints[2]->getStationNo()) 
	{
		recStatus = -3;
		return;
	}

	// Make sure they are all in the same tracker
	if (spacePoints[0]->getTrackerNo() != spacePoints[1]->getTrackerNo() 
	 || spacePoints[1]->getTrackerNo() != spacePoints[2]->getTrackerNo()) 
	{
		recStatus = -4;
		return;
	}

	double x0, x1, x2, x3, y0, y1, y2, y3;
	double A, B, C, D;
	double r;

	x0 = 0.0;
	x1 = spacePoints[0]->getPos().x();
	x2 = spacePoints[1]->getPos().x();
	x3 = spacePoints[2]->getPos().x();
	y0 = 0.0;
	y1 = spacePoints[0]->getPos().y();
	y2 = spacePoints[1]->getPos().y();
	y3 = spacePoints[2]->getPos().y();

        z_ref = spacePoints[0]->getPos().z();
        if( spacePoints[1]->getPos().z() < z_ref )
          z_ref = spacePoints[1]->getPos().z();
        if( spacePoints[2]->getPos().z() < z_ref )
          z_ref = spacePoints[2]->getPos().z();

	// Degenerate/ambigous cases result in 0 denominators
	// just junk those (rare?) events for the moment
	if (x1-x2 != 0.0 && x1-x3 != 0.0)
	{
		A = (y2-y1)/(x1-x2);
		B = (x1*x1 - x2*x2 + y1*y1 - y2*y2)/(2*(x1-x2));
		C = (y3-y1)/(x1-x3);
		D = (x1*x1 - x3*x3 + y1*y1 - y3*y3)/(2*(x1-x3));

		if (A-C != 0.0)
		{
			y0 = (D-B)/(A-C);
			x0 = y0*A+B;

			r = (sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0))
			   + sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0))
			   + sqrt((x3-x0)*(x3-x0)+(y3-y0)*(y3-y0)))/3;
			double z1 =spacePoints[0]->getPos().z() - z_ref;
			double z2 =spacePoints[1]->getPos().z() - z_ref;
			double z3 =spacePoints[2]->getPos().z() - z_ref;

			if (r > 0 && fabs(x1-x0)/r > 1.0)
			{
			//	cout << "Recalculating r: 1 r = "<< r  << endl;
				r = 1/fabs(x1-x0);
			}
			if (r > 0 && fabs(x2-x0)/r > 1.0)
			{
			//	cout << "Recalculating r: 2 r = "<< r  << endl;
				r = 1/fabs(x2-x0);
			}
			if (r > 0 && fabs(x3-x0)/r > 1.0)
			{
			//	cout << "Recalculating r: 3 r = " << r << endl;
				r = 1/fabs(x3-x0);
			}

			if (r > 0 
			&& z1-z2 != 0.0 && z1-z3 != 0.0 && z2-z3 !=0
			&& fabs(x1-x0)/r <= 1.0 && fabs(x2-x0)/r <= 1.0
			&& fabs(x3-x0)/r <= 1.0)
			{
				double phi0, phi1, phi2, phi3; 
				double alpha, alpha1, alpha2, alpha3;
				double tsign;

				phi1 = acos((x1-x0)/r);
				if ((y1-y0)<0)
					phi1 = 2*M_PI-phi1;
				phi2 = acos((x2-x0)/r);
				if ((y2-y0)<0)
					phi2 = 2*M_PI-phi2;
				phi3 = acos((x3-x0)/r);
				if ((y3-y0)<0)
					phi3 = 2*M_PI-phi3;

				if (bcharge>0)
				{
				  tsign = (z2-z1)/fabs(z2-z1);
				  alpha1 = (phi2-phi1)/(z2-z1);
				  if (tsign*(phi2-phi1) > 0)
					alpha1 = -1.0*(2*M_PI-(phi2-phi1))/(z2-z1); 
				  tsign = (z3-z1)/fabs(z3-z1); 
				  alpha2 = (phi3-phi1)/(z3-z1);
				  if (tsign*(phi3-phi1) > 0)
					alpha2 = -1.0*(2*M_PI-(phi3-phi1))/(z3-z1);
				  tsign = (z3-z2)/fabs(z3-z2); 
				  alpha3 = (phi3-phi2)/(z3-z2);
				  if (tsign*(phi3-phi2) > 0)
					alpha3 = -1.0*(2*M_PI-(phi3-phi2))/(z3-z2); 
				}
				else
				{
				  tsign = (z2-z1)/fabs(z2-z1);
				  alpha1 = (phi2-phi1)/(z2-z1);
				  if (tsign*(phi2-phi1) < 0)
					alpha1 = (2*M_PI-(phi1-phi2))/(z2-z1);
				  tsign = (z3-z1)/fabs(z3-z1);
				  alpha2 = (phi3-phi1)/(z3-z1);
				  if (tsign*(phi3-phi1) < 0)
					alpha2 = (2*M_PI-(phi1-phi3))/(z3-z1);
				  tsign = (z3-z2)/fabs(z3-z2);
				  alpha3 = (phi3-phi2)/(z3-z2);
				  if (tsign*(phi3-phi2) < 0)
					alpha3 = (2*M_PI-(phi2-phi3))/(z3-z2);

				}

				alpha = (alpha1+alpha2+alpha3)/3.0;
				phi0 = fmod(phi1-alpha*z1, 2*M_PI);
				if (phi0 < 0.0) phi0 += 2*M_PI;
				//pt[MeV/c] = 2.998^-1*r*B[Tesla*mm]
				double pt=0.2998*r*recPars.getBField();

				track_x0 = x0;
				track_y0 = y0;
				track_r = r;
				track_alpha = alpha;
				track_rms_alpha =pow(alpha1-alpha,2);
				track_rms_alpha +=pow(alpha2-alpha,2);
				track_rms_alpha +=pow(alpha3-alpha,2);
				track_rms_alpha  /=3;
				track_rms_alpha =sqrt(track_rms_alpha);
				track_phi0 = phi0;
				track_pt = pt;

				recStatus = 1;

			}
			else
			{
			/*
			cout<<"CalcAlg: returning without pars 1"<<endl;
			cout << "r = " << r 
			<< " : z1-z2 = " << z1-z2 
			<< " : z1-z3 = " << z1-z3 
			<< " : z2-z3 = " << z2-z3 
			<< " : |X1-X0|/r = " << fabs(x1-x0)/r
			<< " : |X2-X0|/r = " << fabs(x2-x0)/r
			<< " : |X3-X0|/r = " << fabs(x3-x0)/r << endl;
			*/
			}
		}
		else
		{
			//cout << "CalcAlg: returning without pars 2" << endl;
		}
	}
}

int SciFiTrack::subset(SciFiTrack tr)
{
	// Return 1 if the points in tr are a subset (not necessarily
        // a proper subset) of this track

	if (tr.spacePoints.size() > spacePoints.size()) return 0;

	vector<int> used(0);
	for (unsigned int i = 0; i<tr.spacePoints.size(); i++)
		used.push_back(0);

	for (unsigned int i = 0; i < tr.spacePoints.size(); i++)
	{
		for (unsigned int j = 0; j < spacePoints.size(); j++)
		{
			double diff = (tr.spacePoints[i]->getPos()-spacePoints[j]->getPos()).mag2();
			if (diff==0)
				used[i] = 1;
		}
	}

	int tused = 1;
	for (unsigned int i = 0; i<used.size(); i++)
		tused *= used[i];

	return tused;
}

void SciFiTrack::addPoint(SciFiSpacePoint& sp)
{
	spacePoints.push_back(&sp);
	// re-sort the list
	sort(spacePoints.begin(), spacePoints.end());
	reverse(spacePoints.end(), spacePoints.begin());
}

void SciFiTrack::ProcessMC(SciFiReconstructionParams &recPars)
{
	Hep3Vector pos = Hep3Vector(0,0,0);
	Hep3Vector mom = Hep3Vector(0,0,0);
	for (vector<SciFiSpacePoint*>::const_iterator it = spacePoints.begin();
		it != spacePoints.end(); it++)
	{
		if (pos.z() == 0)
		{
			SciFiDoubletCluster *td = (*it)->getDoubletCluster(0);
			pos = td->getTruePosition();
			mom = td->getTrueMomentum();
			delete td;
		}
		else 
		{
			SciFiDoubletCluster *td = (*it)->getDoubletCluster(0);
			if (fabs(pos.z()) > fabs(td->getTruePosition().z()))
			{
				pos = td->getTruePosition();
				mom = td->getTrueMomentum();
			}
			delete td;
		}
	}

	// Extract the track parameters from the position and momentum
	double pt=sqrt(mom.x()*mom.x()+mom.y()*mom.y());
	if (pt == 0.0) return;

	double bcharge=recPars.getCharge();
	double r = pt/(0.2998*recPars.getBField());
	double alpha=-1*bcharge*fabs(pt/(r*mom.z()));
	double phi = acos(fabs(mom.y())/pt);
	if (bcharge > 0)
	{
	  if (mom.x() > 0 && mom.y() > 0) phi = M_PI - phi;
	  if (mom.x() > 0 && mom.y() < 0) phi = phi;
	  if (mom.x() < 0 && mom.y() > 0) phi = M_PI + phi;
	  if (mom.x() < 0 && mom.y() < 0) phi = 2*M_PI - phi;
	}
	else
	{
	  if (mom.x() > 0 && mom.y() > 0) phi = 2*M_PI - phi;
	  if (mom.x() > 0 && mom.y() < 0) phi = M_PI + phi;
	  if (mom.x() < 0 && mom.y() > 0) phi = phi;
	  if (mom.x() < 0 && mom.y() < 0) phi = M_PI - phi;
	}

	double x0 = pos.x() - r*cos(phi);
	double y0 = pos.y() - r*sin(phi);

	double phi0 = fmod(phi-alpha*pos.z(), 2*M_PI);
	if (phi0 < 0.0) phi0 += 2*M_PI;

	mc_track_x0 = x0;
	mc_track_y0 = y0;
	mc_track_r = r;
	mc_track_alpha = alpha;
	mc_track_phi0 = phi0;
	mc_track_pt = pt;
}

Hep3Vector SciFiTrack::GetPos(double zref, int flag) const
{
	if (flag)
	{
        	double phi = mc_track_alpha*zref+mc_track_phi0;

		return Hep3Vector(
	  	mc_track_r*cos(phi)+mc_track_x0, 
	  	mc_track_r*sin(phi)+mc_track_y0, zref);
	}

        double phi = track_alpha * ( zref - z_ref ) + track_phi0;

	return Hep3Vector(
	  track_r*cos(phi)+track_x0, 
	  track_r*sin(phi)+track_y0, zref);
}
	
Hep3Vector SciFiTrack::GetMom(double zref, int flag) const
{
  // Added protection against divide by zero AT 18/09/03 //
  if( recStatus == 2 || recStatus != 0 ) //ME
    {
	if (flag)
	{
        	double phi = mc_track_alpha*zref+mc_track_phi0;

		return Hep3Vector(
	  	mc_track_pt*sin(phi), 
	  	-1*mc_track_pt*cos(phi), 
	  	fabs(mc_track_pt/(mc_track_r*mc_track_alpha)));
	}

        double phi = track_alpha * ( zref - z_ref ) + track_phi0;

//        if( zref < 0 )
//          phi += 2.0943951;

        double px = track_pt * sin( phi );
        double py = -1.0 * track_pt * cos( phi );
        double pz = fabs( track_pt / ( track_r * track_alpha ) );

	return Hep3Vector( px, py, pz );
    }
  return Hep3Vector(0,0,0);
}

double SciFiTrack::GetMomentum() const
{

	double pz = fabs(track_pt/(track_r*track_alpha));
	double psqr = track_pt*track_pt + pz*pz;
	return sqrt(psqr);
}

double GetMomentum(double pt, double r, double alpha)
{
	double pz = fabs(pt/(r*alpha));
	double psqr = pt*pt + pz*pz;
	return sqrt(psqr);
}

double SciFiTrack::GetEnergy() const
{
	double mmu = 105.658389;
	double Esqr = GetMomentum()*GetMomentum() + mmu*mmu;
	return sqrt(Esqr);
}

double GetMomentumLoss(double X0Frac, double Mom)
{
	double mmu = 105.658389;
	double mel = 0.51099906;
	double K = 0.307075;
	double Ah = 1.01;
	double Zh = 1;
	double Ih = 1.0e-6*19.2;
	double Ac = 12.01;
	double Zc = 6;
	double Ic = 1.0e-6*78.0;
	double density = 43.8;
       

	double En = sqrt(Mom*Mom + mmu*mmu);
	double beta = Mom/En;
        double gamma = 1/sqrt(1-beta*beta);

	double TMax = 2*mel*beta*beta*gamma*gamma/
		(1+2*gamma*mel/mmu+(mel*mel)/(mmu*mmu));
	
	double tmp1, tmp2;

	tmp1 = K*Zh/(Ah*beta*beta);
	tmp2 = 2*mel*beta*beta*gamma*gamma*TMax/(Ih*Ih);
	double dedxh = tmp1*(0.5*log(tmp2)-beta*beta);

	tmp1 = K*Zc/(Ac*beta*beta);
	tmp2 = 2*mel*beta*beta*gamma*gamma*TMax/(Ic*Ic);
	double dedxc = tmp1*(0.5*log(tmp2)-beta*beta);

	double dedx = 0.076923*dedxh+0.923076*dedxc; //Weighting for polystyrene

	return density*dedx*X0Frac/beta;
}

#define NSTATION 5
vector<Hep3Vector> calcPoints(double *zrefs,double *pars, double bfield, 
		double bcharge)
{
	double momFac = -1.7;

	double x0s[NSTATION], y0s[NSTATION], rs[NSTATION];
        double alphas[NSTATION], phi0s[NSTATION], pts[NSTATION];
	x0s[0] = pars[0];
	y0s[0] = pars[1];
	rs[0] = pars[2];
	alphas[0] = pars[3];
	phi0s[0] = pars[4];
	pts[0] = rs[0]*(0.2998*bfield);

	vector<Hep3Vector> retVec(0);

	double zsign = zrefs[0]/fabs(zrefs[0]);

	double phi = alphas[0]*zrefs[0] + phi0s[0];
	retVec.push_back(Hep3Vector(
	  rs[0]*cos(phi)+x0s[0], 
	  rs[0]*sin(phi)+y0s[0], zrefs[0]));
	for (int i=1; i<NSTATION; i++)
	{
		/* Old version
		double tptot = GetMomentum(pts[i-1], rs[i-1], alphas[i-1]); 
		double tpz = fabs(pts[i-1]/(rs[i-1]*alphas[i-1]));
 		// X0 is for 1 station
		double ploss = GetMomentumLoss(0.2228775e-2, tptot);
		tpz = (tptot+zsign*ploss)*(tpz/tptot);
		pts[i] = (tptot+zsign*ploss)*(pts[i-1]/tptot);
		rs[i] = pts[i]/(0.2998*bfield);
		alphas[i] =-1*bcharge*fabs(pts[i]/(rs[i]*tpz));
		phi = alphas[i-1]*zrefs[i-1] + phi0s[i-1];
		phi0s[i] = (alphas[i-1]-alphas[i]) + phi0s[i-1];
		double deltar = rs[i]-rs[i-1];
		x0s[i] = x0s[i-1]+deltar*cos(phi);
		y0s[i] = y0s[i-1]+deltar*sin(phi);
		double phi = alphas[i]*zrefs[i] + phi0s[i];
		*/

		double tptot = GetMomentum(pts[i-1], rs[i-1], alphas[i-1]); 
		double tpz = fabs(pts[i-1]/(rs[i-1]*alphas[i-1]));
 		// X0 is for 1 station
		double ploss = momFac*GetMomentumLoss(0.2228775e-2, tptot);
		tpz = (tptot+zsign*ploss)*(tpz/tptot);
		pts[i] = (tptot+zsign*ploss)*(pts[i-1]/tptot);
		rs[i] = pts[i]/(0.2998*bfield);
		alphas[i] =-1*bcharge*fabs(pts[i]/(rs[i]*tpz));
		double phiLast = alphas[i-1]*zrefs[i-1] + phi0s[i-1];
		phi = phiLast + alphas[i]*(zrefs[i]-zrefs[i-1]);
		phi0s[i] = phi - zrefs[i]*alphas[i];
		double deltar = rs[i]-rs[i-1];
		x0s[i] = x0s[i-1]+deltar*cos(phi);
		y0s[i] = y0s[i-1]+deltar*sin(phi);

		retVec.push_back(Hep3Vector(
	  	  rs[i]*cos(phi)+x0s[i], 
	  	  rs[i]*sin(phi)+y0s[i], zrefs[i]));
	}
	return retVec;
}

SciFiTrack::~SciFiTrack()
{
	miceMemory.addDelete( Memory::SciFiTrack );
	spacePoints.clear();
}
