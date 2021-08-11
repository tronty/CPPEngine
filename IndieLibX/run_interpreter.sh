#!/bin/bash
#
# Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

cd ../IndieLib_resources/masters-grammar-interpreter

inputs=("squid_ship" "borg_cube" "testing" "enterprise")

inputs2=("space_station" "klingon" "spike_ship" "wing_ship" "c_station" "rotational_ship" "tie_fighter")

for input in "${inputs[@]}"
do
	echo "${input}"
	python interpreter.py -i input -m 5 -p -x ${input} $@ > ${input}.txt
done

for input in "${inputs2[@]}"
do
	echo "${input}"
	python interpreter.py -i input -m 5 -p -x ${input} $@ > ${input}.txt
done

cd ../../IndieLibX

