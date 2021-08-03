import os
try:
	import initVulkan3
	print('initVulkan3 already installed')
except:
	print('installing initVulkan3')
	print('python setup.py install')
	os.system('python setup.py install') # --record initVulkan3installfiles.txt')


