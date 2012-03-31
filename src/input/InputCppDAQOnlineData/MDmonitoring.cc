/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "src/input/InputCppDAQOnlineData/MDmonitoring.hh"

MDmonitoring::MDmonitoring(std::string aMonSrc, std::string aProgName)
: _monSrc("@" + aMonSrc + ":"), _progName(aProgName), _status(0), _asyncRead(true),
  _useTable(false), _useAttributes(false), _useStatic(false), _dataBuffer(NULL), _fRunNumber(0) {
}

MDmonitoring::~MDmonitoring() {
  if (!_useStatic) {
    free(_dataBuffer);
    _dataBuffer = NULL;
  }
}

void MDmonitoring::Init() {
  // Check the monitoring source
  std::cout << "Start Init..." << std::endl;
  std::cout << "Monitoring source is : " << _monSrc << "." << std::endl;
  std::cout << "Program name is : " << _progName << "." << std::endl;
  if (!_monSrc.size()) {
    std::cerr << "Error in MDmonitoring::Init: Monitoring source is not valid ("
    << _monSrc << ")." << std::endl;
    exit(1);
  }

  char* mon = const_cast<char*> (_monSrc.c_str());
  if ((_status = monitorSetDataSource(mon)) != 0) {
    std::cerr << "Error in MDmonitoring::Init( monitorSetDataSource ):"
    << "Cannot attach to monitor scheme. Status: "
    << monitorDecodeError(_status) << std::endl;
    exit(_status);
  }

  // Set the monitoring program name.
  // Can be used to check if the app is connected to monitoring daemon
  // by starting 'monitorClients' on the machine running the daemon.
  char* program = const_cast<char*> (_progName.c_str());
  if ((_status = monitorDeclareMp(program)) != 0) {
    std::cerr << "Error in MDmonitoring::Init (monitorDeclareMp): "
    << monitorDecodeError(_status) << std::endl;
    exit(_status);
  }

  // Set Swap Mode
  if ((_status = monitorSetSwap(false, false)) != 0) {
    std::cerr << "Error in MDmonitoring::Init (monitorSetSwap):"
    << "Cannot set swapping mode. Status: "
    << std::dec << _status << " (" << std::hex << _status << std::dec
    << "), " << monitorDecodeError(_status) << std::endl;
    exit(_status);
  }

  if (_asyncRead) {
    if ((_status = monitorSetNowait()) != 0) {
      std::cerr << "Error in MDmonitoring::Init (monitorSetNowait):"
      << "Cannot set Asynchronous mode. Status: " << monitorDecodeError(_status) << std::endl;
      exit(_status);
    }
  }

  if (_useTable) {
    if (_useAttributes)
      _status = monitorDeclareTableWithAttributes(_monitorTable);
    else
      _status = monitorDeclareTable(_monitorTable);

    if (_status != 0) {
      std::cerr << "Error in MDmonitoring::Init, during monitoring table declaration"
      << (_useAttributes ? " with attributes." : ".") << " Status: "
      << monitorDecodeError(_status) << std::endl;
      exit(_status);
    }
  }
  if (_useStatic) {
    // _dataBuffer = (unsigned char *)_staticDataBuffer;
    _dataBuffer =  reinterpret_cast<unsigned char *>(_staticDataBuffer);
  }

  std::cout << "Done Init." << std::endl;
}

unsigned char * MDmonitoring::GetNextEvent() {
  struct eventHeaderStruct *event;
  std::cout << "Getting event " << std::endl;
  bool again = true;
  int tries(0);
  int DOTS_AT(5000);
  //  std::cout << "free data Buffer if needed..." << std::endl;
  if (_dataBuffer) {
    free(_dataBuffer);
    _dataBuffer = NULL;
  }

  // dataBuffer = new unsigned char;
  while (again) {
    again = false;
    if (_useStatic) {
      // std::cout << "Trying to get event in static data buffer..." << std::endl;
      _status = monitorGetEvent(_staticDataBuffer, sizeof(_staticDataBuffer));
      // std::cout << "...Done" << std::endl;
      std::cout << "Read event statically. Status = " << _status << std::endl;
    } else {
      // std::cout << "Trying to get event in dynamic data buffer..." << std::endl;
      // _status = monitorGetEventDynamic((void **)&_dataBuffer);
      _status = monitorGetEventDynamic(reinterpret_cast<void **>(&_dataBuffer));
      // std::cout << "...Done, status = " << monitorDecodeError(status) << std::endl;
      // std::cout << "Read event dynamically. Status = " << status << std::endl;
    }
    if (_asyncRead && (_status == 0)) {
      // std::cout << "Retrying" << std::endl;
      if ((_useStatic && *reinterpret_cast<long32 *>(_staticDataBuffer) == 0) ||
          (!_useStatic && _dataBuffer == NULL)) {
      // if ( ( (++tries) % DOTS_AT ) == 0 ) { std::cout <<  "*" << std::flush; }
        again = true;
      }
    }
  }

  if (_asyncRead && tries >= DOTS_AT) std::cout << "." << std::endl;
  if (_dataBuffer) {
    event = (struct eventHeaderStruct *)_dataBuffer;

    _fRunNumber = event->eventRunNb;

    std::cout << "Run #: "<< event->eventRunNb << std::endl
    << "Event #: "<< event->eventId << std::endl
    << "Type: " << event->eventType << std::endl
    << "Length: " << event->eventSize << std::endl
    << "Data size: " << (event->eventSize - event->eventHeadSize ) << std::endl;
  }

  return _dataBuffer;
}


