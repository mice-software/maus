// Memory.hh
//
// Class to keep track of how many times each of the G4MICE classes has been created and destroyed
//
// static const strings are used to refer to each class name and maps are used to keep track of new and delete
// operations.
//
// This class is used in MICETESTS but can also be used by user codes to check for memory leaks
//
// M.Ellis 1st November 2006

#ifndef MEMORY_HH
#define MEMORY_HH

#include <string>
#include <map>
#include <vector>

class Memory
{
  public :

    Memory();

    ~Memory();

    void addNew( std::string );

    void addDelete( std::string );

    int numNews( std::string ) const;

    int numDeletes( std::string ) const;

    bool isLeaking( std::string ) const;

    bool anyLeaking() const;

    void reset();

    void printReport() const;

    void writeReport( std::ostream& ) const;

    // MICE classes that are persistent

    static const std::string VlpcHit;
    static const std::string VlpcEventTime;
    static const std::string CkovHit;
    static const std::string CkovDigit;
    static const std::string EMRHit;
    static const std::string EMRDigit;
    static const std::string KLHit;
    static const std::string KLDigit;
    static const std::string KLCellHit;
    static const std::string SciFiHit;
    static const std::string SciFiDigit;
    static const std::string SciFiDoubletCluster;
    static const std::string SciFiSpacePoint;
    static const std::string SciFiTrack;
    static const std::string SciFiKalTrack;
    static const std::string SciFiExtrap;
    static const std::string TofHit;
    static const std::string TofDigit;
    static const std::string TofSlabHit;
    static const std::string TofSpacePoint;
    static const std::string VmeAdcHit;
    static const std::string VmefAdcHit;
    static const std::string VmeTdcHit;
    static const std::string VmeScalerData;
    static const std::string EmCalHit;
    static const std::string EmCalDigit;
    static const std::string EmCalTrack;
    static const std::string VirtualHit;
    static const std::string SpecialHit;
    static const std::string MCParticle;
    static const std::string MCVertex;
    static const std::string ZustandVektor;
    static const std::string RFData;

    // Config classes

    static const std::string BeamParameters;
    static const std::string BeamlineGeometry;
    static const std::string BeamlineParameters;
    static const std::string CoolingChannelGeom;
    static const std::string MagnetParameters;
    static const std::string MiceModule;
    static const std::string RFBackgroundParameters;
    static const std::string RFParameters;
    static const std::string SciFiCableManager;
    static const std::string TofCable;
    static const std::string VirtualGeomParameters;
    static const std::string VlpcCableImperial;
    static const std::string VlpcCableOsaka;
    static const std::string PidFits;

    // DetModel classes

    static const std::string CKOVSD;
    static const std::string EMRSD;
    static const std::string KLFiber;
    static const std::string KLGlue;
    static const std::string KLSD;
    static const std::string EmCalKLFiber;
    static const std::string EmCalKLGlue;
    static const std::string EmCalSD;
    static const std::string DoubletFiberParam;
    static const std::string SciFiPlane;
    static const std::string SciFiSD;
    static const std::string TofSD;
    static const std::string SpecialDummySD;
    static const std::string SpecialVirtualSD;
    static const std::string VirtualSD;
    static const std::string VirtualStation;

    // EngModel classes

    static const std::string AbsorberVessel;
    static const std::string MiceElectroMagneticField;
    static const std::string MiceMagneticField;
    static const std::string Polycone;
    static const std::string Q35;

    // TextFilIo persistency classes

    static const std::string PersistClasses;
    static const std::string TextFileReader;
    static const std::string TextFileWriter;
    static const std::string VlpcHitTextFileIo;
    static const std::string VlpcEventTimeTextFileIo;
    static const std::string CkovHitTextFileIo;
    static const std::string CkovDigitTextFileIo;
    static const std::string SciFiHitTextFileIo;
    static const std::string SciFiDigitTextFileIo;
    static const std::string SciFiDoubletClusterTextFileIo;
    static const std::string SciFiSpacePointTextFileIo;
    static const std::string SciFiTrackTextFileIo;
    static const std::string SciFiKalTrackTextFileIo;
    static const std::string SciFiExtrapTextFileIo;
    static const std::string TofHitTextFileIo;
    static const std::string TofDigitTextFileIo;
    static const std::string TofSlabHitTextFileIo;
    static const std::string TofSpacePointTextFileIo;
    static const std::string VmeAdcHitTextFileIo;
    static const std::string VmefAdcHitTextFileIo;
    static const std::string VmeTdcHitTextFileIo;
    static const std::string VmeScalerDataTextFileIo;
    static const std::string KLHitTextFileIo;
    static const std::string EmCalHitTextFileIo;
    static const std::string EmCalDigitTextFileIo;
    static const std::string EmCalTrackTextFileIo;
    static const std::string VirtualHitTextFileIo;
    static const std::string SpecialHitTextFileIo;
    static const std::string MCParticleTextFileIo;
    static const std::string MCVertexTextFileIo;
    static const std::string ZustandVektorTextFileIo;
    static const std::string RFDataTextFileIo;

    // DATEFilOut persistency classes

    static const std::string V1290DATEFileOut;
    static const std::string V1724DATEFileOut;
    static const std::string V1731DATEFileOut;
    static const std::string V830DATEFileOut;

  private :

    std::map<std::string,int> _news;
    std::map<std::string,int> _deletes;
    std::vector<std::string>  _classes;
};

extern Memory miceMemory;

#endif

