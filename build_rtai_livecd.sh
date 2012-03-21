#!/bin/bash
ARCH=`uname -m`
./build_deb.sh rtai
rm -rf tmp uck-2.4.5
mkdir tmp
tar xvzf rtai/uck_2.4.5.tar.gz 
cd uck-2.4.5
sudo ./uck-remaster-unpack-iso ../ubuntu-10.04.4-desktop-i386.iso ../tmp
sudo ./uck-remaster-unpack-rootfs ../tmp
sudo cp ../powerdevs_i386.deb /tmp
sudo cp ../rtai/install_pd_rtai /tmp
sudo cp ../rtai/rtai-3.8.1.tar.bz2 /tmp
sudo cp ../rtai/.rtai_config /tmp
sudo cp ../rtai/rtai_config.h /tmp
sudo cp ../rtai/inux-*.deb /tmp
#This is to use the local repoistories at CIFASIS
sudo cp ../tmp/remaster-root/etc/apt/sources.list ../tmp
sudo cp /etc/apt/sources.list ../tmp/remaster-root/etc/apt/sources.list 
#Up to here
sudo ./uck-remaster-remove-win32-files ../tmp
sudo ./uck-remaster-chroot-rootfs ../tmp "/tmp/install_pd_rtai"
#sudo mv ../tmp/sources.list ../tmp/remaster-root/etc/apt/sources.list 
#sudo rm -rf tmp
cd uck-2.4.5
sudo ./uck-remaster-pack-initrd ../tmp
sudo ./uck-remaster-pack-rootfs ../tmp
sudo ./uck-remaster-pack-iso "pd_rtai.iso" ../tmp
sudo ./uck-remaster-clean ../tmp
echo "Generation completed SUCCESSFULLY, find your ISO in ../tmp/remaster-new-files"
