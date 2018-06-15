#!/bin/bash

echo "Start Install!"

## Compiling program
echo ">>>>>> Compiling program on MASTER"
cd src
make clean
make all

## Creating dataset on pcpc@MASTER
echo ">>>>>> Creating dataset on pcpc@MASTER"
cd ..
./JacobiGen


## Coping all Environment+dataset on SLAVEs ["Network is faster than CPU+MEM?"]
. ip_private_list.array

echo ">>>>>> Coping Environment+Dataset on pcpc@SLAVEs "
for slave in "${ip_private_list[@]}"
do
 	scp -oStrictHostKeyChecking=no -r * pcpc@$slave:
done
