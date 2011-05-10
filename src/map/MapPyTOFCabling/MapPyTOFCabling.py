#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.


## @class MapPyTOFCabling.MapPyTOFCabling
#  MapPyTOFCabling creates spills with a Stage 6
#  demo event with tracks and hits in detectors.
#
#  This is useful for testing mappers that work on simulation
#  data or doing load testing.
#

import json
import types
import os
import io

# This is the former tofcabling.txt file. Changes are FORBIDDEN IN TRUNK to this until
# this feature is closed: http://micewww.pp.rl.ac.uk/issues/439
#
# This is currently the bad way of doing this.  If you make it worse, I suggest
# you think of your well being and review "caning" in wikipedia.  The comments
# about which column is which is wrong and you need to consult the issue tracker
# conversation above.
#
# Nchannels = 116
#       ADC     TDC     TOF
#crate  board   channel crate   board   channel station plane   slab    pmt     pmtNum  pmtName
temp_tofcabling = io.StringIO(u"""
1       4	0	1	3	04	0	0	0	0	022	tof0s0
1	0	0	1	1	16	0	0	1	0	024	tof0s1
1	0	1	1	1	17	0	0	2	0	026	tof0s2
1	0	2	1	1	18	0	0	3	0	028	tof0s3
1	0	3	1	1	19	0	0	4	0	030	tof0s4
1	0	4	1	1	20	0	0	5	0	032	tof0s5
1	0	5	1	1	21	0	0	6	0	034	tof0s6
1	0	6	1	1	22	0	0	7	0	036	tof0s7
1	0	7	1	1	23	0	0	8	0	038	tof0s8
1	4	1	1	3	06	0	0	9	0	040	tof0s9
1	4	4	1	3	12	0	0	0	1	021	tof0n0
1	2	0	1	2	16	0	0	1	1	023	tof0n1
1	2	1	1	2	17	0	0	2	1	025	tof0n2
1	2	2	1	2	18	0	0	3	1	027	tof0n3
1	2	3	1	2	19	0	0	4	1	029	tof0n4
1	2	4	1	2	20	0	0	5	1	031	tof0n5
1	2	5	1	2	21	0	0	6	1	033	tof0n6
1	2	6	1	2	22	0	0	7	1	035	tof0n7
1	2	7	1	2	23	0	0	8	1	037	tof0n8
1	4	5	1	3	14	0	0	9	1	039	tof0n9
1	4	6	1	3	13	0	1	0	1	020	tof0t0
1	3	0	1	2	24	0	1	1	1	018	tof0t1
1	3	1	1	2	25	0	1	2	1	016	tof0t2
1	3	2	1	2	26	0	1	3	1	014	tof0t3
1	3	3	1	2	27	0	1	4	1	012	tof0t4
1	3	4	1	2	28	0	1	5	1	010	tof0t5
1	3	5	1	2	29	0	1	6	1	08	tof0t6
1	3	6	1	2	30	0	1	7	1	06	tof0t7
1	3	7	1	2	31	0	1	8	1	04	tof0t8
1	4	7	1	3	15	0	1	9	1	02	tof0t9
1	4	2	1	3	05	0	1	0	0	019	tof0b0
1	1	0	1	1	24	0	1	1	0	017	tof0b1
1	1	1	1	1	25	0	1	2	0	015	tof0b2
1	1	2	1	1	26	0	1	3	0	013	tof0b3
1	1	3	1	1	27	0	1	4	0	011	tof0b4
1	1	4	1	1	28	0	1	5	0	09	tof0b5
1	1	5	1	1	29	0	1	6	0	07	tof0b6
1	1	6	1	1	30	0	1	7	0	05	tof0b7
1	1	7	1	1	31	0	1	8	0	03	tof0b8
1	4	3	1	3	07	0	1	9	0	01	tof0b9
1	5	0	1	1	2	1	0	0	0	116	tof1s0
1	5	1	1	1	3	1	0	1	0	118	tof1s1
1	5	2	1	1	4	1	0	2	0	120	tof1s2
1	5	3	1	1	5	1	0	3	0	122	tof1s3
1	5	4	1	1	6	1	0	4	0	124	tof1s4
1	5	5	1	1	7	1	0	5	0	126	tof1s5
1	5	6	1	1	8	1	0	6	0	128	tof1s6
1	7	0	1	2	2	1	0	0	1	115	tof1n0
1	7	1	1	2	3	1	0	1	1	117	tof1n1
1	7	2	1	2	4	1	0	2	1	119	tof1n2
1	7	3	1	2	5	1	0	3	1	121	tof1n3
1	7	4	1	2	6	1	0	4	1	123	tof1n4
1	7	5	1	2	7	1	0	5	1	125	tof1n5
1	7	6	1	2	8	1	0	6	1	127	tof1n6
1	6	0	1	1	9	1	1	0	0	113	tof1b0
1	6	1	1	1	10	1	1	1	0	111	tof1b1
1	6	2	1	1	11	1	1	2	0	109	tof1b2
1	6	3	1	1	12	1	1	3	0	107	tof1b3
1	6	4	1	1	13	1	1	4	0	105	tof1b4
1	6	5	1	1	14	1	1	5	0	103	tof1b5
1	6	6	1	1	15	1	1	6	0	101	tof1b6
1	8	0	1	2	9	1	1	0	1	114	tof1t0
1	8	1	1	2	10	1	1	1	1	112	tof1t1
1	8	2	1	2	11	1	1	2	1	110	tof1t2
1	8	3	1	2	12	1	1	3	1	108	tof1t3
1	8	4	1	2	13	1	1	4	1	106	tof1t4
1	8	5	1	2	14	1	1	5	1	104	tof1t5
1	8	6	1	2	15	1	1	6	1	102	tof1t6
0	22	0	1	4	04	2	0	0	1	220	tof2s0
0	18	0	1	3	16	2	0	1	1	218	tof2s1
0	18	1	1	3	17	2	0	2	1	216	tof2s2
0	18	2	1	3	18	2	0	3	1	214	tof2s3
0	18	3	1	3	19	2	0	4	1	212	tof2s4
0	18	4	1	3	20	2	0	5	1	210	tof2s5
0	18	5	1	3	21	2	0	6	1	208	tof2s6
0	18	6	1	3	22	2	0	7	1	206	tof2s7
0	18	7	1	3	23	2	0	8	1	204	tof2s8
0	22	4	1	4	12	2	0	9	1	202	tof2s9
0	22	2	1	4	06	2	0	0	0	222	tof2n0
0	20	0	1	4	16	2	0	1	0	224	tof2n1
0	20	1	1	4	17	2	0	2	0	226	tof2n2
0	20	2	1	4	18	2	0	3	0	228	tof2n3
0	20	3	1	4	19	2	0	4	0	230	tof2n4
0	20	4	1	4	20	2	0	5	0	232	tof2n5
0	20	5	1	4	21	2	0	6	0	234	tof2n6
0	20	6	1	4	22	2	0	7	0	236	tof2n7
0	20	7	1	4	23	2	0	8	0	238	tof2n8
0	22	6	1	4	14	2	0	9	0	240	tof2n9
0	22	1	1	4	05	2	1	0	0	219	tof2b0
0	19	0	1	3	24	2	1	1	0	217	tof2b1
0	19	1	1	3	25	2	1	2	0	215	tof2b2
0	19	2	1	3	26	2	1	3	0	213	tof2b3
0	19	3	1	3	27	2	1	4	0	211	tof2b4
0	19	4	1	3	28	2	1	5	0	209	tof2b5
0	19	5	1	3	29	2	1	6	0	207	tof2b6
0	19	6	1	3	30	2	1	7	0	205	tof2b7
0	19	7	1	3	31	2	1	8	0	203	tof2b8
0	22	5	1	4	13	2	1	9	0	201	tof2b9
0	22	3	1	4	07	2	1	0	1	221	tof2t0
0	21	0	1	4	24	2	1	1	1	223	tof2t1
0	21	1	1	4	25	2	1	2	1	225	tof2t2
0	21	2	1	4	26	2	1	3	1	227	tof2t3
0	21	3	1	4	27	2	1	4	1	229	tof2t4
0	21	4	1	4	28	2	1	5	1	231	tof2t5
0	21	5	1	4	29	2	1	6	1	233	tof2t6
0	21	6	1	4	30	2	1	7	1	235	tof2t7
0	21	7	1	4	31	2	1	8	1	237	tof2t8
0	22	7	1	4	15	2	1	9	1	239	tof2t9
99	99	99	1	1	0	99	99	99	99	99	trigger
99	99	99	1	2	0	99	99	99	99	99	trigger
99	99	99	1	3	0	99	99	99	99	99	trigger
99	99	99	1	4	0	99	99	99	99	99	trigger
99	99	99	1	1	1	99	99	99	99	99	triggerRequest
99	99	99	1	2	1	99	99	99	99	99	triggerRequest
99	99	99	1	3	1	99	99	99	99	99	triggerRequest
99	99	99	1	4	1	99	99	99	99	99	triggerRequest
""")

class MapPyTOFCabling:
    def birth(self, configJSON):
        config = json.loads(configJSON)

        self.cabling = []

        for line in temp_tofcabling:
            line = line.rstrip()
            if not line:  # skip starting and ending empty lines
                continue
            
            line_parts = line.split()
            assert(len(line_parts) == 12)
            
            [tdc_crate, tdc_board, tdc_channel]  = map(int, line_parts[3:6])

            channel_id = {}
            channel_id['tdc_crate'] = tdc_crate
            channel_id['tdc_board'] = tdc_board
            channel_id['tdc_channel'] = tdc_channel

            station, plane, slab, pmt, pmt_num = map(int, line_parts[6:11])
            channel_id = {}
            channel_id['tof_station'] = station
            channel_id['tof_plane'] = plane
            channel_id['tof_slab'] = slab
            channel_id['tof_pmt'] = pmt
            channel_id['tof_pmt_num'] = pmt_num

            self.cabling.append(channel_id)
        
        return True

    def process(self, str):
        spill = json.loads(str)

        if 'daq_data' not in spill:
            return str
        
        if spill['daq_data'] is None:
            return str

        digits = {}

        for subdata in spill['daq_data']:
            if subdata is None:
                continue

            if 'tdc' in subdata.keys():
                 #print subdata['tdc'].keys()
                 subsubdata = subdata['tdc']
                 #print subdata
                 #print 'geo', subsubdata['geo']  # geo is the geographic position of a board
                 #print 'ttt', subsubdata['trigger_time_tag']

                 new_hits = []
                 
                 # find triggers
                 trigger_time = None
                 for hit in subsubdata['hits']:
                     if hit['channel'] == 0:
                         print hit
                         assert(trigger_time == None)
                         trigger_time = hit['leading_time']

                 for hit in subsubdata['hits']:
                     del hit['trailing_time']
                     new_hits.append(hit)

                     print subsubdata['ldc_id'], subsubdata['geo'], hit['channel'], hit['leading_time'] - trigger_time

        

        return json.dumps(spill)

    def death(self):
        return True
