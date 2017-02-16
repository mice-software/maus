#!/bin/bash
export geometry_run=07469
export geometry_dir=${MAUS_ROOT_DIR}/tests/cpp_unit/Recon/Kalman/geometry_${geometry_run}
rm -r ${geometry_dir}
mkdir ${geometry_dir}
python ${MAUS_ROOT_DIR}/bin/utilities/download_geometry.py --geometry_download_by run_number --geometry_download_directory ${geometry_dir} --geometry_download_run ${geometry_run} --cdb_download_url "http://cdb.mice.rl.ac.uk/cdb/"

