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

#ifndef JSONCPPCONVERTER_H
#define JSONCPPCONVERTER_H
#include "json/json.h"

#include "src/common_cpp/Converter/ConverterBase.hh"
#include "src/common_cpp/DataStructure/Spill.hh"

namespace MAUS {

/*!
 * \class JsonCppConverter
 *
 * \brief Converts JSON documents into corresponding binary Cpp fromat
 *
 * JsonCppConverter is build on \a ConverterBase as a specialisation to
 * handle the conversion of data from \a Json::Value to binary \a MausData
 * format. Since Json documents often come in the form of a string, these
 * too are accepted and parsed on the fly.
 *
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
  class JsonCppSpillConverter : public ConverterBase<Json::Value, Spill> {
  public:
    JsonCppSpillConverter()
      : ConverterBase<Json::Value, Spill>("JsonCppConverter") {}

  private:
    /*!
     * \brief Convert Json value
     * Overloaded process initiate the conversion process converting the
     * \a Json::Value given as the argument into the output type \a MausData
     *
     * \param Json::Value& The root \a Json::Value object from the Json data
     *        file
     * \return a pointer to the MausData object
     */
    Spill* _convert(const Json::Value*) const;
  private:
};
}

#endif
