sudo /etc/init.d/lightdm stop
sudo cp /etc/X11/xorg.conf /etc/X11/xorg.conf.backup
sudo dpkg-reconfigure xserver-xorg
sudo /etc/init.d/lightdm start

