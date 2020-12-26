#
# Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import os 
import os.path
import re
import string
import struct
import getopt
import sys
from xml.etree import ElementTree
from os.path import expanduser
import shutil
import platform

def raw_string(s):
    if isinstance(s, str):
        s = s.encode('string-escape')
    elif isinstance(s, unicode):
        s = s.encode('unicode-escape')
    return s

def query_yes_no(question, default="no"):
    """Ask a yes/no question via raw_input() and return their answer.

    "question" is a string that is presented to the user.
    "default" is the presumed answer if the user just hits <Enter>.
        It must be "yes" (the default), "no" or None (meaning
        an answer is required of the user).

    The "answer" return value is True for "yes" or False for "no".
    """
    valid = {"yes": True, "y": True, "ye": True,
             "no": False, "n": False}
    if default is None:
        prompt = " [y/n] "
    elif default == "yes":
        prompt = " [Y/n] "
    elif default == "no":
        prompt = " [y/N] "
    else:
        raise ValueError("invalid default answer: '%s'" % default)

    while True:
        sys.stdout.write(question + prompt)
        choice = raw_input().lower()
        if default is not None and choice == '':
            return valid[default]
        elif choice in valid:
            return valid[choice]
        else:
            sys.stdout.write("Please respond with 'yes' or 'no' "
                             "(or 'y' or 'n').\n")

drive,tail=os.path.splitdrive(os.path.expanduser("~"))
test_OS=''
def getRealOS():
	OS='Linux'
	d=platform.dist()
	if d[0]=='Ubuntu':
		OS='Ubuntu'
	elif sys.platform=='darwin':
		OS='Darwin'
	elif os.path.exists('/etc/debian_version'):
		OS='Debian'
	elif sys.platform=='cygwin':
		OS='CYGWIN'
		#if defined(__CYGWIN__) && !defined(_WIN32) && !defined(_WIN64)
	#elif os.name=='posix':
	#	OS='Linux'
	elif os.name=='nt':
		OS='Windows'
	#print 'OS='+OS
	#exit(0)
	return OS

def getOS(test=''):
	if not test_OS=='':
		return test_OS
	OS=getRealOS()
	#print 'OS='+OS
	#exit(0)
	return OS

def log_manager(question, default="no"): 
    return query_yes_no(question, default="no") 

def choose_SDK():
	pre='/windows'
	if getOS()=='Windows':
		pre=r''+drive+''

	sdk='/Program Files (x86)/Windows Kits/10/Include/'
	sdk10=os.path.exists(raw_string(pre+sdk))
	sdk='/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include/'
	sdk2010=os.path.exists(raw_string(pre+sdk))
		
	v=[]
	if (sdk2010):
		r=log_manager('Build with Microsoft DirectX SDK (June 2010)','no')
		if (r):
			v.append('Microsoft DirectX SDK (June 2010)')

	if (sdk10):
		r=log_manager('Build with Windows Kit 10','no')
		if (r):
			v.append('Windows Kit 10')
	return v

v=choose_SDK()
print v

txt='''
C:\Program Files (x86)\Windows Kits\10\Include\10.0.14393.0\um
C:\Program Files (x86)\Windows Kits\10\Include\10.0.14393.0\shared
C:\Program Files (x86)\Windows Kits\10\Lib\10.0.14393.0\um\x86
C:\Program Files (x86)\Windows Kits\10\Lib\10.0.14393.0\um\x64

C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include
C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86
C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64

C:\Program Files\Microsoft Platform SDK\Include
C:\Program Files\Microsoft Platform SDK\Lib
'''
