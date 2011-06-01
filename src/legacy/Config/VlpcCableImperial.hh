// MAUS WARNING: THIS IS LEGACY CODE.
/**
 * @author      Takashi Matsushita
 * @date        $Date: 2006-11-03 20:01:57 $
 * @version     $Revision: 1.7 $
 */
/*
 * Copyright:   (c) 2006 Takashi Matsushita
 * Licence:     All rights reserved.
 * Created:     6 Jul 2006
 * Credits:
 */

/** @todo */
/** @warnings */

#ifndef VlpcCableImperial_hh
#define VlpcCableImperial_hh
/*====================================================================*
 * declarations
 *====================================================================*/
/*--------------------------------------------------------------------*
 * headers
 *--------------------------------------------------------------------*/
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "Config/VlpcCable.hh"

/*--------------------------------------------------------------------*
 * constants
 *--------------------------------------------------------------------*/
/** name of configuration file for decoding */
const std::string CONFIG_FILE = "decode.txt";

/** commet character; if it is placed on the beginning of the line,
    the line is ignored */
const char COMMENT_CHAR = '#';

/** number of modules/cassette per afe board */
const int NUMBER_OF_MODULES = 8;

/** number of readout channels per mcm */
const int NUMBER_OF_CHANNELS_PER_MCM = 64;

/*--------------------------------------------------------------------*
 * classes
 *--------------------------------------------------------------------*/
/**
 *  This class implements data structure and methods needed to parse
 *  and use the fibre channel decoding information stored in text file.
 */
class VlpcCableImperial: public VlpcCable
{
  public:
    // -----------------------------------------------------------------
    // Constructors and Destructor
    // -----------------------------------------------------------------
    VlpcCableImperial() {};
    VlpcCableImperial(const std::string configFileName);

    // -----------------------------------------------------------------
    // Setter methods
    // -----------------------------------------------------------------
    void setConfigFileName(const std::string& name);

    // -----------------------------------------------------------------
    // Getter methods
    // -----------------------------------------------------------------
    const std::string getConfigFileName();

    // -----------------------------------------------------------------
    // Miscellaneous methods
    // -----------------------------------------------------------------
    /** decode file parser */
    bool parse();

    /** display setting defined in a configuration file */
    void showSetup();

    /** return true after decode files are parsed */
    bool isValid();

    /**
     *  map readout to fibre
     *  @param afe [in] afe board id
     *  @param mcm [in] mcm id
     *  @param channel [in] channel id of mcm
     *  @param station [out] station id
     *  @param plane [out] plane id
     *  @param fibre [out] fibre id
     *  @return true when successful mapping performed otherwise false
     */
   
    bool readout2fibre(int afe,
                       int mcm,
                       int channel,
                       int& station,
                       int& plane,
                       int& fibre);

    void statPlanFib(int afe,
                     int mcm,
                     int channel,
                     int& station, 
                     int& plane,
                     int& fibre);

    void readout2cassette(int afe,
                          int mcm,
                     	  int channel,
                          int& cassette, 
                          int& module,
                          int& channelCassette);
  
    void readout2waveguide(int afe,
                           int mcm,
                     	   int channel,
                           int& waveguide,
                           int& channelWaveguide);

    void readout2patchpanel(int afe,
                            int mcm,
                     	    int channel,
                            int& patchPanelCon, 
                            int& channelPatchPanel);

    bool readouthaspatchpanel(int afe,
                            int mcm,
                     	    int channel );

    void readout2station(int afe,
                         int mcm,
                     	 int channel,
                         int& stat,
                         int& statCon,
                         int& channelStation);

   int cassettes() const;

   int cassetteNum( int ) const;

   int LeftHandBoard( int ) const;

   int RightHandBoard( int ) const;

    /** convert and display mapping in 'osaka' convention */
    bool toOsaka();

    // functions to test the internal decoding and encoding of information

    bool testExternalCoding( int waveguideId, int holeId );

    bool testInternalCoding( int waveguideId, int connectorId, int holeId );
    
    bool testStationCoding( int stationId, int viewId, int connectorId, int holeId );

  private:
    bool isValid_;

    std::string configFileName_;

    struct Config {
      std::string fibreMap;
      std::string connection;
      std::string roLeft;
      std::string roRight;
    } config_;

    struct Readout {
      int left[NUMBER_OF_CHANNELS_PER_MCM];
      int right[NUMBER_OF_CHANNELS_PER_MCM];
    } readout_;

    struct Afe {
      int externalWaveguide[NUMBER_OF_MODULES];
      int cassette;
      bool isLeft;
    };

    typedef std::map<int, Afe *> IntAfePtrMap;
    IntAfePtrMap intAfePtrMap_;

    typedef std::map<int, int> StationConnectorMap;
    StationConnectorMap stationConnector_;

    typedef std::map<int, int> InternalWaveguideMap;
    InternalWaveguideMap internalWaveguide_;

    typedef std::map<int, int> ExternalWaveguideMap;
    ExternalWaveguideMap externalWaveguide_;


    // -----------------------------------------------------------------
    // Methods
    // -----------------------------------------------------------------
    bool parseConfig();
    void showConfig();

    bool parseReadout(const std::string& fileName,
                      int *array);
    void showReadout();

    bool parseConnection();
    void showConnection();

    bool parseFibremap();
    void showFibremap();

    int encodeExternal(int waveguideId,
                       int d0holeId);
    int encodeInternal(int waveguideId,
                       int connectorId,
                       int holeId);
    int encodeStation(int stationId,
                      int viewId,
                      int connectorId,
                      int holeId);

    void decodeExternal(int encodedExternal,
                        int& waveguideId,
                        int& holeId);

    void decodeInternal(int encodedInternal,
                        int& waveguideId,
                        int& connectorId,
                        int& holeId);

    void decodeStation(int encodedInternal,
                       int& stationId,
                       int& viewId,
                       int& connectorId,
                       int& holeId);

    void printExternal(int encoded);
    void printInternal(int encoded);
    void printStation(int encoded);

    void decodeStation(int encoded,
                       int& station,
                       int& view);
};

namespace TM {
/*--------------------------------------------------------------------*
 * function
 *--------------------------------------------------------------------*/
/**
 *  remove leading and trailing white characters from string
 *
 *  @param source [in] input string
 *  @param whites [in] white characters
 *  @return trimmed string
 */
std::string trim(std::string const& source,
                 char const *whites = " \t\r\n");

/**
 *  tokenise a string with delimiters
 *  @param source [in] input string
 *  @param tokens [out] array of tokens
 *  @param delimiters [in] delimiters
 */
void tokenise(const std::string& str,
              std::vector<std::string>& tokens,
              const std::string& delimiters = " ");

} // namespace TM

#endif // VlpcCableImperial_hh
/* eof */
