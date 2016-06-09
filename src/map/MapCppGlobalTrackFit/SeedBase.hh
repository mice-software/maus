#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"

#ifndef SRC_MAP_MAPCPPGLOBALTRACKFIT_SEEDBASE_HH
#define SRC_MAP_MAPCPPGLOBALTRACKFIT_SEEDBASE_HH

namespace MAUS {

class ReconEvent;

class SeedBase {
  public:
    
    virtual ~SeedBase() {}
    virtual Kalman::State GetSeed(ReconEvent* reco_event) = 0;

  protected:
    static const double mu_mass;
    static const double c_light;
};

class USTrackerTofSeed : public SeedBase {
  public:
    USTrackerTofSeed() {}
    ~USTrackerTofSeed() {}
    virtual Kalman::State GetSeed(ReconEvent* reco_event);

};

class Tof01Seed : public SeedBase {
  public:
    Tof01Seed() {}
    ~Tof01Seed() {}
    virtual Kalman::State GetSeed(ReconEvent* reco_event);
};

class USTrackerTrackFitSeed : public SeedBase {
  public:
    USTrackerTrackFitSeed() {}
    ~USTrackerTrackFitSeed() {}
    virtual Kalman::State GetSeed(ReconEvent* reco_event);

  private:
};



class SeedManager {
  public:
    typedef std::map<std::string, SeedBase*>::iterator iterator;
    SeedManager() {RegisterAll();}
    ~SeedManager();
    /** Register an algorithm
     *    - name: name of the algorithm
     *    - algorithm: the algorithm; if NULL, attempt to remove the algorithm
     *                 with name or throw an exception on failure
     *  SeedManager takes ownership of memory allocated to algorithm
     */
    void RegisterAlgorithm(std::string name, SeedBase* algorithm);

    /** Get seed-finding algorithm with a given name
     *    - name: name of the algorithm
     *  Return equivalent algorithm (registered using RegisterAlgorithm)
     */
    SeedBase* GetAlgorithm(std::string name) {return _seed_algorithms[name];}

    void RegisterAll();

  private:
    std::map<std::string, SeedBase*> _seed_algorithms;
};

} // namespace MAUS

#endif  // #ifndef SRC_MAP_MAPCPPGLOBALTRACKFIT_SEEDBASE_HH

