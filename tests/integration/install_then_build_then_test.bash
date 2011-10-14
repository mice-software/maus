#!/bin/bash
./configure
source env.sh
./third_party/build_all.bash 
source env.sh
scons -c
scons build || exit 1
./run_tests.bash || exit 1
# see if libxml2 imports okay
python -m libxml2
if [ $? == 0 ]; then
    echo "LIBXML2 IMPORT SUCCESSFUL"
else
    echo "LIBXML2 IMPORT FAILED"
fi

# just another way of doing it - just to check
echo import libxml2 | python
if [ $? == 0 ]; then
    echo "LIBXML2 IMPORT SUCCESSFUL"
else
    echo "LIBXML2 IMPORT FAILED"
fi

# see if libxslt imports okay
python -m libxslt
if [ $? == 0 ]; then
    echo "LIBXML2 IMPORT SUCCESSFUL"
else
    echo "LIBXML2 IMPORT FAILED"
fi


# just another way of doing it - just to check
echo import libxslt | python
if [ $? == 0 ]; then
    echo "LIBXML2 IMPORT SUCCESSFUL"
else
    echo "LIBXML2 IMPORT FAILED"
fi

