#!/bin/bash
#
# Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

for file in *.dds
do
    echo "$file"
    convert "$file" "$(basename "$file" .dds).png"
done

