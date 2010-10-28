#!/bin/bash
./configure
source env.sh
find ${MAUS_ROOT_DIR}/third_party/bash/2* -type f |grep -v REDUNDANT |sort| xargs -i bash {}
scons all