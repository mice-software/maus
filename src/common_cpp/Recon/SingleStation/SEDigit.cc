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

#include "src/common_cpp/Recon/SingleStation/SEDigit.hh"

// namespace MAUS {

SEDigit::SEDigit():_used(false) {}

SEDigit::SEDigit(int plane, int channel, double npe, double time, int adc)
                 :_plane(plane), _channel(channel), _npe(npe),
                  _time(time), _adc(adc), _used(false) {
}

SEDigit::~SEDigit() {}
// } // ends namespace MAUS
