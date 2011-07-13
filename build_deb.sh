#!/bin/bash
ARCH=`uname -m`
echo "Building PowerDEVS DEB package for $ARCH";
echo "Building Binaries";
make -f Makefile.linux
rm -rf tmp_deb
svn export deb tmp_deb
mkdir ./tmp_deb/opt/powerdevs
svn export bin ./tmp_deb/opt/powerdevs/bin
if [ "$ARCH" == "i686" ]; then
  rm ./tmp_deb/DEBIAN/control.amd64; mv ./tmp_deb/DEBIAN/control.i386 ./tmp_deb/DEBIAN/control
fi
if [ "$ARCH" == "x86_64" ]; then
  rm ./tmp_deb/DEBIAN/control.i386; mv ./tmp_deb/DEBIAN/control.amd64 ./tmp_deb/DEBIAN/control
fi
cp ./bin/pd* ./tmp_deb/opt/powerdevs/bin # copy the binaries
cp ./bin/original.ini ./tmp_deb/opt/powerdevs/bin/powerdevs.ini
cp COPYING ./tmp_deb/opt/powerdevs
cp bin/run.sh ./tmp_deb/opt/powerdevs/bin
svn export build ./tmp_deb/opt/powerdevs/build
svn export engine ./tmp_deb/opt/powerdevs/engine
svn export library ./tmp_deb/opt/powerdevs/library
svn export examples ./tmp_deb/opt/powerdevs/examples
svn export atomics ./tmp_deb/opt/powerdevs/atomics
svn export output ./tmp_deb/opt/powerdevs/output
dpkg -b tmp_deb powerdevs.deb
