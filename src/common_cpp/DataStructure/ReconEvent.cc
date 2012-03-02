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

#include "src/common_cpp/DataStructure/ReconEvent.hh"

namespace MAUS {

ReconEvent::ReconEvent() :_tof_event(NULL), _scifi_event(NULL),
    _ckov_event(NULL), _kl_event(NULL), _emr_event(NULL), _trigger_event(NULL),
    _global_event(NULL) {
}

ReconEvent::ReconEvent(const ReconEvent& md)
          : _tof_event(NULL), _scifi_event(NULL),
            _ckov_event(NULL), _kl_event(NULL), _emr_event(NULL),
            _trigger_event(NULL), _global_event(NULL) {
    *this = md;
}

ReconEvent& ReconEvent::operator=(const ReconEvent& recon) {
    if (this == &recon) {
        return *this;
    }

    if (_tof_event != NULL) {
        delete _tof_event;
    }
    if (recon._tof_event == NULL) {
        _tof_event = NULL;
    } else {
        _tof_event = new TOFEvent(*recon._tof_event);
    }

    if (_scifi_event != NULL) {
        delete _scifi_event;
    }
    if (recon._scifi_event == NULL) {
        _scifi_event = NULL;
    } else {
        _scifi_event = new SciFiEvent(*recon._scifi_event);
    }

    if (_ckov_event != NULL) {
        delete _ckov_event;
    }
    if (recon._ckov_event == NULL) {
        _ckov_event = NULL;
    } else {
        _ckov_event = new CkovEvent(*recon._ckov_event);
    }

    if (_kl_event != NULL) {
        delete _kl_event;
    }
    if (recon._kl_event == NULL) {
        _kl_event = NULL;
    } else {
        _kl_event = new KLEvent(*recon._kl_event);
    }

    if (_emr_event != NULL) {
        delete _emr_event;
    }
    if (recon._emr_event == NULL) {
        _emr_event = NULL;
    } else {
        _emr_event = new EMREvent(*recon._emr_event);
    }

    if (_trigger_event != NULL) {
        delete _trigger_event;
    }
    if (recon._trigger_event == NULL) {
        _trigger_event = NULL;
    } else {
        _trigger_event = new TriggerEvent(*recon._trigger_event);
    }

    if (_global_event != NULL) {
        delete _global_event;
    }
    if (recon._global_event == NULL) {
        _global_event = NULL;
    } else {
        _global_event = new GlobalEvent(*recon._global_event);
    }
    return *this;
}

ReconEvent::~ReconEvent() {
    if (_tof_event != NULL) {
        delete _tof_event;
    }
    if (_scifi_event != NULL) {
        delete _scifi_event;
    }
    if (_ckov_event != NULL) {
        delete _ckov_event;
    }
    if (_kl_event != NULL) {
        delete _kl_event;
    }
    if (_emr_event != NULL) {
        delete _emr_event;
    }
    if (_trigger_event != NULL) {
        delete _trigger_event;
    }
    if (_global_event != NULL) {
        delete _global_event;
    }
}

TOFEvent* ReconEvent::GetTOFEvent() const {
    return _tof_event;
}

void ReconEvent::SetTOFEvent(TOFEvent* event) {
    if (_tof_event != NULL) {
        delete _tof_event;
    }
    _tof_event = event;
}

SciFiEvent* ReconEvent::GetSciFiEvent() const {
    return _scifi_event;
}

void ReconEvent::SetSciFiEvent(SciFiEvent* event) {
    if (_scifi_event != NULL) {
        delete _scifi_event;
    }
    _scifi_event = event;
}

CkovEvent* ReconEvent::GetCkovEvent() const {
    return _ckov_event;
}

void ReconEvent::SetCkovEvent(CkovEvent* event) {
    if (_ckov_event != NULL) {
        delete _ckov_event;
    }
    _ckov_event = event;
}

KLEvent* ReconEvent::GetKLEvent() const {
    return _kl_event;
}

void ReconEvent::SetKLEvent(KLEvent* event) {
    if (_kl_event != NULL) {
        delete _kl_event;
    }
    _kl_event = event;
}

EMREvent* ReconEvent::GetEMREvent() const {
    return _emr_event;
}

void ReconEvent::SetEMREvent(EMREvent* event) {
    if (_emr_event != NULL) {
        delete _emr_event;
    }
    _emr_event = event;
}

TriggerEvent* ReconEvent::GetTriggerEvent() const {
    return _trigger_event;
}

void ReconEvent::SetTriggerEvent(TriggerEvent* event) {
    if (_trigger_event != NULL) {
        delete _trigger_event;
    }
    _trigger_event = event;
}

GlobalEvent* ReconEvent::GetGlobalEvent() const {
    return _global_event;
}

void ReconEvent::SetGlobalEvent(GlobalEvent* event) {
    if (_global_event != NULL) {
        delete _global_event;
    }
    _global_event = event;
}
}

