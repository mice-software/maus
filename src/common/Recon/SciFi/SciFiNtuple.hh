// ntuple.hh - struct containing the information to be written out for 
// the Sci Fi tracker performance analysis

#ifndef REC_SCIFI_NTUPLE_HH
#define REC_SCIFI_NTUPLE_HH

class SciFiNtuple
{
   public: 

   // statistics

   int nhits;
   int nclusts;
   int npoints;
   int ntriplets;
   int nduplets;
   int ntracks;
 
   // points

   double x1[2][5];
   double y1[2][5];
   double z1[2][5];
   double mcx1[2][5];
   double mcy1[2][5];
   double mcz1[2][5];

   // tracks

   double chi2[2];
   int numpoints[2];
   int numclusts[2];
   int numclustsmu[2];
   double x[2];
   double y[2];
   double z[2];
   double xe[2];
   double ye[2];
   double mcx[2];
   double mcy[2];
   double px[2];
   double py[2];
   double pz[2];
   double pxe[2];
   double pye[2];
   double pze[2];
   double time[2];
   double timeE[2];
   double mcpx[2];
   double mcpy[2];
   double mcpz[2];
   double mctime[2];

   // default constructor

   SciFiNtuple()
   {
     nhits = nclusts = npoints = ntriplets = nduplets = ntracks = 0;
     for( int t  = 0; t < 2; ++t )
     {
       for( int s = 0; s < 5; ++s )
         x1[t][s] = y1[t][s] = z1[t][s] = mcx1[t][s] = mcy1[t][s] = mcz1[t][s] = -99999.9;
       chi2[t] = -1;
       numpoints[t] = numclusts[t] = numclustsmu[t] = 0;
       x[t] = y[t] = z[t] = xe[t] = ye[t] = mcx[t] = mcy[t] = -99999.9;
       px[t] = py[t] = pz[t] = pxe[t] = pye[t] = pze[t] = mcpx[t] = mcpy[t] = mcpz[t] = -99999.9;
       time[t] = mctime[t] = -99999.9;
       timeE[t] = 0;
     }
   }

   // method to write the ntuple out..

   void write()
   {
     std::cout << std::scientific;

     std::cout << "SCIFINTUPLE " << nhits << " " << nclusts << " " << npoints << " "
                                 << ntriplets << " " << nduplets << " " << ntracks << std::endl;

     for( int tracker = 0; tracker < 2; ++tracker )
       for( int station = 0; station < 5; ++station )
         std::cout << "SCIFINTUPLE " << x1[tracker][station] << " " << y1[tracker][station] << " "
                                     << z1[tracker][station] << " " << mcx1[tracker][station] << " "
                                     << mcy1[tracker][station] << " " << mcz1[tracker][station]
                                     << std::endl;

     for( int tracker = 0; tracker < 2; ++tracker )
     {
       if( chi2[tracker] <= 0 )
         chi2[tracker] += 1e-6;

       std::cout << "SCIFINTUPLE " << chi2[tracker] << " " << numpoints[tracker] << " "
                                   << numclusts[tracker] << " " << numclustsmu[tracker] << std::endl;

       std::cout << "SCIFINTUPLE " << x[tracker] << " " << y[tracker] << " " << z[tracker] << " "
                                   << xe[tracker] << " " << ye[tracker] << " " << mcx[tracker] << " "
                                   << mcy[tracker] << std::endl;
 
       std::cout << "SCIFINTUPLE " << px[tracker] << " " << py[tracker] << " " 
                                   << pz[tracker] << std::endl;
       std::cout << "SCIFINTUPLE " << pxe[tracker] << " " << pye[tracker] << " " 
                                   << pze[tracker] << std::endl;
       std::cout << "SCIFINTUPLE " << mcpx[tracker] << " " << mcpy[tracker] << " "
                                   << mcpz[tracker] << std::endl;
       std::cout << "SCIFINTUPLE " << time[tracker] << " " << timeE[tracker] << " " << mctime[tracker] << std::endl;
     }
   }
};

#endif

