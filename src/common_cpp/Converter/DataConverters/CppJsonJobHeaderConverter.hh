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

#ifndef _SRC_COMMON_CPP_CONVERTER_DATACONVERTERS_CPPJSONJOBHEADERCONVERTER_H_
#define _SRC_COMMON_CPP_CONVERTER_DATACONVERTERS_CPPJSONJOBHEADERCONVERTER_H_
#include "json/json.h"

#include "src/common_cpp/Converter/ConverterBase.hh"

namespace MAUS {

class JobHeaderData;

/*!
 * \class CppJsonJobHeaderConverter
 *
 * \brief Converts binary Cpp JobHeader into corresponding json type
 *
 * JsonCppConverter is build on \a ConverterBase as a specialisation to
 * handle the conversion of data from \a Json::Value to binary \a MausData
 * format. Since Json documents often come in the form of a string, these
 * too are accepted and parsed on the fly.
 */
  class CppJsonJobHeaderConverter : public ConverterBase<JobHeaderData, Json::Value> {
  public:
    CppJsonJobHeaderConverter()
      : ConverterBase<JobHeaderData, Json::Value>("CppJsonConverter") {}

  private:
    /*!
     * \brief Convert MausData
     * Overloaded process initiates the conversion process converting the
     * \a MausData given as the argument into the output type \a Json::Value
     *
     * \param MausData& The root \a MausData object from the cpp data structure
     * \return a pointer to the Json::Value object
     */
    Json::Value* _convert(const JobHeaderData*) const;

  private:
};
}

#endif
