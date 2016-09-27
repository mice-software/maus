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

#include <algorithm>
#include <vector>
#include <iostream>

#include "Utils/Squeak.hh"
#include "Utils/Exception.hh"

#include "src/legacy/Interface/Mesh.hh"
#include "src/legacy/Interface/STLUtils.hh"

#include "src/common_cpp/FieldTools/SectorMagneticFieldMap.hh"

namespace MAUS {

std::map<std::string, SectorMagneticFieldMap*> SectorMagneticFieldMap::_fields;

SectorMagneticFieldMap::SectorMagneticFieldMap()
                       : SectorField(), _interpolator(NULL), _symmetry(none) {
}

SectorMagneticFieldMap::SectorMagneticFieldMap(std::string file_name,
       std::string file_format, std::vector<double> units, std::string symmetry)
       : SectorField(), _interpolator(NULL), _symmetry(none), _units(units), _format(file_format),
                        _filename(file_name) {
    SetSymmetry(symmetry);
    if (_fields.find(file_name) == _fields.end()) {
        Interpolator3dGridTo3d* interpolator = SectorMagneticFieldMapIO::ReadMap
                                      (file_name, file_format, units, symmetry);
        SetInterpolator(interpolator);
        delete _interpolator->Clone();
        _fields[file_name] = new SectorMagneticFieldMap(*this);
    } else {
        SectorMagneticFieldMap* tgt = _fields[file_name];
        if (_symmetry != tgt-> _symmetry || _units != tgt->_units ||
            _format != tgt->_format || _filename != tgt->_filename) {
            throw(Exceptions::Exception(Exceptions::recoverable,
               "Attempt to construct different SectorFieldMaps with same file "+
               std::string("but different settings"),
               "SectorMagneticFieldMap::SectorMagneticFieldMap(...)"));
        }
        SetInterpolator(tgt->_interpolator->Clone());
    }
}

SectorMagneticFieldMap::SectorMagneticFieldMap
                                        (const SectorMagneticFieldMap& field)
    : SectorField(field), _interpolator(NULL), _symmetry(field._symmetry),
      _units(field._units), _format(field._format),
      _filename(field._filename) {
    Interpolator3dGridTo3d* interpolator = NULL;
    if (field._interpolator != NULL) {
        interpolator = field._interpolator->Clone();
    }
    SetInterpolator(interpolator);
}

SectorMagneticFieldMap::~SectorMagneticFieldMap() {
    delete _interpolator;
}

Interpolator3dGridTo3d* SectorMagneticFieldMap::GetInterpolator() {
    return _interpolator;
}

void SectorMagneticFieldMap::SetInterpolator
                                        (Interpolator3dGridTo3d* interpolator) {
    if (_interpolator != NULL) {
        delete _interpolator;
    }
    _interpolator = interpolator;
    if (_interpolator != NULL) {
        ThreeDGrid* grid = _interpolator->GetMesh();
        SectorField::SetPolarBoundingBox
                                    (grid->MinX(), grid->MinY(), grid->MinZ(),
                                     grid->MaxX(), grid->MaxY(), grid->MaxZ());
    }
    delete _interpolator->Clone();
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
    throw(Exceptions::Exception(Exceptions::recoverable,
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
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Didn't recognise symmetry type",
                 "SectorMagneticFieldMap::SymmetryToString"));
}

void SectorMagneticFieldMap::GetFieldValuePolar
                                    (const double* point, double* field) const {
    _interpolator->F(&point[0], field);
    SectorField::ConvertToPolar(point, field);
}

void SectorMagneticFieldMap::GetFieldValue
                                    (const double* point, double* field) const {
    std::vector<double> _point(&point[0], &point[4]);
    SectorField::ConvertToPolar(&_point[0]);
    _interpolator->F(&_point[0], field);
}


void SectorMagneticFieldMap::ClearFieldCache() {
    for (std::map<std::string, SectorMagneticFieldMap*>::iterator it =
                                   _fields.begin(); it != _fields.end(); ++it) {
        delete (*it).second;
    }
    _fields = std::map<std::string, SectorMagneticFieldMap*>();
}

const double SectorMagneticFieldMapIO::float_tolerance = 1e-3;
const int SectorMagneticFieldMapIO::sort_order[3] = {0, 1, 2};

Interpolator3dGridTo3d* SectorMagneticFieldMapIO::ReadMap
     (std::string file_name, std::string file_type, std::vector<double> units,
                                         std::string symmetry) {
    try {
        file_name = STLUtils::ReplaceVariables(file_name);
        Squeak::mout(Squeak::debug) << "Opening sector field map " << file_name
                                    << " type " << file_type << std::endl;
        if (file_type == "tosca_sector_1") {
            return ReadToscaMap(file_name, units, symmetry);
        }
    } catch (std::exception& exc) {
        throw(Exceptions::Exception(Exceptions::recoverable,
                     "Failed to read file "+file_name+" with "+(&exc)->what(),
                     "SectorMagneticFieldMapIO::ReadMap"));
    }
    throw(Exceptions::Exception(Exceptions::recoverable,
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
    std::vector< std::vector<double> > field_points = ReadLines
                                                             (file_name, units);
    // build grid
    ThreeDGrid* grid = GenerateGrid(field_points, sym);

    // build interpolator
    return GetInterpolator(field_points, grid, sym);
}

Interpolator3dGridTo3d* SectorMagneticFieldMapIO::GetInterpolator
                        (const std::vector< std::vector<double> > field_points,
                         ThreeDGrid* grid,
                         SectorMagneticFieldMap::symmetry sym) {
    int y_start = 0;
    if (sym == SectorMagneticFieldMap::dipole) {
        y_start = (grid->ySize()+1)/2-1;
    }
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
                if (index >= static_cast<int>(field_points.size())) {
                   throw Exceptions::Exception(Exceptions::recoverable,
                                   "Field grid is not rectangular",
                                   "SectorMagneticFieldMapIO::GetInterpolator");
                }
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

std::vector< std::vector<double> > SectorMagneticFieldMapIO::ReadLines
                             (std::string file_name, std::vector<double> units) {
    std::vector< std::vector<double> > field_points;
    std::string line;
    if (units.size() != 6) {
        throw(Exceptions::Exception(Exceptions::recoverable,
                    "Units should be of length 6",
                    "SectorMagneticFieldMapIO::ReadMap"));
    }

    std::ifstream fin(file_name.c_str());
    if (!fin || !fin.is_open()) {
        throw(Exceptions::Exception(Exceptions::recoverable,
                     "Failed to open file "+file_name,
                     "SectorMagneticFieldMapIO::ReadMap"));
    }
    // skip header lines
    Squeak::mout(Squeak::debug) << "Opened "+file_name << std::endl;
    for (size_t i = 0; i < 8; ++i) {
        std::getline(fin, line);
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
        SectorField::ConvertToPolar(&field_points[i][0]);
    }

    // force check sort order
    std::sort(field_points.begin(), field_points.end(),
                                          SectorMagneticFieldMapIO::Comparator);
    return field_points;
}

ThreeDGrid* SectorMagneticFieldMapIO::GenerateGrid
                       (const std::vector< std::vector<double> > field_points,
                        SectorMagneticFieldMap::symmetry sym) {
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
    int y_end = static_cast<int>(y_grid.size());
    if (sym == SectorMagneticFieldMap::dipole) {
        for (int i = 0; i < y_end-1; ++i) {
            y_grid.insert(y_grid.begin(), 2.*y_grid[i]-y_grid[2*i+1]);
        }
        y_end = y_grid.size();
    }
    Squeak::mout(Squeak::debug) << "Grid size (r, y, phi) = ("
                                << r_grid.size() << ", " << y_grid.size()
                                << ", " << phi_grid.size() << ")" << std::endl;

    ThreeDGrid* grid = new ThreeDGrid(r_grid, y_grid, phi_grid);
    return grid;
}
}

