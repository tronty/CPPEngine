import re, urllib

start='https://developer.apple.com/library/ios'
start='http://developer.download.nvidia.com/SDK/10.5/direct3d/samples.html'

myurl = start#+'index.html'

f=open('../screenshots/DirectX.html','a')

visited=[]
def find(url):

	if url.endswith('.zip') or url.endswith('.ZIP'):
		s=url.rfind('/')
		zipfile=url[s+1:len(url)]
		#print url
		f.write('<DT><a target="_blank" href="http://developer.download.nvidia.com/SDK/10.5/Samples/'+zipfile+'">http://developer.download.nvidia.com/SDK/10.5/Samples/'+zipfile+'</a></DT>\n')
	if not url.startswith(start):
		return

	if url in visited:
		return
	else:
		visited.append(url)
	#if url.endswith('.zip') or url.endswith('.ZIP'):


	if not (url.endswith('.html') or url.endswith('.HTML') or url.endswith('.htm') or url.endswith('.HTM')):
		return

	#print url




	for ii in re.findall('''href=["'](.[^"']+)["']''', urllib.urlopen(url).read(), re.I):
		find(ii)

find(myurl)
f.write('</DL><p></DL><p></BODY></HTML>\n')
f.close()


