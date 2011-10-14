#!/bin/bash
./configure
source env.sh
#./third_party/bash/01python.bash
#./third_party/bash/42libxml2.bash
source env.sh
python -m libxml2
if [ $? == 0 ]; then
    echo "LIBXML2 IMPORT SUCCESSFUL"
else
    echo "LIBXML2 IMPORT FAILED"
fi

