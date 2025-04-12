#!/usr/bin/env bash

script -c '
make run RUNOPTIONS="Law500K.dat header.txt 4 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
' final_out
