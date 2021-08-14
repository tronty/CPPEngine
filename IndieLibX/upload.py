#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

import ftplib
import os
import sys
import traceback
import os.path
import re
import string
import struct
import getopt
from xml.etree import ElementTree
from os.path import expanduser
import shutil
import platform

def upload(password, fullname, username, ftphostname, targetdirectory='./'):
	print "Logging in..."
	ftp = ftplib.FTP()
	ftp.connect(ftphostname)
	print ftp.getwelcome()
	try:
    		try:
        		ftp.login(username, password)
        		ftp.cwd(targetdirectory)
        		# move to the desired upload directory
        		print "Currently in:", ftp.pwd()

        		#fullname = '../sourcedirectoy/filename'
        		name = os.path.split(fullname)[1]

    			try:
        			ftp.delete(name)
        			print "FTP file deleted: " + name
        			print "-" * 70
    			except:        
        			print "FTP file not deleted: " + name
			print "Uploading...",
        		
        		f = open(fullname, "rb")
        		ftp.storbinary('STOR ' + name, f)
        		f.close()
        		print "OK"
        
        		print "Files:"
        		print ftp.retrlines('LIST')
    		finally:
        		print "Quitting..."
        		ftp.quit()
	except:
		traceback.print_exc()

def clean(dir):
	os.system("find "+dir+" -name \"*.obj\" -type f -delete")
	os.system("find "+dir+" -name \"*.o\" -type f -delete")
	os.system("find "+dir+" -name \"*.*~\" -type f -delete")
	os.system("find "+dir+" -name \"*~\" -type f -delete")

def delx():
	os.system("rm -f ../IndieLib_resources/*.debug.log")
	os.system("rm -f ../IndieLib_resources/*.log")
	os.system("rm -f ../AllLibs_LNX32.zip")
	os.system("rm -f ../AllLibs_LNX64.zip")
	os.system("rm -f ../AllLibs_OSX64.zip")
	os.system("rm -f ../IndieLib_resources.zip")
	os.system("rm -f ../IndieLibX.zip")

def upload2x(ftphostname, un, directory):
	pw=raw_input('password for \ndomain '+ftphostname+'\nuser '+un+'\n#')
	
	upload(pw, '../AllLibs_LNX32.zip', un, ftphostname, directory)
	upload(pw, '../AllLibs_LNX64.zip', un, ftphostname, directory)
	upload(pw, '../AllLibs_OSX64.zip', un, ftphostname, directory)
	upload(pw, '../IndieLib_resources.zip', un, ftphostname, directory)
	upload(pw, '../IndieLibX.zip', un, ftphostname, directory)
	upload(pw, './testgles2.sh', un, ftphostname, directory)
	upload(pw, './install.sh', un, ftphostname, directory)
	upload(pw, './install.html', un, ftphostname, directory)
	upload(pw, './README.html', un, ftphostname, directory)
	upload(pw, './DirectX.html', un, ftphostname, directory)
	upload(pw, './index.html', un, ftphostname, directory)

def main():

	clean('../IndieLib_resources')
	clean('../IndieLibX')

	delx()

	#os.system('ecw -u '+user+' -p '+raw_input('elfcloud.fi password:')+' -s elfcloud.fi --file ../IndieLib.zip --method replace store  --no-encryption -n IndieLib.zip -id 77777')

	os.system("zip -r ../AllLibs_LNX32.zip ../AllLibs_LNX32")
	os.system("zip -r ../AllLibs_LNX64.zip ../AllLibs_LNX64")
	os.system("zip -r ../AllLibs_OSX64.zip ../AllLibs_OSX64")
	os.system("zip -r ../IndieLib_resources.zip ../IndieLib_resources")
	os.system("zip -r ../IndieLibX.zip ../IndieLibX")
	
	ftphostname='cloud19.hostingpalvelu.fi'
	un='tommiro1'
	upload2x(ftphostname, un, './public_html')

	ftphostname='www.elisanet.fi'
	un='zv3942409'
	upload2x(ftphostname, un, './')

	delx()

main()

x='''
>>> import StringIO

>>> client.store_data(parent_id = 1, key = "dataitem_name", p_data = StringIO.StringIO('Example data'))
>>> dataitem = client.get_dataitem(parent_id=1, key="dataitem_name")
>>> dataitem.raw_meta
'v1:ENC:NONE::'
>>> dataitem.data
{'checksum': 'c13b2bc2027489c3398a3113f47c800a', 'data': <elfcloud.filecrypt.FileIterator object at ...>}

>>> response = client.fetch_data(parent_id = 1, key = "dataitem_name")
>>> print response
{'checksum': 'c13b2bc2027489c3398a3113f47c800a', 'data': <elfcloud.filecrypt.FileIterator object at ...>}
>>> for data in response['data']:
...     print data
Example data
>>> response['data']._md5.hexdigest()
'c13b2bc2027489c3398a3113f47c800a'
'''

