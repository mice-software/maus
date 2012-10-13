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

#include "src/legacy/Interface/Interpolation/Interpolator3dGridTo1d.hh"

void Interpolator3dGridTo1d::DeleteFunc(double*** func) {
    if (func == NULL)
        return;
    for (int i = 0; i < NumberOfXCoords(); i++) {
        for (int j = 0; j < NumberOfYCoords(); j++)
            delete [] func[i][j];
        delete [] func[i];
    }
    delete [] func;
    func = NULL;
}

