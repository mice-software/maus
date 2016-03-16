/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#include "DataStructure/Spill.hh"

namespace MAUS {

int Spill::reference_count = 0;

Spill::Spill()
        : _daq(NULL), _scalars(NULL), _mc(NULL), _recon(NULL), _emr(NULL),
          _spill_number(0), _run_number(0), _event_id(0), _time_stamp(0),
          _daq_event_type(), _errors(), _test(NULL) {
}

Spill::Spill(const Spill& md)
        : _daq(NULL), _scalars(NULL), _mc(NULL), _recon(NULL), _emr(NULL),
          _spill_number(0), _run_number(0), _event_id(0), _time_stamp(0),
          _daq_event_type(), _errors(), _test(NULL) {
    *this = md;
}

Spill& Spill::operator=(const Spill& md) {
    if (this == &md) {
        return *this;
    }

    if (_daq != NULL) {
        delete _daq;
    }
    if (md._daq == NULL) {
        _daq = NULL;
    } else {
        _daq = new DAQData(*md._daq);
    }

    if (_scalars != NULL) {
        delete _scalars;
    }
    if (md._scalars == NULL) {
        _scalars = NULL;
    } else {
        _scalars = new Scalars(*md._scalars);
    }

    if (_mc != NULL) {
        for (size_t i = 0; i < _mc->size(); ++i) {
            delete (*_mc)[i];
        }
        delete _mc;
    }
    if (md._mc == NULL) {
        _mc = NULL;
    } else {
        _mc = new MCEventPArray(*md._mc);
        for (size_t i = 0; i < _mc->size(); ++i)
            _mc->at(i) = new MCEvent(*_mc->at(i));
    }

    if (_recon != NULL) {
        for (size_t i = 0; i < _recon->size(); ++i) {
            delete (*_recon)[i];
        }
        delete _recon;
    }
    if (md._recon == NULL) {
        _recon = NULL;
    } else {
        _recon = new ReconEventPArray(*md._recon);
        for (size_t i = 0; i < _recon->size(); ++i)
            _recon->at(i) = new ReconEvent(*_recon->at(i));
    }

    if (_emr != NULL) {
        delete _emr;
    }
    if (md._emr == NULL) {
        _emr = NULL;
    } else {
        _emr = new EMRSpillData(*md._emr);
    }

    _daq_event_type = md._daq_event_type;
    _spill_number = md._spill_number;
    _run_number = md._run_number;
    _event_id = md._event_id;
    _time_stamp = md._time_stamp;
    _errors = md._errors;
    if (md._test == NULL) {
        _test = NULL;
    } else {
        _test = new TestBranch(*md._test);
    }
    return *this;
}

Spill::~Spill() {
    if (_daq != NULL) {
        delete _daq;
    }
    if (_scalars != NULL) {
        delete _scalars;
    }

    if (_mc != NULL) {
        for (size_t i = 0; i < _mc->size(); ++i) {
            delete (*_mc)[i];
        }
        delete _mc;
    }
    if (_recon != NULL) {
        for (size_t i = 0; i < _recon->size(); ++i) {
            delete (*_recon)[i];
        }
        delete _recon;
    }
    if (_emr != NULL) {
        delete _emr;
    }
    if (_test != NULL) {
        delete _test;
    }
}

void Spill::SetScalars(Scalars *scalars) {
  if (_scalars != NULL)
      delete _scalars;
  _scalars = scalars;
}

Scalars* Spill::GetScalars() const {
  return _scalars;
}

void Spill::SetMCEvents(MCEventPArray* mc) {
  if (_mc != NULL && mc != _mc) {
      for (size_t i = 0; i < _mc->size(); ++i)
          delete (*_mc)[i];
      delete _mc;
  }
  _mc = mc;
}

MCEventPArray* Spill::GetMCEvents() const {
  return _mc;
}

void Spill::SetReconEvents(ReconEventPArray* recon) {
  if (_recon != NULL && recon != _recon) {
      for (size_t i = 0; i < _recon->size(); ++i)
          delete (*_recon)[i];
      delete _recon;
  }
  _recon = recon;
}

ReconEventPArray* Spill::GetReconEvents() const {
  return _recon;
}

void Spill::SetEMRSpillData(EMRSpillData* emr) {
  if (_emr != NULL && emr != _emr) {
      delete _emr;
  }
  _emr = emr;
}

EMRSpillData* Spill::GetEMRSpillData() const {
  return _emr;
}

void Spill::SetDAQData(DAQData *daq) {
  if (_daq != NULL && daq != _daq) {
      delete _daq;
  }
  _daq = daq;
}

DAQData* Spill::GetDAQData() const {
  return _daq;
}

void Spill::SetSpillNumber(int spill) {
  _spill_number = spill;
}

int Spill::GetSpillNumber() const {
  return _spill_number;
}

void Spill::SetRunNumber(int spill) {
  _run_number = spill;
}

int Spill::GetRunNumber() const {
  return _run_number;
}

void Spill::SetEventId(int eid) {
  _event_id = eid;
}

int Spill::GetEventId() const {
  return _event_id;
}

void Spill::SetTimeStamp(int ts) {
  _time_stamp = ts;
}

int Spill::GetTimeStamp() const {
  return _time_stamp;
}

void Spill::SetDaqEventType(std::string type) {
    _daq_event_type = type;
}

std::string Spill::GetDaqEventType() const {
    return _daq_event_type;
}

void Spill::SetErrors(ErrorsMap errors) {
  _errors = errors;
}

ErrorsMap Spill::GetErrors() const {
  return _errors;
}

TestBranch* Spill::GetTestBranch() const {
  return _test;
}

void Spill::SetTestBranch(TestBranch* test) {
  if (_test != NULL && test != _test)
    delete _test;
  _test = test;
}
}

