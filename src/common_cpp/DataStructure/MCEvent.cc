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

#include "src/common_cpp/DataStructure/Track.hh"
#include "src/common_cpp/DataStructure/VirtualHit.hh"
#include "src/common_cpp/DataStructure/Primary.hh"

#include "src/common_cpp/DataStructure/MCEvent.hh"

namespace MAUS {

MCEvent::MCEvent()
       : _primary(NULL), _virtuals(NULL), _sci_fi_hits(NULL), _sci_fi_noise_hits(NULL),
         _tof_hits(NULL), _kl_hits(NULL), _emr_hits(NULL), _special_virtual_hits(NULL),
         _tracks(NULL), _ckov_hits(NULL) {
}

MCEvent::MCEvent(const MCEvent& md)
       : _primary(NULL), _virtuals(NULL), _sci_fi_hits(NULL), _sci_fi_noise_hits(NULL),
         _tof_hits(NULL), _kl_hits(NULL), _emr_hits(NULL), _special_virtual_hits(NULL),
         _tracks(NULL), _ckov_hits(NULL) {
  *this = md;
}

MCEvent& MCEvent::operator=(const MCEvent& md) {
    if (this == &md) {
        return *this;
    }
    if (_tracks != NULL) {
        delete _tracks;
    }
    if (md._tracks == NULL) {
        _tracks = NULL;
    } else {
        _tracks = new TrackArray(*md._tracks);
    }

    if (_virtuals != NULL) {
        delete _virtuals;
    }
    if (md._virtuals == NULL) {
        _virtuals = NULL;
    } else {
        _virtuals = new VirtualHitArray(*md._virtuals);
    }

    if (_sci_fi_hits != NULL) {
        delete _sci_fi_hits;
    }
    if (md._sci_fi_hits == NULL) {
        _sci_fi_hits = NULL;
    } else {
        _sci_fi_hits = new SciFiHitArray(*md._sci_fi_hits);
    }

    if (_sci_fi_noise_hits != NULL) {
        delete _sci_fi_noise_hits;
    }
    if (md._sci_fi_noise_hits == NULL) {
        _sci_fi_noise_hits = NULL;
    } else {
        _sci_fi_noise_hits = new SciFiNoiseHitArray(*md._sci_fi_noise_hits);
    }

    if (_tof_hits != NULL) {
        delete _tof_hits;
    }
    if (md._tof_hits == NULL) {
        _tof_hits = NULL;
    } else {
        _tof_hits = new TOFHitArray(*md._tof_hits);
    }

    if (_ckov_hits != NULL) {
      delete _ckov_hits;
    }
    if (md._ckov_hits == NULL) {
      _ckov_hits = NULL;
    } else {
      _ckov_hits = new CkovHitArray(*md._ckov_hits);
    }

    if (_kl_hits != NULL) {
        delete _kl_hits;
    }
    if (md._kl_hits == NULL) {
        _kl_hits = NULL;
    } else {
        _kl_hits = new KLHitArray(*md._kl_hits);
    }

    if (_emr_hits != NULL) {
        delete _emr_hits;
    }
    if (md._emr_hits == NULL) {
        _emr_hits = NULL;
    } else {
        _emr_hits = new EMRHitArray(*md._emr_hits);
    }

    if (_special_virtual_hits != NULL) {
        delete _special_virtual_hits;
    }
    if (md._special_virtual_hits == NULL) {
        _special_virtual_hits = NULL;
    } else {
        _special_virtual_hits =
         new SpecialVirtualHitArray(*md._special_virtual_hits);
    }

    if (_primary != NULL) {
        delete _primary;
    }
    if (md._primary == NULL) {
        _primary = NULL;
    } else {
        _primary = new Primary(*md._primary);
    }

    return *this;
}

MCEvent::~MCEvent() {
    if (_tracks != NULL) {
        delete _tracks;
        _tracks = NULL;
    }
    if (_virtuals != NULL) {
        delete _virtuals;
        _virtuals = NULL;
    }
    if (_sci_fi_hits != NULL) {
        delete _sci_fi_hits;
        _sci_fi_hits = NULL;
    }
    if (_sci_fi_noise_hits != NULL) {
        delete _sci_fi_noise_hits;
        _sci_fi_noise_hits = NULL;
    }
    if (_tof_hits != NULL) {
        delete _tof_hits;
        _tof_hits = NULL;
    }
    if (_ckov_hits != NULL) {
        delete _ckov_hits;
	_ckov_hits = NULL;
    }
    if (_kl_hits != NULL) {
        delete _kl_hits;
        _kl_hits = NULL;
    }
    if (_emr_hits != NULL) {
        delete _emr_hits;
        _emr_hits = NULL;
    }
    if (_special_virtual_hits != NULL) {
        delete _special_virtual_hits;
        _special_virtual_hits = NULL;
    }
    if (_primary != NULL) {
        delete _primary;
        _primary = NULL;
    }
}

TrackArray* MCEvent::GetTracks() const {
    return _tracks;
}

void MCEvent::SetTracks(TrackArray* tracks) {
    if (_tracks != NULL) {
        delete _tracks;
    }
    _tracks = tracks;
}

VirtualHitArray* MCEvent::GetVirtualHits() const {
    return _virtuals;
}

void MCEvent::SetVirtualHits(VirtualHitArray* hits) {
    if (_virtuals != NULL) {
        delete _virtuals;
    }
    _virtuals = hits;
}

VirtualHit& MCEvent::GetAVirtualHit(size_t i) const {
    return (*_virtuals)[i];
}

size_t MCEvent::GetVirtualHitsSize() const {
    return _virtuals->size();
}

SciFiHitArray* MCEvent::GetSciFiHits() const {
    return _sci_fi_hits;
}

void MCEvent::SetSciFiHits(SciFiHitArray* hits) {
    if (_sci_fi_hits != NULL) {
        delete _sci_fi_hits;
    }
    _sci_fi_hits = hits;
}

SciFiNoiseHitArray* MCEvent::GetSciFiNoiseHits() const {
    return _sci_fi_noise_hits;
}

void MCEvent::SetSciFiNoiseHits(SciFiNoiseHitArray* noise_hits) {
    if (_sci_fi_noise_hits != NULL) {
        delete _sci_fi_noise_hits;
    }
    _sci_fi_noise_hits = noise_hits;
}

TOFHitArray* MCEvent::GetTOFHits() const {
    return _tof_hits;
}

void MCEvent::SetTOFHits(TOFHitArray* hits) {
    if (_tof_hits != NULL) {
        delete _tof_hits;
    }
    _tof_hits = hits;
}

CkovHitArray* MCEvent::GetCkovHits() const {
    return _ckov_hits;
}

void MCEvent::SetCkovHits(CkovHitArray* hits) {
    if (_ckov_hits != NULL) {
        delete _ckov_hits;
    }
    _ckov_hits = hits;
}

KLHitArray* MCEvent::GetKLHits() const {
    return _kl_hits;
}

void MCEvent::SetKLHits(KLHitArray* hits) {
    if (_kl_hits != NULL) {
        delete _kl_hits;
    }
    _kl_hits = hits;
}

EMRHitArray* MCEvent::GetEMRHits() const {
    return _emr_hits;
}

void MCEvent::SetEMRHits(EMRHitArray* hits) {
    if (_emr_hits != NULL) {
        delete _emr_hits;
    }
    _emr_hits = hits;
}

SpecialVirtualHitArray* MCEvent::GetSpecialVirtualHits() const {
    return _special_virtual_hits;
}

void MCEvent::SetSpecialVirtualHits(SpecialVirtualHitArray* hits) {
    if (_special_virtual_hits != NULL) {
        delete _special_virtual_hits;
    }
    _special_virtual_hits = hits;
}

Primary* MCEvent::GetPrimary() const {
    return _primary;
}

void MCEvent::SetPrimary(Primary* primary) {
    if (_primary != NULL) {
        delete _primary;
    }
    _primary = primary;
}
}

