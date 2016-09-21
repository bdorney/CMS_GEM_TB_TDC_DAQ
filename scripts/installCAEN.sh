#!/bin/sh
#=============================================================================
#
#             --- CAEN SpA - Computing Systems Division ---
#
#  Library installation script.
#
#
#  March  2004 :   Created.
#  Sept   2016 :   Modified. B.Dorney
#
#=============================================================================

install ${CAEN_BASE}/lib/libCAENVME.so.2.11 ${TDC_BASE}/lib
ln -sf ${TDC_BASE}/lib/libCAENVME.so.2.11 ${TDC_BASE}/lib/libCAENVME.so
/sbin/ldconfig
cp ${CAEN_BASE}/include/* ${TDC_BASE}/include/CAEN
