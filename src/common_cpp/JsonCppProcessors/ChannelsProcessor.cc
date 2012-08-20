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

#include "src/common_cpp/JsonCppProcessors/ChannelsProcessor.hh"

namespace MAUS {

ChannelsProcessor::ChannelsProcessor()
    : _ch27_proc(), _ch26_proc(), _ch25_proc(), _ch24_proc(), _ch23_proc(),
      _ch22_proc(), _ch21_proc(), _ch20_proc(), _ch29_proc(), _ch28_proc(),
      _ch16_proc(), _ch31_proc(), _ch12_proc(), _ch13_proc(), _ch10_proc(),
      _ch11_proc(), _ch30_proc(), _ch17_proc(), _ch14_proc(), _ch15_proc(),
      _ch18_proc(), _ch19_proc(), _ch0_proc(), _ch1_proc(), _ch2_proc(),
      _ch3_proc(), _ch4_proc(), _ch5_proc(), _ch6_proc(), _ch7_proc(),
      _ch8_proc(), _ch9_proc() {
    RegisterValueBranch
          ("ch27", &_ch27_proc, &Channels::GetCh27,
          &Channels::SetCh27, true);
    RegisterValueBranch
          ("ch26", &_ch26_proc, &Channels::GetCh26,
          &Channels::SetCh26, true);
    RegisterValueBranch
          ("ch25", &_ch25_proc, &Channels::GetCh25,
          &Channels::SetCh25, true);
    RegisterValueBranch
          ("ch24", &_ch24_proc, &Channels::GetCh24,
          &Channels::SetCh24, true);
    RegisterValueBranch
          ("ch23", &_ch23_proc, &Channels::GetCh23,
          &Channels::SetCh23, true);
    RegisterValueBranch
          ("ch22", &_ch22_proc, &Channels::GetCh22,
          &Channels::SetCh22, true);
    RegisterValueBranch
          ("ch21", &_ch21_proc, &Channels::GetCh21,
          &Channels::SetCh21, true);
    RegisterValueBranch
          ("ch20", &_ch20_proc, &Channels::GetCh20,
          &Channels::SetCh20, true);
    RegisterValueBranch
          ("ch29", &_ch29_proc, &Channels::GetCh29,
          &Channels::SetCh29, true);
    RegisterValueBranch
          ("ch28", &_ch28_proc, &Channels::GetCh28,
          &Channels::SetCh28, true);
    RegisterValueBranch
          ("ch16", &_ch16_proc, &Channels::GetCh16,
          &Channels::SetCh16, true);
    RegisterValueBranch
          ("ch31", &_ch31_proc, &Channels::GetCh31,
          &Channels::SetCh31, true);
    RegisterValueBranch
          ("ch12", &_ch12_proc, &Channels::GetCh12,
          &Channels::SetCh12, true);
    RegisterValueBranch
          ("ch13", &_ch13_proc, &Channels::GetCh13,
          &Channels::SetCh13, true);
    RegisterValueBranch
          ("ch10", &_ch10_proc, &Channels::GetCh10,
          &Channels::SetCh10, true);
    RegisterValueBranch
          ("ch11", &_ch11_proc, &Channels::GetCh11,
          &Channels::SetCh11, true);
    RegisterValueBranch
          ("ch30", &_ch30_proc, &Channels::GetCh30,
          &Channels::SetCh30, true);
    RegisterValueBranch
          ("ch17", &_ch17_proc, &Channels::GetCh17,
          &Channels::SetCh17, true);
    RegisterValueBranch
          ("ch14", &_ch14_proc, &Channels::GetCh14,
          &Channels::SetCh14, true);
    RegisterValueBranch
          ("ch15", &_ch15_proc, &Channels::GetCh15,
          &Channels::SetCh15, true);
    RegisterValueBranch
          ("ch18", &_ch18_proc, &Channels::GetCh18,
          &Channels::SetCh18, true);
    RegisterValueBranch
          ("ch19", &_ch19_proc, &Channels::GetCh19,
          &Channels::SetCh19, true);
    RegisterValueBranch
          ("ch0", &_ch0_proc, &Channels::GetCh0,
          &Channels::SetCh0, true);
    RegisterValueBranch
          ("ch1", &_ch1_proc, &Channels::GetCh1,
          &Channels::SetCh1, true);
    RegisterValueBranch
          ("ch2", &_ch2_proc, &Channels::GetCh2,
          &Channels::SetCh2, true);
    RegisterValueBranch
          ("ch3", &_ch3_proc, &Channels::GetCh3,
          &Channels::SetCh3, true);
    RegisterValueBranch
          ("ch4", &_ch4_proc, &Channels::GetCh4,
          &Channels::SetCh4, true);
    RegisterValueBranch
          ("ch5", &_ch5_proc, &Channels::GetCh5,
          &Channels::SetCh5, true);
    RegisterValueBranch
          ("ch6", &_ch6_proc, &Channels::GetCh6,
          &Channels::SetCh6, true);
    RegisterValueBranch
          ("ch7", &_ch7_proc, &Channels::GetCh7,
          &Channels::SetCh7, true);
    RegisterValueBranch
          ("ch8", &_ch8_proc, &Channels::GetCh8,
          &Channels::SetCh8, true);
    RegisterValueBranch
          ("ch9", &_ch9_proc, &Channels::GetCh9,
          &Channels::SetCh9, true);
}
}  // namespace MAUS


