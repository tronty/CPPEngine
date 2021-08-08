import re, urllib2

url='http://downloads.gamedev.net/pdf/gpbb/'

def download_file(download_url, afile):
    print download_url+'/'+afile
    response = urllib2.urlopen(download_url+'/'+afile)
    file = open(afile, 'wb')
    file.write(response.read())
    file.close()

def main():
	pdfs=['gpbb.pdf', 'gpbbaftr.pdf', 'gpbbindx.pdf', 'gpbbintr.pdf', 'gpbbpt1.pdf', 'gpbbpt2.pdf']
	#for i in pdfs:
	#	download_file(url, i)
	for i in range(70,71):
		dl='gpbb%d.pdf' % i
		download_file(url, dl)

if __name__ == "__main__":
    main()

