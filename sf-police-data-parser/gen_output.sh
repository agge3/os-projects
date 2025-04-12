#!/usr/bin/env bash

script -c '
make run RUNOPTIONS="Law5K.dat header.txt 1 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 1 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 1 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 1 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""

make run RUNOPTIONS="Law5K.dat header.txt 2 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 2 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 2 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 2 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""

make run RUNOPTIONS="Law5K.dat header.txt 4 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 4 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 4 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 4 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""

make run RUNOPTIONS="Law5K.dat header.txt 8 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 8 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 8 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
make run RUNOPTIONS="Law5K.dat header.txt 8 analysis_neighborhood \"Tenderloin\" \"Nob Hill\""
' out
