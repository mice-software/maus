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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_CHANNELSPROCESSOR_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_CHANNELSPROCESSOR_

#include "src/common_cpp/DataStructure/Channels.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

/** @class ChannelsProcessor Conversions for Channels between C++ and Json 
 *
 *  @var _ch27_proc_proc Processor for _ch27
 *  @var _ch26_proc_proc Processor for _ch26
 *  @var _ch25_proc_proc Processor for _ch25
 *  @var _ch24_proc_proc Processor for _ch24
 *  @var _ch23_proc_proc Processor for _ch23
 *  @var _ch22_proc_proc Processor for _ch22
 *  @var _ch21_proc_proc Processor for _ch21
 *  @var _ch20_proc_proc Processor for _ch20
 *  @var _ch29_proc_proc Processor for _ch29
 *  @var _ch28_proc_proc Processor for _ch28
 *  @var _ch16_proc_proc Processor for _ch16
 *  @var _ch31_proc_proc Processor for _ch31
 *  @var _ch12_proc_proc Processor for _ch12
 *  @var _ch13_proc_proc Processor for _ch13
 *  @var _ch10_proc_proc Processor for _ch10
 *  @var _ch11_proc_proc Processor for _ch11
 *  @var _ch30_proc_proc Processor for _ch30
 *  @var _ch17_proc_proc Processor for _ch17
 *  @var _ch14_proc_proc Processor for _ch14
 *  @var _ch15_proc_proc Processor for _ch15
 *  @var _ch18_proc_proc Processor for _ch18
 *  @var _ch19_proc_proc Processor for _ch19
 *  @var _ch0_proc_proc Processor for _ch0
 *  @var _ch1_proc_proc Processor for _ch1
 *  @var _ch2_proc_proc Processor for _ch2
 *  @var _ch3_proc_proc Processor for _ch3
 *  @var _ch4_proc_proc Processor for _ch4
 *  @var _ch5_proc_proc Processor for _ch5
 *  @var _ch6_proc_proc Processor for _ch6
 *  @var _ch7_proc_proc Processor for _ch7
 *  @var _ch8_proc_proc Processor for _ch8
 *  @var _ch9_proc_proc Processor for _ch9
 */

class ChannelsProcessor : public ObjectProcessor<Channels> {
  public:
    /** Set up processors and register branches
     *
     *  Everything else is handled by the base class
     */
    ChannelsProcessor();

  private:
    IntProcessor _ch27_proc;
    IntProcessor _ch26_proc;
    IntProcessor _ch25_proc;
    IntProcessor _ch24_proc;
    IntProcessor _ch23_proc;
    IntProcessor _ch22_proc;
    IntProcessor _ch21_proc;
    IntProcessor _ch20_proc;
    IntProcessor _ch29_proc;
    IntProcessor _ch28_proc;
    IntProcessor _ch16_proc;
    IntProcessor _ch31_proc;
    IntProcessor _ch12_proc;
    IntProcessor _ch13_proc;
    IntProcessor _ch10_proc;
    IntProcessor _ch11_proc;
    IntProcessor _ch30_proc;
    IntProcessor _ch17_proc;
    IntProcessor _ch14_proc;
    IntProcessor _ch15_proc;
    IntProcessor _ch18_proc;
    IntProcessor _ch19_proc;
    IntProcessor _ch0_proc;
    IntProcessor _ch1_proc;
    IntProcessor _ch2_proc;
    IntProcessor _ch3_proc;
    IntProcessor _ch4_proc;
    IntProcessor _ch5_proc;
    IntProcessor _ch6_proc;
    IntProcessor _ch7_proc;
    IntProcessor _ch8_proc;
    IntProcessor _ch9_proc;
};
}  // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_JSONCPPPROCESSORS_CHANNELSPROCESSOR_

