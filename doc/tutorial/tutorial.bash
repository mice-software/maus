#!/bin/bash
python ${MAUS_ROOT_DIR}/doc/tutorial/files/text_1.py
source ${MAUS_ROOT_DIR}/env.sh
python ${MAUS_ROOT_DIR}/doc/tutorial/files/text_2.py
${MAUS_ROOT_DIR}/bin/simulate_mice.py --simulation_geometry_filename Stage4.dat --spill_generator_number_of_spills 5
python ${MAUS_ROOT_DIR}/doc/tutorial/files/text_3.py
python ${MAUS_ROOT_DIR}/doc/tutorial/files/xboa_graph.py
