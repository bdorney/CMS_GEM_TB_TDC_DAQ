#!/bin/bash
g++ setscaler.cc -I../include -I../include/CAEN -o setscaler -Wall -Dlong32="int" -Dlong64="long long" -L../lib -lCAENVME