#!/bin/bash
#
# Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

OS="Windows"
if [ -f "/etc/lsb-release" ]; then
	DINFO=$(cat /etc/lsb-release | grep DISTRIB_ID | sed s/.*=//)
	if [ $DINFO == "Ubuntu" ]; then
		OS="Ubuntu"
	fi
	if [ $DINFO == "LinuxMint" ]; then
		OS="LinuxMint"
	fi
elif [ -f "/etc/debian_version" ]; then
	OS="Debian"
elif [ -d "/Applications/Xcode.app" ]; then
	OS="Darwin"
elif [ -d "/Applications/Xcode-Beta.app" ]; then
	OS="Darwin"
fi

echo "OS:$OS"

