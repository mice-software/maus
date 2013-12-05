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

#include "TCanvas.h"

#include "src/common_cpp/DataStructure/Image.hh"

namespace MAUS {
Image::Image() : _description(""), _text(""), _run_number(0), _spill_number(0),
    _input_time(), _output_time(), _canvas(NULL) {
}

Image::Image(const Image& data) : _canvas(NULL) {
    *this = data;
}

Image& Image::operator=(const Image& rhs) {
    if (this == &rhs)
        return *this;
    _description = rhs._description;
    _text = rhs._text;
    _run_number = rhs._run_number;
    _spill_number = rhs._spill_number;
    _input_time = rhs._input_time;
    _output_time = rhs._output_time;
    if (_canvas == NULL)
        delete _canvas;
    _canvas = dynamic_cast<TCanvas*>(rhs._canvas->DrawClone());
    return *this;
}

Image::~Image() {
    if (_canvas != NULL)
        delete _canvas;
}

void Image::SetCanvas(TCanvas* canvas) {
    if (_canvas != NULL)
        delete _canvas;
    _canvas = canvas;
}
} // namespace MAUS

