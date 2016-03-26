#!/bin/bash
export geometry_run=07417
export geometry_dir=geometry_${geometry_run}
rm -r ${geometry_dir}
mkdir ${geometry_dir}
python ${MAUS_ROOT_DIR}/bin/utilities/download_geometry.py --geometry_download_by run_number --geometry_download_directory ${geometry_dir} --geometry_download_run ${geometry_run} --cdb_download_url "http://cdb.mice.rl.ac.uk/cdb/"

exit 0

export geometry_id=736
export geometry_dir=geometry_preprod${geometry_id}
rm -r ${geometry_dir}
mkdir ${geometry_dir}

#python ${MAUS_ROOT_DIR}/bin/utilities/download_geometry.py --geometry_download_by id --geometry_download_directory ${geometry_dir} --geometry_download_id ${geometry_id} --cdb_download_url "http://preprodcdb.mice.rl.ac.uk/cdb/"

echo "**********************\n\n\n"
echo "YOU MUST CHECK THE CURRENTS FOR PREPROD GEOMETRY"
