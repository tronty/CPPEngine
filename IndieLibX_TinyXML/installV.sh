#!/bin/bash
#
# Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

if [[ "True" == "True" ]]; then
	GRAPHICSCARDVENDOR="lshw -C video | awk -F\'product: \' \'/product/{print $2}\'"
	echo "$GRAPHICSCARDVENDOR"
	if [[ "NVIDIA Corporation" == "$GRAPHICSCARDVENDOR" ]]; then
		echo "installNVIDIAdriver	# If you have an NVIDIA graphics card attached to the computer"
	fi
	if [[ "AMD" == "$GRAPHICSCARDVENDOR" ]]; then
		echo "installAMDdriver	# If you have an AMD graphics card attached to the computer"
	fi
fi
