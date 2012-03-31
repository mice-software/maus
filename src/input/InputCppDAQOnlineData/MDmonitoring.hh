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

#ifndef __MD_MONITORING_H__
#define __MD_MONITORING_H__

#include <stdlib.h>

#include <string>
#include <iostream>
#include <cctype>

#include "unpacking/monitor.h"
#include "unpacking/event.h"


class MDmonitoring {
 public:
  // Constructor and destructor
  // aMonSrc - monitoring source
  //   : --> local online,
  //   @host: --> remote online;
  // For more check DATE documentation.
  MDmonitoring(std::string _aMonSrc = ":", std::string _aProgName = "OnRec Monitoring Program");
  virtual ~MDmonitoring();

  void setAsyncRead(bool aValue) {_asyncRead = aValue;}
  bool getAsyncRead() {return _asyncRead;}

  void setUseAttrib(bool aValue) {_useAttributes = aValue;}
  bool getUseAttrib() {return _useAttributes;}

  void setUseTable(bool aValue) {_useTable = aValue;}
  bool getUseTable() {return _useTable;}

  void setUseStatic(bool aValue) {_useStatic = aValue;}
  bool getUseStatic() {return _useStatic;}

  void        setMonSrc(std::string mon) {_monSrc = mon;}
  std::string getMonSrc() {return _monSrc;}

  void Init();
  unsigned char * GetNextEvent();

  // void FreeBuffer();

  private:
  std::string _monSrc;
  std::string _progName;
  int _status;
  bool _asyncRead;
  bool _useTable;
  bool _useAttributes;
  bool _useStatic;
  int _staticDataBuffer[5000000];
  unsigned char *_dataBuffer;
  char ** _monitorTable;
  unsigned _fRunNumber;
};

#endif
