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

#include "src/common_cpp/DataStructure/Image.hh"
#include "src/common_cpp/DataStructure/ImageData.hh"

namespace MAUS {
ImageData::ImageData() : MAUSEvent<Image>("ImageData"), _image(NULL) {
}

ImageData::ImageData(const ImageData& data)
      : MAUSEvent<Image>("ImageData"), _image(NULL) {
    *this = data;
}

ImageData::~ImageData() {
    if (_image != NULL) {
        delete _image;
    }
}

ImageData& ImageData::operator=(const ImageData& data) {
    if (&data == this) {
        return *this;
    }
    if (_image != NULL)
        delete _image;
    _image = new Image(*data._image);
    return *this;
}

void ImageData::SetImage(Image* image) {
    if (_image != NULL) {
        delete _image;
    }
    _image = image;
}

int ImageData::GetSizeOf() const {
  ImageData data;
  return sizeof(data);
}

}

