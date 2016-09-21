#!bin/bash

export TDC_BASE=$PWD
export CAEN_BASE=${TDC_BASE}/software/controller

# Switch Compiler to gcc-4.8
LCG_gcc_home=/opt/gcc-4_8_build/gcc-4_8
LCG_lib_name=lib

export PATH=${LCG_gcc_home}/bin:${PATH}

if [ ${LD_LIBRARY_PATH} ] 
then
export LD_LIBRARY_PATH=${LCG_gcc_home}/${LCG_lib_name}:${LD_LIBRARY_PATH}
else
export LD_LIBRARY_PATH=${LCG_gcc_home}/${LCG_lib_name}
fi

#ln -s ${LCG_gcc_home}/bin/gcc gcc
alias gcc=${LCG_gcc_home}/bin/gcc
#ln -s ${LCG_gcc_home}/bin/g++ g++
alias g++=${LCG_gcc_home}/bin/g++

# Make sure CAEN VME Library is included
#export LD_LIBRARY_PATH=/usr/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${TDC_BASE}/lib:${LD_LIBRARY_PATH}
