#!/bin/bash

log_file="install.log"

echo "INFO: Building third party libraries including DAQ" >> $log_file

if [ ! -e "./install_build_test.bash" ] 
then
    echo "FATAL: Failed to find install_build_test.bash. Check that you are" >> $log_file
    echo "FATAL: running from the maus root directory" >> $log_file
fi
echo "INFO: Found install_build_test.bash - now building" >> $log_file
./install_build_test.bash
echo "INFO: Now building DAQ extras"
source env.sh >> $log_file
echo "INFO: Building DAQ add-on" >> $log_file
bash third_party/bash/70daq.bash >> $log_file
echo "INFO: Build online reconstruction routines (now we have daq)" >> $log_file
scons >> $log_file
echo "INFO: Check that online services are running" >> $log_file
python tests/integration/test_distributed_processing/_test_online_okay.py >> $log_file
if [ "$?" != "0" ] 
then
    echo "FATAL: Failed to run online services - check output" >> $log_file
fi

