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

#include <sstream>
#include <iostream>

#include "Utils/Exception.hh"
#include "src/common_cpp/DataStructure/DateTime.hh"

namespace MAUS {

DateTime::DateTime() : _date_time("1976-04-04T00:00:00.000000") {
}

DateTime::DateTime(const DateTime& rhs) : _date_time(rhs._date_time) {
}

DateTime& DateTime::operator=(const DateTime& rhs) {
    if (this == &rhs) {
        return *this;
    }
    _date_time = rhs._date_time;
    return *this;
}

DateTime::~DateTime() {}

std::string DateTime::GetDateTime() const {
    return _date_time;
}

void DateTime::SetDateTime(std::string datetime) {
    if (!CheckFormat(datetime))
        throw(Exceptions::Exception(Exceptions::recoverable,
          "Malformatted date time string "+datetime+\
          " should be like YYYY-MM-DDTHH:MM:SS.mmmmmm", "DateTime::SetDateTime"
        ));
    _date_time = datetime;
}


bool DateTime::CheckFormat(std::string datetime) {
    // there is strptime, it's pretty clunky and doesnt seem terribly well done.
    // (#include "time.h" in linux only) e.g. doesn't check even months properly
    if (datetime.size() != 26)
        return false;
    try {
        bool out = Check(datetime.substr(0, 4), 0, 10000); // years
        out &= Check(datetime.substr(5, 2), 1, 13); // months
        out &= CheckDay(datetime.substr(8, 2), datetime.substr(5, 2)); // days
        out &= Check(datetime.substr(11, 2), 0, 24); // hours
        out &= Check(datetime.substr(14, 2), 0, 60); // minutes
        out &= Check(datetime.substr(17, 2), 0, 60); // seconds
        out &= Check(datetime.substr(20, 6), 0, 1000000); // milliseconds
        out &= CheckSeparators(datetime); // separators
        return out;
    } catch (Exceptions::Exception exc) {
        return false;
    }
}

int DateTime::ToInt(std::string some_string) {
    std::stringstream in(some_string);
    int out;
    in >> out;
    // should be able to do this using failbits etc but for some reason fail()
    // wasn't getting set. I always find error handling on these streamer ops
    // a bit obscure.
    std::stringstream out_test;
    out_test.fill('0');
    out_test.width(some_string.size());
    out_test << out;
    if (out_test.str() != some_string) {
        throw(Exceptions::Exception(Exceptions::recoverable,
              "Failed to convert "+some_string+" to an int",
              "DateTime::ToInt"));
    }
    return out;
}

bool DateTime::Check(std::string int_as_string, int lower, int upper_plus_one) {
    int target = ToInt(int_as_string);
    return target >= lower && target < upper_plus_one;
}

bool DateTime::CheckDay(std::string day, std::string month) {
    if (month == "01") {
        return Check(day, 1, 32);
    } else if (month == "02") {
        return Check(day, 1, 29);
    } else if (month == "03") {
        return Check(day, 1, 32);
    } else if (month == "04") {
        return Check(day, 1, 31);
    } else if (month == "05") {
        return Check(day, 1, 32);
    } else if (month == "06") {
        return Check(day, 1, 31);
    } else if (month == "07") {
        return Check(day, 1, 32);
    } else if (month == "08") {
        return Check(day, 1, 32);
    } else if (month == "09") {
        return Check(day, 1, 31);
    } else if (month == "10") {
        return Check(day, 1, 32);
    } else if (month == "11") {
        return Check(day, 1, 31);
    } else if (month == "12") {
        return Check(day, 1, 32);
    } else {
            throw(Exceptions::Exception(Exceptions::recoverable, "Could not resolve month "+month,
                         "DateTime::CheckDay"));
    }
}

bool DateTime::CheckSeparators(std::string datetime) {
    // YYYY-MM-DDTHH:MM:SS.mmmmmm
    bool out = datetime[4] == '-';
    out &= datetime[7] == '-';
    out &= datetime[13] == ':';
    out &= datetime[16] == ':';
    out &= datetime[19] == '.';
    return out;
}
}

