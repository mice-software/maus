#!/bin/bash
set -e
python ${MAUS_THIRD_PARTY}/third_party/install/bin/nosetests -v ${MAUS_ROOT_DIR}/tests/style
