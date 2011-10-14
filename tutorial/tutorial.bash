#!/bin/bash
python ./files/text_1.py
source ./../env.sh
python ./files/text_2.py
./../bin/simulate_mice.py --simulation_geometry_filename Stage4.dat --spill_generator_number_of_spills 5
python ./files/text_3.py
python ./files/xboa_graph.py
