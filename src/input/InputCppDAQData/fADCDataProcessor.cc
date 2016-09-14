/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

#include "src/input/InputCppDAQData/fADCDataProcessor.hh"
#include "Utils/Exception.hh"

namespace MAUS {

MAUS::DAQData* MDarranger::_daq_data = 0;
DAQChannelMap* MDarranger::_chMap = 0;

int fADCDataProcessor::get_area() {
  int area = 0;
  for ( unsigned int ich = 0; ich < _data.size(); ich++)
    area += abs(_data[ich]);
  return static_cast<int>(_pedestal*_data.size()-area+0.5);
}

void fADCDataProcessor::set_pedestal() {
  int size = _data.size();
//   cerr << size << endl;
  if (size < SIGNAL_INTEGRATION_WINDOW*2) {
    _pedestal = 0;

     throw(Exceptions::Exception(Exceptions::recoverable,
           "The data size is too short.",
           "fADCDataProcessor::set_pedestal()"));
  }

  /* check if there is no signal in first 40 bins */
  vector<int>::iterator min;
  min = min_element(_data.begin(), _data.end());
  int pos = distance(_data.begin(), min);

  double area = 0;
  int i_0, i = 0;
  if (pos > SIGNAL_INTEGRATION_WINDOW)
    i_0 = 0;
  else
    i_0 = _data.size() - SIGNAL_INTEGRATION_WINDOW;

  while (i < SIGNAL_INTEGRATION_WINDOW) {
     area += _data[i+i_0];
     i++;
  }
  _pedestal = area/SIGNAL_INTEGRATION_WINDOW;
}

int fADCDataProcessor::chargeMinMax() {
  int min, max, d;
  d = -99;
  max = INT_MIN;
  min = INT_MAX;
  for ( unsigned int i = 0; i < _data.size(); ++i ) {
    d = _data[ i ];
    min = ( min > d )? d : min;
    max = ( max > d )? max : d;
  }

  return max - min;
}

int fADCDataProcessor::print_data() {
  vector<int>::iterator it = _data.begin();
  int i = 0;
  if (_data.size()) {
    while (it < _data.end()) {
      std::cout << "sample: " << i << "   value: " << *it << std::endl;
      it++;
      i++;
    }
  }
  return _data.size();
}

int fADCDataProcessor::get_neg_signal_area(int& pos) {
  vector<int>::iterator it = _data.begin();
  vector<int>::iterator min;
  int area = 0;
  if (_data.size()) {
    min = min_element(_data.begin(), _data.end());
    pos = distance(_data.begin(), min);
    int nSamples = 0;
    if (pos > 10)
      it = min -  10;
    else
      it = _data.begin();

    while (it < min + SIGNAL_INTEGRATION_WINDOW - 10 && it < _data.end()) {
      // area += abs(*it - _pedestal);
      area += *it;
      // std::cout << "val: " << *it << "  integr. area: " << area << std::endl;
      it++;
      nSamples++;
    }
    return static_cast<int>(_pedestal*nSamples-area+0.5);
  }
  return 0;
}


int fADCDataProcessor::get_pos_signal_area(int& pos) {
  vector<int>::iterator it = _data.begin();
  vector<int>::iterator max;
  int area = 0;
  if (_data.size()) {
    max = max_element(_data.begin(), _data.end());
    pos = distance(_data.begin(), max);
    int nSamples = 0;
    if (pos > 10)
      it = max -  10;
    else
      it = _data.begin();

    while (it < max + SIGNAL_INTEGRATION_WINDOW - 10 && it < _data.end()) {
      // area += abs(*it - _pedestal);
      area += *it;
      // std::cout << "val: " << *it << "  integr. area: " << area << std::endl;
      it++;
      nSamples++;
    }
    return static_cast<int>(-_pedestal*nSamples+area+0.5);
  }
  return 0;
}

int fADCDataProcessor::get_arrival_time() {
  int arr_time = 0;
  for ( unsigned int i = 0; i < _data.size(); ++i ) {
    arr_time = i;
    if ( abs(static_cast<int>(_pedestal+0.5) - _data[i]) > 2 ) {
      break;
    }
  }
    return arr_time;
}

int fADCDataProcessor::get_pedestal_area(int& pos) {
  vector<int>::iterator it = _data.begin();
  vector<int>::iterator max;
  int pedareaminus = 0;
  int pedareaplus = 0;

  if (_data.size()) {
    max = max_element(_data.begin(), _data.end());
    pos = distance(_data.begin(), max);

    if (pos > 30 && pos < 97) {
      it = max - 30;
      while (it < max-10) {
        pedareaminus += *it - static_cast<int>(_pedestal+0.5);
        it++;
      }

      it = max + 20;
      while (it < max+30) {
        pedareaplus += *it - static_cast<int>(_pedestal+0.5);
        it++;
      }
    }

    return pedareaminus + pedareaplus;
  }

  return 0;
}

int fADCDataProcessor::get_max_position() {
  int pos = 0;
  vector<int>::iterator max;
  if (_data.size()) {
    max = max_element(_data.begin(), _data.end());
    pos = distance(_data.begin(), max);

    return pos;
  }

  return 0;
}

int fADCDataProcessor::get_min_position() {
  int pos = 0;
  vector<int>::iterator min;
  if (_data.size()) {
    min = min_element(_data.begin(), _data.end());
    pos = distance(_data.begin(), min);

    return pos;
  }

  return 0;
}

int fADCDataProcessor::chargePedMax() {
  int max = 0;
  if (_data.size()) {
    max = *max_element(_data.begin(), _data.end());
    return max - static_cast<int>(_pedestal+0.5);
  }

  return 0;
}

int fADCDataProcessor::chargePedMin() {
  int min = 0;
  if (_data.size()) {
    min = *min_element(_data.begin(), _data.end());
    return static_cast<int>(_pedestal+0.5) - min;
  }

  return 0;
}

int fADCDataProcessor::get_charge(int Algorithm) {
  int charge;
  switch ( Algorithm ) {
  case ceaMinMax:
    charge = chargeMinMax();
    break;
  case ceaPedMax:
    charge = chargePedMax();
    break;
  case ceaPedMin:
    charge = chargePedMin();
    break;
  default:
    charge = -99;
    break;
  }

  return charge;
}

Json::Value fADCDataProcessor::get_samples() {

Json::Value xfAdcHit;
  for ( unsigned int i = 0; i < _data.size(); ++i ) {
    xfAdcHit.append(_data[i]);
  }
  return xfAdcHit;
}
}
