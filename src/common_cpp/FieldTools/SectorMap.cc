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

#include <math.h>

#include <vector>
#include <iostream>

#include "src/legacy/Interface/Squeak.hh"
#include "src/legacy/Interface/Squeal.hh"

#include "src/legacy/Interface/Mesh.hh"

#include "src/common_cpp/FieldTools/SectorMap.hh"

namespace MAUS {

SectorMagneticFieldMap::SectorMagneticFieldMap()
                       : _interpolator(NULL), _symmetry(none) {
}

SectorMagneticFieldMap::SectorMagneticFieldMap(std::string file_name,
       std::string file_format, std::vector<double> units, std::string symmetry)
                        : _interpolator(NULL), _symmetry(none) {
    SetSymmetry(symmetry);
    _interpolator = SectorMagneticFieldMapIO::ReadMap
                                    (file_name, file_format, units, symmetry);

}

SectorMagneticFieldMap::~SectorMagneticFieldMap() {
    delete _interpolator;
}

Interpolator3dGridTo3d* SectorMagneticFieldMap::GetInterpolator() {
    return _interpolator;
}

void SectorMagneticFieldMap::SetInterpolator
                                        (Interpolator3dGridTo3d* interpolator) {
    _interpolator = interpolator;
}

std::string SectorMagneticFieldMap::GetSymmetry() {
    return SymmetryToString(_symmetry);
}

void SectorMagneticFieldMap::SetSymmetry(std::string name) {
    _symmetry = StringToSymmetry(name);
}

SectorMagneticFieldMap::symmetry SectorMagneticFieldMap::StringToSymmetry
                                                             (std::string sym) {
    if (sym == "None") {
        return none;
    }
    if (sym == "Dipole") {
        return dipole;
    }
    throw(Squeal(Squeal::recoverable,
          "Didn't recognise symmetry type "+sym,
          "SectorMagneticFieldMap::StringToSymmetry"));
}

std::string SectorMagneticFieldMap::SymmetryToString
                                        (SectorMagneticFieldMap::symmetry sym) {
    if (sym == none) {
        return "None";
    }
    if (sym == dipole) {
        return "Dipole";
    }
    throw(Squeal(Squeal::recoverable,
                 "Didn't recognise symmetry type",
                 "SectorMagneticFieldMap::SymmetryToString"));
   
}

void SectorMagneticFieldMap::GetFieldValuePolar
                                          (const double* point, double* field) {
    double int_b[3];  // bx, by, bz
    _interpolator->F(&point[0], int_b);
    field[0] = int_b[0]*::cos(point[2])+int_b[2]*::sin(point[2]);
    field[1] = int_b[1];
    field[2] = int_b[0]*::sin(point[2])+int_b[2]*::cos(point[2]);
}

void SectorMagneticFieldMap::GetFieldValue(const double* point, double* field) {
    double mirror = 1.;
    std::vector<double> _point(&point[0], &point[3]);
    _point[0] = sqrt(point[0]*point[0]+point[2]*point[2]);
    _point[2] = atan2(point[2], point[0]);
    if (_symmetry == dipole) {
        if (_point[1] < 0.) {
            mirror = -1;
            _point[1] *= -1.;
        }
    }
    _interpolator->F(&_point[0], field);
}

const double SectorMagneticFieldMapIO::float_tolerance = 1e-3;
const int SectorMagneticFieldMapIO::sort_order[3] = {0, 1, 2};

Interpolator3dGridTo3d* SectorMagneticFieldMapIO::ReadMap
     (std::string file_name, std::string file_type, std::vector<double> units,
                                         std::string symmetry) {
    try {
        Squeak::mout(Squeak::debug) << "Opening sector field map " << file_name
                                    << " type " << file_type << std::endl;
        if (file_type == "tosca_sector") {
            return ReadToscaMap(file_name, units, symmetry);
        } 
    } catch(std::exception& exc) {
        throw(Squeal(Squeal::recoverable,
                     "Failed to read file "+file_name+" with "+(&exc)->what(),
                     "SectorMagneticFieldMapIO::ReadMap"));
    }
    throw(Squeal(Squeal::recoverable,
                 "Didn't recognise map type "+file_type,
                 "SectorMagneticFieldMapIO::ReadMap"));
    return NULL;
}

Interpolator3dGridTo3d* SectorMagneticFieldMapIO::ReadToscaMap
                            (std::string file_name, std::vector<double> units,
                                         std::string symmetry) {
    SectorMagneticFieldMap::symmetry sym = 
                             SectorMagneticFieldMap::StringToSymmetry(symmetry);
    // set up
    std::vector< std::vector<double> > field_points;
    std::string line;
    if (units.size() != 6) {
        throw(Squeal(Squeal::recoverable,
                    "Units should be of length 6",
                    "SectorMagneticFieldMapIO::ReadMap"));
    }

    std::ifstream fin(file_name.c_str());
    if (!fin || !fin.is_open()) {
        throw(Squeal(Squeal::recoverable,
                     "Failed to open file "+file_name,
                     "SectorMagneticFieldMapIO::ReadMap"));        
    }
    // skip header lines
    Squeak::mout(Squeak::debug) << "Opened "+file_name << std::endl;
    for (size_t i = 0; i < 8; ++i) {
        std::getline(fin, line);
        Squeak::mout(Squeak::debug) << line << std::endl;
    }
    // read in field map
    int line_number = 0;
    while (fin) {
        std::vector<double> field(6);
        fin >> field[0] >> field[1] >> field[2] >> field[3] >> field[4] 
            >> field[5];
        
        if (fin) {
            for (size_t i = 0; i < 6; ++i) {
                field[i] *= units[i];
            }
            field_points.push_back(field);
            line_number++;
        }
    }
    Squeak::mout(Squeak::debug) << "Read " << line_number << " lines"
                                << std::endl;

    // convert coordinates to polar; nb we leave field as cartesian
    for (size_t i = 0; i < field_points.size(); ++i) {
        double x = field_points[i][0];
        double z = field_points[i][2];
        field_points[i][0] = sqrt(x*x+z*z);
        field_points[i][2] = atan2(z, x);
    }

    //force check sort order
    std::sort(field_points.begin(), field_points.end(),
                                          SectorMagneticFieldMapIO::Comparator);   
    // build grid
    std::vector<double> r_grid(1, field_points[0][0]);
    std::vector<double> y_grid(1, field_points[0][1]);
    std::vector<double> phi_grid(1, field_points[0][2]);
    for (size_t i = 0; i < field_points.size(); ++i) {
        if (field_points[i][0] > r_grid.back()*(1+float_tolerance)+float_tolerance) {
            r_grid.push_back(field_points[i][0]);
        }
        if (field_points[i][1] > y_grid.back()*(1+float_tolerance)+float_tolerance) {
            y_grid.push_back(field_points[i][1]);
        }
        if (field_points[i][2] > phi_grid.back()*(1+float_tolerance)+float_tolerance) {
            phi_grid.push_back(field_points[i][2]);
        }
    }

    // reflect about y if symmetry is dipole
    int y_start = 0;
    int y_end = static_cast<int>(y_grid.size());
    if (sym == SectorMagneticFieldMap::dipole) {
        for (int i = 0; i < y_end-1; ++i) {
            y_grid.insert(y_grid.begin(), 2.*y_grid[i]-y_grid[2*i+1]);
        }               
        y_start = y_end-1;
        y_end = y_grid.size();
    }
    Squeak::mout(Squeak::debug) << "Grid size (r, y, phi) = ("
                                << r_grid.size() << ", " << y_grid.size()
                                << ", " << phi_grid.size() << ")" << std::endl;

    ThreeDGrid* grid = new ThreeDGrid(r_grid, y_grid, phi_grid);

    // build field arrays
    double *** Bx, *** By, *** Bz;
    int index = 0;
    Bx = new double**[grid->xSize()];
    By = new double**[grid->xSize()];
    Bz = new double**[grid->xSize()];
    for (int i = 0; i < grid->xSize(); ++i) {
        Bx[i] = new double*[grid->ySize()];
        By[i] = new double*[grid->ySize()];
        Bz[i] = new double*[grid->ySize()];
        for (int j = 0; j < grid->ySize(); ++j) {
            Bx[i][j] = new double[grid->zSize()];
            By[i][j] = new double[grid->zSize()];
            Bz[i][j] = new double[grid->zSize()];
            for (int k = 0; k < grid->zSize() && j >= y_start; ++k) {
                Bx[i][j][k] = field_points[index][3];
                By[i][j][k] = field_points[index][4];
                Bz[i][j][k] = field_points[index][5];
                ++index;
            }
        }
    }

    // extend field array downwards if field is dipole
    if (sym == SectorMagneticFieldMap::dipole) {
        for (int i = 0; i < grid->xSize(); ++i) {
            for (int j = 0; j < y_start; ++j) {
                for (int k = 0; k < grid->zSize(); ++k) {
                    Bx[i][j][k] = -Bx[i][grid->ySize()-j-1][k];
                    By[i][j][k] = By[i][grid->ySize()-j-1][k];
                    Bz[i][j][k] = -Bz[i][grid->ySize()-j-1][k];
                    ++index;
                }
            }
        }
    }

    return new Interpolator3dGridTo3d(grid, Bx, By, Bz);
}

bool SectorMagneticFieldMapIO::Comparator(std::vector<double> field_item1,
                     std::vector<double> field_item2) {
    const int* order = sort_order;
    if (fabs(field_item1[order[0]] - field_item2[order[0]]) > float_tolerance) {
        return field_item1[order[0]] < field_item2[order[0]];
    }
    if (fabs(field_item1[order[1]] - field_item2[order[1]]) > float_tolerance) {
        return field_item1[order[1]] < field_item2[order[1]];
    }
    return field_item1[order[2]] < field_item2[order[2]];
}

}

