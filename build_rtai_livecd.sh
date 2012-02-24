#!/bin/bash
ARCH=`uname -m`
#./build_deb.sh
mkdir tmp
tar xvzf uck_2.4.5.tar.gz 
cd uck-2.4.5
#sudo ./uck-remaster-unpack-iso ../ubuntu-10.04.4-desktop-i386.iso ../tmp
#sudo ./uck-remaster-unpack-rootfs ../tmp
sudo cp ../powerdevs_i386.deb ../tmp/remaster-root/tmp/
sudo cp ../install_pd_rtai ../tmp/remaster-root/tmp/
sudo cp ../rtai-3.8.1.tar.bz2 ../tmp/remaster-root/tmp
#This is to use the local repoistories at CIFASIS
sudo cp ../tmp/remaster-root/etc/apt/sources.list ../tmp
sudo cp /etc/apt/sources.list ../tmp/remaster-root/etc/apt/sources.list 
#Up to here
sudo ./uck-remaster-remove-win32-files ../tmp
sudo ./uck-remaster-chroot-rootfs ../tmp "/tmp/install_pd_rtai"
#sudo apt-get update
sudo mv ../tmp/sources.list ../tmp/remaster-root/etc/apt/sources.list 
#sudo rm -rf tmp
