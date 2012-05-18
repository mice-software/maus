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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_CHANNELS_
#define _SRC_COMMON_CPP_DATASTRUCTURE_CHANNELS_

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class Channels comment
 *
 *  @var ch27  <--description-->
 *  @var ch26  <--description-->
 *  @var ch25  <--description-->
 *  @var ch24  <--description-->
 *  @var ch23  <--description-->
 *  @var ch22  <--description-->
 *  @var ch21  <--description-->
 *  @var ch20  <--description-->
 *  @var ch29  <--description-->
 *  @var ch28  <--description-->
 *  @var ch16  <--description-->
 *  @var ch31  <--description-->
 *  @var ch12  <--description-->
 *  @var ch13  <--description-->
 *  @var ch10  <--description-->
 *  @var ch11  <--description-->
 *  @var ch30  <--description-->
 *  @var ch17  <--description-->
 *  @var ch14  <--description-->
 *  @var ch15  <--description-->
 *  @var ch18  <--description-->
 *  @var ch19  <--description-->
 *  @var ch0  <--description-->
 *  @var ch1  <--description-->
 *  @var ch2  <--description-->
 *  @var ch3  <--description-->
 *  @var ch4  <--description-->
 *  @var ch5  <--description-->
 *  @var ch6  <--description-->
 *  @var ch7  <--description-->
 *  @var ch8  <--description-->
 *  @var ch9  <--description-->
 */

class Channels {
  public:
    /** Default constructor - initialises to 0/NULL */
    Channels();

    /** Copy constructor - any pointers are deep copied */
    Channels(const Channels& _channels);

    /** Equality operator - any pointers are deep copied */
    Channels& operator=(const Channels& _channels);

    /** Destructor - any member pointers are deleted */
    ~Channels();


    /** Returns Ch27 */
    int GetCh27() const;

    /** Sets Ch27 */
    void SetCh27(int ch27);

    /** Returns Ch26 */
    int GetCh26() const;

    /** Sets Ch26 */
    void SetCh26(int ch26);

    /** Returns Ch25 */
    int GetCh25() const;

    /** Sets Ch25 */
    void SetCh25(int ch25);

    /** Returns Ch24 */
    int GetCh24() const;

    /** Sets Ch24 */
    void SetCh24(int ch24);

    /** Returns Ch23 */
    int GetCh23() const;

    /** Sets Ch23 */
    void SetCh23(int ch23);

    /** Returns Ch22 */
    int GetCh22() const;

    /** Sets Ch22 */
    void SetCh22(int ch22);

    /** Returns Ch21 */
    int GetCh21() const;

    /** Sets Ch21 */
    void SetCh21(int ch21);

    /** Returns Ch20 */
    int GetCh20() const;

    /** Sets Ch20 */
    void SetCh20(int ch20);

    /** Returns Ch29 */
    int GetCh29() const;

    /** Sets Ch29 */
    void SetCh29(int ch29);

    /** Returns Ch28 */
    int GetCh28() const;

    /** Sets Ch28 */
    void SetCh28(int ch28);

    /** Returns Ch16 */
    int GetCh16() const;

    /** Sets Ch16 */
    void SetCh16(int ch16);

    /** Returns Ch31 */
    int GetCh31() const;

    /** Sets Ch31 */
    void SetCh31(int ch31);

    /** Returns Ch12 */
    int GetCh12() const;

    /** Sets Ch12 */
    void SetCh12(int ch12);

    /** Returns Ch13 */
    int GetCh13() const;

    /** Sets Ch13 */
    void SetCh13(int ch13);

    /** Returns Ch10 */
    int GetCh10() const;

    /** Sets Ch10 */
    void SetCh10(int ch10);

    /** Returns Ch11 */
    int GetCh11() const;

    /** Sets Ch11 */
    void SetCh11(int ch11);

    /** Returns Ch30 */
    int GetCh30() const;

    /** Sets Ch30 */
    void SetCh30(int ch30);

    /** Returns Ch17 */
    int GetCh17() const;

    /** Sets Ch17 */
    void SetCh17(int ch17);

    /** Returns Ch14 */
    int GetCh14() const;

    /** Sets Ch14 */
    void SetCh14(int ch14);

    /** Returns Ch15 */
    int GetCh15() const;

    /** Sets Ch15 */
    void SetCh15(int ch15);

    /** Returns Ch18 */
    int GetCh18() const;

    /** Sets Ch18 */
    void SetCh18(int ch18);

    /** Returns Ch19 */
    int GetCh19() const;

    /** Sets Ch19 */
    void SetCh19(int ch19);

    /** Returns Ch0 */
    int GetCh0() const;

    /** Sets Ch0 */
    void SetCh0(int ch0);

    /** Returns Ch1 */
    int GetCh1() const;

    /** Sets Ch1 */
    void SetCh1(int ch1);

    /** Returns Ch2 */
    int GetCh2() const;

    /** Sets Ch2 */
    void SetCh2(int ch2);

    /** Returns Ch3 */
    int GetCh3() const;

    /** Sets Ch3 */
    void SetCh3(int ch3);

    /** Returns Ch4 */
    int GetCh4() const;

    /** Sets Ch4 */
    void SetCh4(int ch4);

    /** Returns Ch5 */
    int GetCh5() const;

    /** Sets Ch5 */
    void SetCh5(int ch5);

    /** Returns Ch6 */
    int GetCh6() const;

    /** Sets Ch6 */
    void SetCh6(int ch6);

    /** Returns Ch7 */
    int GetCh7() const;

    /** Sets Ch7 */
    void SetCh7(int ch7);

    /** Returns Ch8 */
    int GetCh8() const;

    /** Sets Ch8 */
    void SetCh8(int ch8);

    /** Returns Ch9 */
    int GetCh9() const;

    /** Sets Ch9 */
    void SetCh9(int ch9);

  private:
    int _ch27;
    int _ch26;
    int _ch25;
    int _ch24;
    int _ch23;
    int _ch22;
    int _ch21;
    int _ch20;
    int _ch29;
    int _ch28;
    int _ch16;
    int _ch31;
    int _ch12;
    int _ch13;
    int _ch10;
    int _ch11;
    int _ch30;
    int _ch17;
    int _ch14;
    int _ch15;
    int _ch18;
    int _ch19;
    int _ch0;
    int _ch1;
    int _ch2;
    int _ch3;
    int _ch4;
    int _ch5;
    int _ch6;
    int _ch7;
    int _ch8;
    int _ch9;

    ClassDef(Channels, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_CHANNELS_

