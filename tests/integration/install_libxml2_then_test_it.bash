#!/bin/bash
./configure
source env.sh
./third_party/bash/01python.bash
./third_party/bash/42libxml2.bash
source env.sh
python -m libxml2
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
    echo "LIBXSLT IMPORT SUCCESSFUL"
else
    echo "LIBXSLT IMPORT FAILED"
fi


# just another way of doing it - just to check
echo import libxslt | python
if [ $? == 0 ]; then
    echo "LIBXSLT IMPORT SUCCESSFUL"
else
    echo "LIBXSLT IMPORT FAILED"
fi

