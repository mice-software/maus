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

#include <iostream>

#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/DataStructure/ImageData/CanvasWrapper.hh"

namespace MAUS {
CanvasWrapper::CanvasWrapper()
    : _description(""), _file_tag(""), _canvas(NULL) {
}

CanvasWrapper::CanvasWrapper(const CanvasWrapper& data)
    : _description(""), _file_tag(""), _canvas(NULL) {
    *this = data;
}

CanvasWrapper& CanvasWrapper::operator=(const CanvasWrapper& rhs) {
    if (this == &rhs)
        return *this;
    _description = rhs._description;
    _file_tag = rhs._file_tag;
    if (_canvas != NULL)
        delete _canvas;
    if (rhs._canvas != NULL) {
        _canvas = static_cast<TCanvas*>(rhs._canvas->Clone());
    } else {
        _canvas = NULL;
    }
    return *this;
}

CanvasWrapper::~CanvasWrapper() {}

void CanvasWrapper::SetCanvas(TCanvas* canvas) {
    if (_canvas != NULL)
        delete _canvas;
    _canvas = canvas;
}

void CanvasWrapper::EmbedCanvas(TRootEmbeddedCanvas* embed) {
    if (embed == NULL) {
        throw Exception(Exception::recoverable, "EmbeddedCanvas was NULL",
                        "CanvasWrapper::EmbedCanvas");
    }
    if (_canvas == NULL) {
        throw Exception(Exception::recoverable, "Trying to adopt NULL canvas",
                        "CanvasWrapper::EmbedCanvas");
    }
    embed->AdoptCanvas(_canvas);
    _canvas = NULL; // embed now owns the memory
}

} // namespace MAUS

