#!/bin/bash

latencyVal=(1 10 100 1000 10000)

for lVal in "${latencyVal[@]}"
do 
    echo $lVal
    ./waf --run 'proj1 -latency='"$lVal"'' >>./lab1Res/latencyRes.txt
done

