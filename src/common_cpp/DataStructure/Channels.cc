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

#include "src/common_cpp/DataStructure/Channels.hh"


namespace MAUS {

Channels::Channels()
    : _ch27(0), _ch26(0), _ch25(0), _ch24(0), _ch23(0), _ch22(0), _ch21(0),
      _ch20(0), _ch29(0), _ch28(0), _ch16(0), _ch31(0), _ch12(0), _ch13(0),
      _ch10(0), _ch11(0), _ch30(0), _ch17(0), _ch14(0), _ch15(0), _ch18(0),
      _ch19(0), _ch0(0), _ch1(0), _ch2(0), _ch3(0), _ch4(0), _ch5(0), _ch6(0),
      _ch7(0), _ch8(0), _ch9(0) {
}

Channels::Channels(const Channels& _channels)
    : _ch27(0), _ch26(0), _ch25(0), _ch24(0), _ch23(0), _ch22(0), _ch21(0),
      _ch20(0), _ch29(0), _ch28(0), _ch16(0), _ch31(0), _ch12(0), _ch13(0),
      _ch10(0), _ch11(0), _ch30(0), _ch17(0), _ch14(0), _ch15(0), _ch18(0),
      _ch19(0), _ch0(0), _ch1(0), _ch2(0), _ch3(0), _ch4(0), _ch5(0), _ch6(0),
      _ch7(0), _ch8(0), _ch9(0) {
    *this = _channels;
}

Channels& Channels::operator=(const Channels& _channels) {
    if (this == &_channels) {
        return *this;
    }
    SetCh27(_channels._ch27);
    SetCh26(_channels._ch26);
    SetCh25(_channels._ch25);
    SetCh24(_channels._ch24);
    SetCh23(_channels._ch23);
    SetCh22(_channels._ch22);
    SetCh21(_channels._ch21);
    SetCh20(_channels._ch20);
    SetCh29(_channels._ch29);
    SetCh28(_channels._ch28);
    SetCh16(_channels._ch16);
    SetCh31(_channels._ch31);
    SetCh12(_channels._ch12);
    SetCh13(_channels._ch13);
    SetCh10(_channels._ch10);
    SetCh11(_channels._ch11);
    SetCh30(_channels._ch30);
    SetCh17(_channels._ch17);
    SetCh14(_channels._ch14);
    SetCh15(_channels._ch15);
    SetCh18(_channels._ch18);
    SetCh19(_channels._ch19);
    SetCh0(_channels._ch0);
    SetCh1(_channels._ch1);
    SetCh2(_channels._ch2);
    SetCh3(_channels._ch3);
    SetCh4(_channels._ch4);
    SetCh5(_channels._ch5);
    SetCh6(_channels._ch6);
    SetCh7(_channels._ch7);
    SetCh8(_channels._ch8);
    SetCh9(_channels._ch9);
    return *this;
}

Channels::~Channels() {
}

int Channels::GetCh27() const {
    return _ch27;
}

void Channels::SetCh27(int ch27) {
    _ch27 = ch27;
}

int Channels::GetCh26() const {
    return _ch26;
}

void Channels::SetCh26(int ch26) {
    _ch26 = ch26;
}

int Channels::GetCh25() const {
    return _ch25;
}

void Channels::SetCh25(int ch25) {
    _ch25 = ch25;
}

int Channels::GetCh24() const {
    return _ch24;
}

void Channels::SetCh24(int ch24) {
    _ch24 = ch24;
}

int Channels::GetCh23() const {
    return _ch23;
}

void Channels::SetCh23(int ch23) {
    _ch23 = ch23;
}

int Channels::GetCh22() const {
    return _ch22;
}

void Channels::SetCh22(int ch22) {
    _ch22 = ch22;
}

int Channels::GetCh21() const {
    return _ch21;
}

void Channels::SetCh21(int ch21) {
    _ch21 = ch21;
}

int Channels::GetCh20() const {
    return _ch20;
}

void Channels::SetCh20(int ch20) {
    _ch20 = ch20;
}

int Channels::GetCh29() const {
    return _ch29;
}

void Channels::SetCh29(int ch29) {
    _ch29 = ch29;
}

int Channels::GetCh28() const {
    return _ch28;
}

void Channels::SetCh28(int ch28) {
    _ch28 = ch28;
}

int Channels::GetCh16() const {
    return _ch16;
}

void Channels::SetCh16(int ch16) {
    _ch16 = ch16;
}

int Channels::GetCh31() const {
    return _ch31;
}

void Channels::SetCh31(int ch31) {
    _ch31 = ch31;
}

int Channels::GetCh12() const {
    return _ch12;
}

void Channels::SetCh12(int ch12) {
    _ch12 = ch12;
}

int Channels::GetCh13() const {
    return _ch13;
}

void Channels::SetCh13(int ch13) {
    _ch13 = ch13;
}

int Channels::GetCh10() const {
    return _ch10;
}

void Channels::SetCh10(int ch10) {
    _ch10 = ch10;
}

int Channels::GetCh11() const {
    return _ch11;
}

void Channels::SetCh11(int ch11) {
    _ch11 = ch11;
}

int Channels::GetCh30() const {
    return _ch30;
}

void Channels::SetCh30(int ch30) {
    _ch30 = ch30;
}

int Channels::GetCh17() const {
    return _ch17;
}

void Channels::SetCh17(int ch17) {
    _ch17 = ch17;
}

int Channels::GetCh14() const {
    return _ch14;
}

void Channels::SetCh14(int ch14) {
    _ch14 = ch14;
}

int Channels::GetCh15() const {
    return _ch15;
}

void Channels::SetCh15(int ch15) {
    _ch15 = ch15;
}

int Channels::GetCh18() const {
    return _ch18;
}

void Channels::SetCh18(int ch18) {
    _ch18 = ch18;
}

int Channels::GetCh19() const {
    return _ch19;
}

void Channels::SetCh19(int ch19) {
    _ch19 = ch19;
}

int Channels::GetCh0() const {
    return _ch0;
}

void Channels::SetCh0(int ch0) {
    _ch0 = ch0;
}

int Channels::GetCh1() const {
    return _ch1;
}

void Channels::SetCh1(int ch1) {
    _ch1 = ch1;
}

int Channels::GetCh2() const {
    return _ch2;
}

void Channels::SetCh2(int ch2) {
    _ch2 = ch2;
}

int Channels::GetCh3() const {
    return _ch3;
}

void Channels::SetCh3(int ch3) {
    _ch3 = ch3;
}

int Channels::GetCh4() const {
    return _ch4;
}

void Channels::SetCh4(int ch4) {
    _ch4 = ch4;
}

int Channels::GetCh5() const {
    return _ch5;
}

void Channels::SetCh5(int ch5) {
    _ch5 = ch5;
}

int Channels::GetCh6() const {
    return _ch6;
}

void Channels::SetCh6(int ch6) {
    _ch6 = ch6;
}

int Channels::GetCh7() const {
    return _ch7;
}

void Channels::SetCh7(int ch7) {
    _ch7 = ch7;
}

int Channels::GetCh8() const {
    return _ch8;
}

void Channels::SetCh8(int ch8) {
    _ch8 = ch8;
}

int Channels::GetCh9() const {
    return _ch9;
}

void Channels::SetCh9(int ch9) {
    _ch9 = ch9;
}

void Channels::SetCh(int xCh, int xValue) {
    switch ( xCh ) {
        case 0:
            this->SetCh0(xValue);
        case 1:
            this->SetCh1(xValue);
        case 2:
            this->SetCh2(xValue);
        case 3:
            this->SetCh3(xValue);
        case 4:
            this->SetCh4(xValue);
        case 5:
            this->SetCh5(xValue);
        case 6:
            this->SetCh6(xValue);
        case 7:
            this->SetCh7(xValue);
        case 8:
            this->SetCh8(xValue);
        case 9:
            this->SetCh9(xValue);
        case 10:
            this->SetCh10(xValue);
        case 11:
            this->SetCh11(xValue);
        case 12:
            this->SetCh12(xValue);
        case 13:
            this->SetCh13(xValue);
        case 14:
            this->SetCh14(xValue);
        case 15:
            this->SetCh15(xValue);
        case 16:
            this->SetCh16(xValue);
        case 17:
            this->SetCh17(xValue);
        case 18:
            this->SetCh18(xValue);
        case 19:
            this->SetCh19(xValue);
        case 20:
            this->SetCh20(xValue);
        case 21:
            this->SetCh21(xValue);
        case 22:
            this->SetCh22(xValue);
        case 23:
            this->SetCh23(xValue);
        case 24:
            this->SetCh24(xValue);
        case 25:
            this->SetCh25(xValue);
        case 26:
            this->SetCh26(xValue);
        case 27:
            this->SetCh27(xValue);
        case 28:
            this->SetCh28(xValue);
        case 29:
            this->SetCh29(xValue);
        case 30:
            this->SetCh30(xValue);
        case 31:
            this->SetCh31(xValue);
    }
}
}

