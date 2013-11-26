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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIMCLOOKUP_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIMCLOOKUP_HH_

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"

namespace MAUS {

/** Identification number derived from SciFi digits */
class SciFiMCLookup {
  public:
    /** Constructor - allocate to 0 */
    SciFiMCLookup();

    /** Copy Constructor - copies data from lookup*/
    SciFiMCLookup(const SciFiMCLookup& lookup);

	/** Constructor - initialises from SciFiDigit */
	explicit SciFiMCLookup(const SciFiDigit* digit);
	/** Constructor - initialises from SciFiDigit */
	explicit SciFiMCLookup(const SciFiDigit& digit);

    /** Equality operator - copies data from lookup */
    SciFiMCLookup& operator=(const SciFiMCLookup& lookup);

    /** Destructor (does nothing)*/
    virtual ~SciFiMCLookup();

    /** Get the ID number */
    double GetID() const { return _digit_Id; }
	/** Set ID number */
	void SetID(double digit_Id) { _digit_Id = digit_Id; }

	/** Return true or false, is noise part of this digit? */
	bool GetNoise() const {return _noise;}
	/** Set noise truth */
	void SetNoise(bool noise) {_noise = noise;}

	/** Get noise size in NPE */
	double GetNoiseNPE() const {return _noise_size;}
	/** Set noise size in NPE */
	void SetNoiseNPE(double noise_size) {_noise_size = noise_size;}

  private:
    double _noise_size;
	double _digit_Id;
	bool _noise;

    MAUS_VERSIONED_CLASS_DEF(SciFiMCLookup)
};

// typedef std::vector<SciFiMCLookup> SciFiMCLookupArray;
}

#endif
