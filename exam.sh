#!/bin/bash
mkdir mydir
chmod 777 mydir
echo 2023 > myfile
mv moveme mydir
cp copyme mydir
cat readme
gcc bad.c 2> err.txt
n=10
if [ $# -eq 1 ];
then
	n=$1
fi
i=1
while [ $i -le $n ];
do
	touch $i.txt
	i=$(( $i + 1 ))
done
