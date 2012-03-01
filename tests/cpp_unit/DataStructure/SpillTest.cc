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

#include "gtest/gtest.h"

#include "src/common_cpp/DataStructure/Spill.hh"

namespace MAUS {

/** Tests the Spill - all one big function urk
 */
TEST(SpillTest, SpillTest) {
    DAQData* daq_1 = new DAQData();
    DAQData* daq_2 = new DAQData();
    DAQData* daq_null = NULL;

    Scalars* scalars_1 = new Scalars();
    Scalars* scalars_2 = new Scalars();
    Scalars* scalars_null = NULL;

    EMRSpillData* emr_1 = new EMRSpillData();
    EMRSpillData* emr_2 = new EMRSpillData();
    EMRSpillData* emr_null = NULL;

    MCEventArray* mc_1 = new MCEventArray();
    MCEventArray* mc_2 = new MCEventArray();
    MCEventArray* mc_null = NULL;

    ReconEventArray* rec_1 = new ReconEventArray();
    ReconEventArray* rec_2 = new ReconEventArray();
    ReconEventArray* rec_null = NULL;

    Spill my_spill;
    // check allocation from NULL okay
    my_spill.SetDAQData(daq_1);
    my_spill.SetScalars(scalars_1);
    my_spill.SetEMRSpillData(emr_1);
    my_spill.SetMCEvents(mc_1);
    my_spill.SetReconEvents(rec_1);
    my_spill.SetSpillNumber(1);
    EXPECT_EQ(my_spill.GetDAQData(), daq_1);    
    EXPECT_EQ(my_spill.GetScalars(), scalars_1);    
    EXPECT_EQ(my_spill.GetEMRSpillData(), emr_1);    
    EXPECT_EQ(my_spill.GetMCEvents(), mc_1);    
    EXPECT_EQ(my_spill.GetReconEvents(), rec_1);    
    EXPECT_EQ(my_spill.GetSpillNumber(), 1);

    // check reallocation okay
    my_spill.SetDAQData(daq_2);
    my_spill.SetScalars(scalars_2);
    my_spill.SetEMRSpillData(emr_2);
    my_spill.SetMCEvents(mc_2);
    my_spill.SetReconEvents(rec_2);
    EXPECT_EQ(my_spill.GetDAQData(), daq_2);    
    EXPECT_EQ(my_spill.GetScalars(), scalars_2);    
    EXPECT_EQ(my_spill.GetEMRSpillData(), emr_2);    
    EXPECT_EQ(my_spill.GetMCEvents(), mc_2);    
    EXPECT_EQ(my_spill.GetReconEvents(), rec_2);    

    // check copy constructor (deep copy)
    Spill my_spill_copy(my_spill);
    EXPECT_NE(my_spill.GetDAQData(), my_spill_copy.GetDAQData());
    EXPECT_NE(my_spill.GetScalars(), my_spill_copy.GetScalars());
    EXPECT_NE(my_spill.GetEMRSpillData(), my_spill_copy.GetEMRSpillData());
    EXPECT_NE(my_spill.GetMCEvents(), my_spill_copy.GetMCEvents());
    EXPECT_NE
            (my_spill.GetReconEvents(), my_spill_copy.GetReconEvents());
    EXPECT_NE(my_spill.GetSpillNumber(), my_spill_copy.GetSpillNumber());

    // check equality operator (deep copy)
    Spill my_spill_equal;
    my_spill_equal = my_spill;
    EXPECT_NE(my_spill.GetDAQData(), my_spill_equal.GetDAQData());
    EXPECT_NE(my_spill.GetScalars(), my_spill_equal.GetScalars());
    EXPECT_NE(my_spill.GetEMRSpillData(), my_spill_equal.GetEMRSpillData());
    EXPECT_NE(my_spill.GetMCEvents(), my_spill_equal.GetMCEvents());
    EXPECT_NE
           (my_spill.GetReconEvents(), my_spill_equal.GetReconEvents());
    EXPECT_NE(my_spill.GetSpillNumber(), my_spill_equal.GetSpillNumber());

    // test that we free okay on reallocation
    my_spill_equal = my_spill;

    // test that equality on myself doesn't cause trouble
    my_spill_equal = my_spill_equal;

    // check that we handle copy of NULL data okay
    // (i.e. don't attempt to copy!)
    Spill default_event;
    my_spill_equal = default_event;
    EXPECT_EQ(daq_null, my_spill_equal.GetDAQData());
    EXPECT_EQ(scalars_null, my_spill_equal.GetScalars());
    EXPECT_EQ(emr_null, my_spill_equal.GetEMRSpillData());
    EXPECT_EQ(mc_null, my_spill_equal.GetMCEvents());
    EXPECT_EQ(rec_null, my_spill_equal.GetReconEvents());
}

}

