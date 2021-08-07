#!/bin/bash
#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
source ./make.cnf
OS="Windows"
if [ -f "/etc/lsb-release" ]; then
	DINFO=$(cat /etc/lsb-release | grep DISTRIB_ID | sed s/.*=//)
	if [ $DINFO == "Ubuntu" ]; then
		OS="Ubuntu"
		PYTHONV="Python $pythonUbuntuversion"
	fi
	if [ $DINFO == "LinuxMint" ]; then
		OS="LinuxMint"
		PYTHONV="Python $pythonUbuntuversion"
	fi
elif [ -f "/etc/debian_version" ]; then
	OS="Debian"
	PYTHONV="Python $pythonDebianversion"
elif [ -d "/Applications/Xcode.app" ]; then
	OS="Darwin"
	PYTHONV="Python $pythonOSXversion"
elif [ -d "/Applications/Xcode-Beta.app" ]; then
	OS="Darwin"
	PYTHONV="Python $pythonOSXversion"
elif [[ "$OSTYPE" == "cygwin" ]]; then
	OS="CYGWIN"
	PYTHONV="Python $pythoncygwinversion"
elif [[ "$OSTYPE" == "msys" ]]; then
	OS="msys"
	PYTHONV="Python $pythonmsysversion"
else
	OS="CYGWIN"
	PYTHONV="Python $pythoncygwinversion"
fi

PYTHON_="$(python -V 2>&1)"

if [[ "$PYTHON_" == "$PYTHONV" ]]; then
	echo "Found the Python: $PYTHONV"
else
	echo "Could not find the Python, install $PYTHONV and add it to the path"
	exit 1
fi

