#!/usr/bin/env bash
if [ -n "${MAUS_ROOT_DIR+x}" ]; then
    find ${MAUS_ROOT_DIR}/third_party/bash/0* -type f -executable |grep -v REDUNDANT |sort| xargs -i bash {}
    find ${MAUS_ROOT_DIR}/third_party/bash/1* -type f -executable |grep -v REDUNDANT |sort| xargs -i bash {}
else
echo
echo "FATAL: MAUS_ROOT_DIR is not set, which is required to" >&2
echo "FATAL: know where to install this package.  You have two" >&2
echo "FATAL: options:" >&2
echo "FATAL:" >&2
echo "FATAL: 1. Set the MAUS_ROOT_DIR from the command line by" >&2
echo "FATAL: (if XXX is the directory where MAUS is installed):" >&2
echo "FATAL:" >&2
echo "FATAL:        MAUS_ROOT_DIR=XXX ${0}" >&2
echo "FATAL:" >&2
echo "FATAL: 2. Run the './configure' script in the MAUS ROOT" >&2
echo "FATAL: directory, run 'source env.sh' then rerun this" >&2
echo "FATAL: command ">&2
exit 1
fi

