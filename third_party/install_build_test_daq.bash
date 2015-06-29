#!/bin/bash

log_file="install.log"

echo "INFO: Building third party libraries including DAQ"

if [ ! -f "./install_build_test.bash" ] 
then
    echo "FATAL: Failed to find install_build_test.bash. Check that you are"
    echo "FATAL: running from the maus root directory"
fi
echo "INFO: Found install_build_test.bash - now building"
./install_build_test.bash "$@"

echo "INFO: Now building DAQ extras"
echo "INFO: Now building DAQ extras" 2>>$log_file 1>>$log_file
source env.sh 2>>$log_file 1>>$log_file
echo "INFO: Building DAQ add-on"  2>>$log_file 1>>$log_file
bash third_party/bash/70daq.bash 2>>$log_file 1>>$log_file
#bash third_party/bash/61monitoring.bash 2>>$log_file 1>>$log_file
if [ "$?" != "0" ]
then
    echo "FATAL: DAQ add-ons failed to build okay" 2>>$log_file 1>>$log_file
    cat $log_file
    exit 1;
fi
echo "INFO: Build online reconstruction routines (now we have daq)" 2>>$log_file 1>>$log_file
scons 2>>$log_file 1>>$log_file
if [ "$?" != "0" ]
then
    echo "FATAL: DAQ add-ons failed to build okay" 2>>$log_file 1>>$log_file
    cat $log_file
    exit 1;
fi


echo "INFO: Check that online services are running" 2>>$log_file 1>>$log_file
python tests/integration/test_distributed_processing/_test_online_okay.py 2>>$log_file 1>>$log_file
if [ "$?" != "0" ]
then
    echo "FATAL: Online appeared to install okay but not all services started" 2>>$log_file 1>>$log_file
    echo "FATAL: check rabbitmq and mongod are installed and running" 2>>$log_file 1>>$log_file
    cat $log_file
    exit 1;
fi
echo "INFO: Success! MAUS installed successfully with daq libraries" 2>>$log_file 1>>$log_file
echo "INFO: Success! MAUS installed successfully with daq libraries"
