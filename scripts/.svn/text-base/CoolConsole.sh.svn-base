#!/bin/bash
# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>

if test -z "$CMTCONFIG" ; then
    echo Setup Athena please. FAIL.
    exit
fi

AtlCoolConsole.py "sqlite://x;schema=tileSqlite.db;dbname=COMP200"
