#!/bin/bash
ARCH=`uname -m`
echo "Retrieving latest from SVN";
svn up
svnversion >rev
cat version.major rev > version
VER=`cat version`
echo "Building PowerDEVS DEB package for $ARCH";
echo "Building Binaries";
make -f Makefile clean
if [ $# -eq 1 ]; then
  make -f Makefile MODE=release RTAI=yes -j 8
else
  make -f Makefile MODE=release -j 8
fi
rm -rf tmp_deb
svn export deb tmp_deb
mkdir ./tmp_deb/opt/powerdevs
svn export bin ./tmp_deb/opt/powerdevs/bin
if [ "$ARCH" == "i686" ]; then
  cat ./tmp_deb/DEBIAN/control.i386 | awk -v VERSION="$VER" '{ if(index($0,"Version: ")>=1) print "Version: " VERSION ; else print $0;}' >  ./tmp_deb/DEBIAN/control
  rm ./tmp_deb/DEBIAN/control.amd64; 
  rm ./tmp_deb/DEBIAN/control.i386; 
fi
if [ "$ARCH" == "x86_64" ]; then
  cat ./tmp_deb/DEBIAN/control.amd64 | awk -v VERSION="$VER" '{ if(index($0,"Version: ")>=1) print "Version: " VERSION ; else print $0;}' >  ./tmp_deb/DEBIAN/control
  rm ./tmp_deb/DEBIAN/control.amd64; 
  rm ./tmp_deb/DEBIAN/control.i386; 
fi
cp ./bin/pd* ./tmp_deb/opt/powerdevs/bin # copy the binaries
cp ./bin/original.ini ./tmp_deb/opt/powerdevs/bin/powerdevs.ini
cp COPYING ./tmp_deb/opt/powerdevs
cp version ./tmp_deb/opt/powerdevs
cp bin/run.sh ./tmp_deb/opt/powerdevs/bin
cp -a doc/ ./tmp_deb/opt/powerdevs/
svn export build ./tmp_deb/opt/powerdevs/build
svn export engine ./tmp_deb/opt/powerdevs/engine
if [ $# -eq 1 ]; then
  cat engine/Makefile.include \
   | awk '{if (match($0,'/^CXX/')) print $0, " -DRTAIOS"; else print $0}' \
   | awk '{if (match($0,'/^LIBS/')) print $0, " -L/usr/realtime/lib -lpthread -llxrt -lm "; else print $0}' \
   | awk '{if (match($0,'/^INCLUDE/')) print $0, "-I/usr/realtime/include"; else print $0}' > tmp_deb/opt/powerdevs/engine/Makefile.include 
fi
svn export library ./tmp_deb/opt/powerdevs/library
svn export examples ./tmp_deb/opt/powerdevs/examples
svn export atomics ./tmp_deb/opt/powerdevs/atomics
#Extra binaries
cp src/atomics/lcd/lcd ./tmp_deb/opt/powerdevs/atomics/sinks/lcd
cp src/atomics/knob/slider ./tmp_deb/opt/powerdevs/atomics/sources/slider
svn export output ./tmp_deb/opt/powerdevs/output
dpkg -b tmp_deb powerdevs.deb
rm rev
rm version
if [ "$ARCH" == "i686" ]; then
  mv powerdevs.deb powerdevs_i386.deb
fi
if [ "$ARCH" == "x86_64" ]; then
  mv powerdevs.deb powerdevs_amd64.deb
fi
rm -rf tmp_deb
