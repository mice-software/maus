//This file is a part of G4MICE
//
//G4MICE is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//G4MICE is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with G4MICE in the doc folder.  If not, see 
//<http://www.gnu.org/licenses/>.

#include "gtest/gtest.h" 

#include "src/legacy/Interface/dataCards.hh"
#include "Utils/Exception.hh"

#include <sstream>

namespace {

/////////////// datacards Tests ///////////////
// Just test readkeys(std::istream) so far

class dataCardsTest : public ::testing::Test {
protected:
  dataCardsTest() : key_int("numEvts"), val_int(10),  key_dub("MaxZInChannel"), val_dub(1.), units_dub("m"),
                  bad_units_dub("nnnn"), key_str("Description"), val_str("@"), bad_key("NotAKey") {
    ss_good << key_int << " " << val_int << "\n" << key_dub << " " << val_dub << " " << units_dub << "\n"
            << key_str << " " << val_str << "\n";
    ss_good  << "!" << bad_key << " " << val_dub << " " << units_dub << "\n"; 
    ss_good  << "#" << bad_key << " " << val_dub << " " << units_dub << "\n"; 
    ss_bad_key   << ss_good.str() << bad_key << " " << val_dub << " " << units_dub; 
  }

  virtual ~dataCardsTest() {}
  virtual void SetUp    () {}
  virtual void TearDown () {}

  std::string key_int; int         val_int;
  std::string key_dub; double      val_dub; std::string units_dub; std::string bad_units_dub;
  std::string key_str; std::string val_str;
  std::string bad_key;

  std::stringstream ss_good;
  std::stringstream ss_comments;
  std::stringstream ss_bad_key;
};

//! Test read keys
//! 
//! Check that I can read int, double, string types
//! hep3vector and vector types don't actually seem
//! to have any instantiations so don't test for them
//! 
//! Also check that bad key and bad units throws an exception
TEST_F(dataCardsTest, readKeysTest)
{
  dataCards dc(0);
  EXPECT_NO_THROW(dc.readKeys(ss_good));
  EXPECT_EQ(dc.fetchValueInt   (key_int), val_int);
  EXPECT_EQ(dc.fetchValueDouble(key_dub), val_dub);
  EXPECT_EQ(dc.fetchValueString(key_str), val_str);
  EXPECT_THROW(dc.readKeys(ss_bad_key),   Exceptions::Exception);
}


} //namespace


