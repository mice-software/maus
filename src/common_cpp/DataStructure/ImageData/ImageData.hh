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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_IMAGEDATA_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_IMAGEDATA_HH_

#include <string>
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/MAUSEvent.hh"

namespace MAUS {

class Image;

/** ImageData is the root of the MAUS image structure
 *
 *  ImageData class is the root class for the MAUS image structure. It
 *  just holds a pointer to the image. This is to handle stupid way in which
 *  ROOT does data structure stuff.
 */
class ImageData : public MAUSEvent<Image> {
  public:
    /** Default constructor initialises everything to NULL */
    ImageData();

    /** Copy constructor (deepcopy) */
    ImageData(const ImageData& data);

    /** Deepcopy from data to *this */
    ImageData& operator=(const ImageData& data);

    /** Deletes the image data */
    virtual ~ImageData();

    /** Set the image data
     *
     *  ImageData now takes ownership of memory allocated to job header
     */
    void SetImage(Image* image);

    /** Get the job header data
     *
     *  ImageData still owns this memory
     */
    Image* GetImage() const {return _image;}

    /** Returns sizeof the job header object
     *
     *  When setting up TTrees directly in ROOT, it is necessary to hand ROOT
     *  the size of the class member. This is difficult (impossible?) to access
     *  from python, so we provide a convenience function here.
     */
    int GetSizeOf() const;

    /** Get Event Type information 
     */
    std::string GetEventType() const {
        return MAUSEvent<Image>::GetEventType();
    }

    /** Set Event Type information 
     */
    void SetEventType(std::string type) {
        MAUSEvent<Image>::SetEventType(type);
    }

    /** Overload MAUSEvent function (wraps SetImage)*/
    void SetEvent(Image* header) {SetImage(header);}

    /** Overload MAUSEvent function (wraps GetImage)*/
    Image* GetEvent() const {return GetImage();}

  private:
    Image* _image;
    MAUS_VERSIONED_CLASS_DEF(ImageData);
};
}

#endif

