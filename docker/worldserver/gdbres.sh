#!/bin/bash
while :

do

gdb -x=commands -batch /application/bin/worldserver > temp

mv temp /application/crashes/log$(date +\%Y-\%m-\%d-\%H-\%M-\%S).log

done
