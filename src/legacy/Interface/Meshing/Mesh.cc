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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

#include <iomanip>

#include "src/legacy/Interface/Meshing/Mesh.hh"

std::ostream& operator<<(std::ostream& out, const Mesh::Iterator& it) {
    out << std::setw(5) << it.ToInteger() << " ** ";
    for (unsigned int i = 0; i < it.State().size(); i++)
        out << std::setw(5) << it[i] << " ";
    out << "** ";
    for (unsigned int i = 0; i < it.Position().size(); i++)
        out << std::scientific << std::setprecision(3) << std::setw(12)
            << it.Position()[i] << " ";
    return out;
}

