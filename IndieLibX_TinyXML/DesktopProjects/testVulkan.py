def supportsVulkan():
	r=0
	try:
		import initVulkan3
		#print initVulkan3.helloworld()
		r=initVulkan3.initVulkan()
		#print 'r=',r
	except:
		return 1
	return r

r=supportsVulkan()
print 'r=',r

