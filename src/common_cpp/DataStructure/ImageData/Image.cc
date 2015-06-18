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
 *
 */

#include "src/common_cpp/DataStructure/ImageData/Image.hh"

namespace MAUS {
Image::Image() : _run_number(0), _spill_number(0),
    _input_time(), _output_time(), _canvas_wrappers() {
}

Image::Image(const Image& data) {
    *this = data;
}

Image& Image::operator=(const Image& rhs) {
    if (this == &rhs)
        return *this;
    _run_number = rhs._run_number;
    _spill_number = rhs._spill_number;
    _input_time = rhs._input_time;
    _output_time = rhs._output_time;
    for (size_t i = 0; i < _canvas_wrappers.size(); ++i)
        delete _canvas_wrappers[i];
    _canvas_wrappers = std::vector<MAUS::CanvasWrapper*>(rhs._canvas_wrappers.size(), NULL);
    for (size_t i = 0; i < rhs._canvas_wrappers.size(); ++i)
        _canvas_wrappers[i] = new CanvasWrapper(*rhs._canvas_wrappers[i]);
    return *this;
}

Image::~Image() {
}

void Image::SetCanvasWrappers(std::vector<CanvasWrapper*> wrappers) {
    for (size_t i = 0; i < _canvas_wrappers.size(); ++i)
        delete _canvas_wrappers[i];
    _canvas_wrappers = wrappers;
}

std::vector<CanvasWrapper*> Image::GetCanvasWrappers() const {
    return _canvas_wrappers;
}
} // namespace MAUS

