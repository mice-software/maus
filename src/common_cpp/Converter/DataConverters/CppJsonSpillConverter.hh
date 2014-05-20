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

#ifndef CPPJSONSPILLCONVERTER_H
#define CPPJSONSPILLCONVERTER_H
#include "json/json.h"

#include "src/common_cpp/Converter/ConverterBase.hh"

namespace MAUS {

class Data;

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
  class CppJsonSpillConverter : public ConverterBase<Data, Json::Value> {
  public:
    CppJsonSpillConverter()
      : ConverterBase<Data, Json::Value>("CppJsonSpillConverter") {}

  private:
    /*!
     * \brief Convert MausData
     * Overloaded process initiates the conversion process converting the
     * \a MausData given as the argument into the output type \a Json::Value
     *
     * \param MausData& The root \a MausData object from the cpp data structure
     * \return a pointer to the Json::Value object
     *
     * Some ugliness results from the fact that the Spill is written as the base
     * top level in Json, but for reasons of stupidity (Rogers) the Data is
     * written as the top level in ROOT. To make it worse, Spill doesn't always
     * hold spill data - as DAQ sometimes spits out start_of_burst or
     * calibration data.
     *
     * Caller owns memory allocated to Json::Value* in return.
     */
    Json::Value* _convert(const Data*) const;

  private:
};
}

#endif
