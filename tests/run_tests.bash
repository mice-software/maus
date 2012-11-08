#!/bin/bash

bash ${MAUS_ROOT_DIR}/tests/unit_tests.bash
if [ $? != 0 ]
then
    exit 1
fi

bash ${MAUS_ROOT_DIR}/tests/style_tests.bash
if [ $? != 0 ]
then
    exit 1
fi

bash ${MAUS_ROOT_DIR}/tests/application_tests.bash
if [ $? != 0 ]
then
    exit 1
fi

