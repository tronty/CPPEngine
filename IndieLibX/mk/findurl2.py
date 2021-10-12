import re, urllib

start1='https://www.shadertoy.com/results?query=&sort=hot&from=0&num=12'
start2='https://glslsandbox.com/?page=1'

myurl1 = start1
myurl2 = start2

f1=open('../../www.shadertoy.com.html','a')
f2=open('../../glslsandbox.com.html','a')
#<a href='/e#75799.0'><img src='

visited=[]
def find(f, url, start):
	if not url.startswith(start):
		return

	if url in visited:
		return
	else:
		visited.append(url)
	#if url.endswith('.zip') or url.endswith('.ZIP'):
	#print url

	txt=urllib.urlopen(url).read()
	print txt
	for ii in re.findall('''href=["'](.[^"']+)["']''', txt, re.I):
		#print ii
		f.write(ii+'\n')

def find2(f, url, start):
	if not url.startswith(start):
		return

	if url in visited:
		return
	else:
		visited.append(url)
	#if url.endswith('.zip') or url.endswith('.ZIP'):
	#print url

	txt=urllib.urlopen(url).read()
	print txt
	# <a href='/e#75755.0'>
	for ii in re.findall('''\<a href\=\'/(.*?)\'\>''', txt, re.I):
		if ii[0]=='?':
			continue
		print ii
		f.write(ii+'\n')

	for ii in re.findall('''\<img src\=\'/(.*?)\'\>''', txt, re.I):
		if ii[0]=='?':
			continue
		print ii
		f.write(ii+'\n')
		

#find(f1, myurl1, 'https://www.shadertoy.com/')
find2(f2, myurl2, 'https://glslsandbox.com/')
f1.close()
f2.close()

