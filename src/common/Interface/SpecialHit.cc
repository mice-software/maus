//----------------------------------------------------------------------------
//
//     File :       SpecialHit.cc
//     Purpose :    MICE Virtual detector for the MICE proposal.  Describe
//                  a single hit. Many of these variables listed would not be
//                  known in the real experiment.  This hit bank is designed
//                  to be functionallly similar to the SciFiHit bank by E.
//                  McKigney for the simulation
//     Created :    Steve Kahn - Sept 16, 2002
//
//----------------------------------------------------------------------------

#include "Interface/SpecialHit.hh"

using namespace std;

SpecialHit::SpecialHit()
{
miceMemory.addNew( Memory::SpecialHit ); 

	_stationNumber = 0;
	_cellNumber    = 0;
	_volName       = "";
	_bfield        = Hep3Vector(0,0,0);
	_efield        = Hep3Vector(0,0,0);      
	_iax           = 0;
	_irad          = 0;
	_iaz           = 0;
	_parentrackID  = 0;
	_ancestrackID  = 0;
	_vtxpos        = Hep3Vector(0., 0., 0.);
	_vtxtime       = 0.;
	_vtxmom        = Hep3Vector(0., 0., 0.);
	_stepType      = 0;
	_stepLength    = 0;
	_pathLength    = 0;
}

SpecialHit::SpecialHit(const SpecialHit& th)
{
miceMemory.addNew( Memory::SpecialHit ); 
	if(&th != this) *this = th;
}

const SpecialHit& SpecialHit::operator=(const SpecialHit& th)
{
	_iax  =          th.GetZnumber();
	_irad  =         th.GetRnumber();
	_iaz  =          th.GetPhiNumber();
	_stepType     =  th.GetStepType();
	_stepLength   =  th.GetStepLength();
	_stationNumber = th.GetStationNo();
	_cellNumber =    th.GetCellNo();
	_volName =       th.GetVolumeName();
	_bfield =        th.GetBField();
	_efield =        th.GetEField();
	_parentrackID =  th.GetParenTrackID();
	_ancestrackID =  th.GetAncesTrackID();
	_pathLength   =  th._pathLength;

	_vtxpos  = th.GetVtxPosition();
	_vtxtime = th.GetVtxTime();
	_vtxmom  = th.GetVtxMomentum();


	return *this;
}

int  SpecialHit::stationIDfromName( std::string vname ) const //ME - copied from DetModel, not at all ideal solution!!!!
{
  const int nStationTypes = 12;
  const int maxNumberOfVols = 1000;
  std::string stationNames[nStationTypes] = {
   "FOCUS", "MATCH", "COUPLING", "SOLENOID", "ENDCOIL", "ABSORBER", "WINDOWABS", "WINDOWVAC", "TRACKREF", "CAVITYREF", "WINDOWF", "TOFREF" };

  int stationNameInt = 0;
  int endValue = 0;

 //loop over the stationNames
  for( int i=0; i<nStationTypes; i++)
  {
    const char *inputVname = vname.c_str();
    const char *refVname = stationNames[i].c_str();
    size_t nChars = strlen(inputVname);
    //see if any of the last three characters are numbers
                for( int j=1; j<4; j++)
                  //if the input string is the same as the
            if(strncmp(refVname, inputVname, nChars-j) == 0)
            {
              stationNameInt = i+1;
              //this loop produces things like 1;12;123...
              for( int k=0; k<j; k++)
        {
          char endChar = inputVname[nChars-k-1]; // should be numerical character at end
          endValue *= 10; //endValue multiplies by 10 each time
          endValue += (endChar - 0x30);  // then add the number on
                                }
                                //break out of the nested loops
                    j=4;
                    i=nStationTypes;
            }
  }

  int stationID = stationNameInt * maxNumberOfVols + endValue;

  return stationID;
}

void SpecialHit::DecodeVolumeName()
{	int iplane;
	_stationNumber = -1;
   //	sscanf(_volName.c_str(), "Virtual%d", &iplane);
	const char *cvolName = _volName.c_str();
	char csname[10] = "";
	strcpy(csname, &cvolName[4]);
	string strippedName = csname;

	// ME - this will need to be set elsewhere!
        //plane = SpecialVirtualStation::stationIDfromName(strippedName);

        iplane = stationIDfromName(strippedName);

	_stationNumber = iplane;
}

void SpecialHit::SetBField( double b[3])
{
    _bfield = Hep3Vector(b[0], b[1], b[2]);
}


void SpecialHit::SetEField( double e[3])
{
    _efield = Hep3Vector(e[0], e[1], e[2]);
}

void SpecialHit::addEdep(double edep)
{   setEdep( Edep() + edep); }
