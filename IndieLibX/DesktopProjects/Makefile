all:
	sudo python setup.py install
	# --record initVulkan3installfiles.txt

clean:
	sudo rm -fr *.pyc *.o ./build ../build

test:
	python test.py

uninstall:
	#cat initVulkan3installfiles.txt | sudo xargs rm -rf
	sudo rm -fr /usr/local/lib/python2.7/dist-packages/initVulkan3.so
	sudo rm -fr /usr/local/lib/python2.7/dist-packages/initVulkan3-1.0.egg-info

