startRun=$1
for i in {0..7}
do
  ./Linux/TestTDC << EOF
    GE11_TDCRun$((startRun+i))_Pion_5k_MSPL4_Async_$2uA_T$3_T$4_2015 
EOF

done
