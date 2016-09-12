#!/bin/sh
echo "" > sample_output
echo "" > demo_output
inlist=`ls tests/in*`
outlist=`echo $inlist | sed 's/in/out/g'`
for x in $inlist 
do
	echo "============================" >> demo_output
	./demo $x >> demo_output
done

for y in $outlist
do
	echo "============================" >> sample_output
	cat $y >> sample_output
done
