startRun=$1
for i in {0..1}
do
  ./Linux/TestTDC << EOF
    GE11_TDCRun$((startRun+i))_Muon_5k_MSPL4_Async_$2uA_T$3_T$4_2015 
EOF

done
