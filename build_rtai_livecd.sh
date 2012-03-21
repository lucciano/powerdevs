#!/bin/bash

# This script is used to create a PowerDEVS-RTAI
# Live CD.
# Author: Federico Bergero (fbergero@fceia.unr.edu.ar)
ARCH=`uname -m`

#Build debian package with rtai support
./build_deb.sh rtai

#Clean directories
rm -rf tmp uck-2.4.5
mkdir tmp
tar xvzf rtai/uck_2.4.5.tar.gz 
cd uck-2.4.5

# Extract iso cd
sudo ./uck-remaster-unpack-iso ../ubuntu-10.04.4-desktop-i386.iso ../tmp

# Unpack squash filesystem
sudo ./uck-remaster-unpack-rootfs ../tmp

# Copy everything we need to install
sudo mv ../powerdevs_i386.deb /tmp
sudo cp ../rtai/install_pd_rtai /tmp
sudo cp ../rtai/rtai-3.8.1.tar.bz2 /tmp
sudo cp ../rtai/.rtai_config /tmp
sudo cp ../rtai/rtai_config.h /tmp
sudo cp ../rtai/linux-*.deb /tmp
sudo cp ../deb/usr/share/applications/powerdevs.desktop ../tmp/remaster-root/etc/skel
sudo chmod +x  ../tmp/remaster-root/etc/skel


#This is to use the local repoistories at CIFASIS
sudo cp ../tmp/remaster-root/etc/apt/sources.list ../tmp
sudo cp /etc/apt/sources.list ../tmp/remaster-root/etc/apt/sources.list 
#Up to here

# Tell ldconfig to look for libraries in /usr/realtime/lib
echo "/usr/realtime/lib" | sudo tee ../tmp/remaster-root/etc/ld.so.conf.d/rtai.conf >/dev/null

# Remove windows files
sudo ./uck-remaster-remove-win32-files ../tmp

# Chroot to the filesystem and run installation script
sudo ./uck-remaster-chroot-rootfs ../tmp "/tmp/install_pd_rtai"


# This is to use the local repoistories at CIFASIS
#sudo mv ../tmp/sources.list ../tmp/remaster-root/etc/apt/sources.list 
#sudo rm -rf tmp
# Up to here

cd uck-2.4.5
#Replace livecd kernel and initrd with RTAI's
sudo cp /boot/initrd.img-2.6.32.11+drm33.2 ../tmp/remaster-iso/casper/initrd.gz
sudo cp /boot/vmlinuz-2.6.32.11+drm33.2 ../tmp/remaster-iso/casper/vmlinuz
sudo sed -i 's/initrd.lz/initrd.gz/g' ../tmp/remaster-iso/isolinux/text.cfg 

# Pack filesystem
sudo ./uck-remaster-pack-rootfs ../tmp

# Create iso
sudo ./uck-remaster-pack-iso "pd_rtai.iso" ../tmp

# Clean dirs
sudo ./uck-remaster-clean ../tmp
#sudo rm -rf tmp uck-2.4.5 

echo "Generation completed SUCCESSFULLY, find your ISO in ../tmp/remaster-new-files"
