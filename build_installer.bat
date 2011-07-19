@echo off
set QTDIR=C:\Qt\2010.05\qt
set PATH=C:\Qt\2010.05\qt\bin
set PATH=%PATH%;C:\Qt\2010.05\bin;C:\Qt\2010.05\mingw\bin
set PATH=%PATH%;%SystemRoot%\System32
set PATH=%PATH%;C:\Program Files (x86)\Subversion\bin
set PATH=%PATH%;C:\Program Files (x86)\Ethalone\Ghost Installer\Bin\
set QMAKESPEC=win32-g++
echo "Retrieving latest from SVN"
svn up
svnversion >rev
copy version.major+rev version /Y
rmdir /s /q tmp_win_installer 
svn export win_installer tmp_win_installer
svn export atomics tmp_win_installer\powerdevs\atomics
svn export build tmp_win_installer\powerdevs\build 
svn export engine tmp_win_installer\powerdevs\engine 
svn export examples tmp_win_installer\powerdevs\examples
svn export library tmp_win_installer\powerdevs\library
svn export output tmp_win_installer\powerdevs\output
copy version tmp_win_installer\powerdevs
copy COPYING tmp_win_installer\powerdevs
echo "Building Binaries";

cd src/pdae
qmake -r -spec win32-g++ "CONFIG+=release"
mingw32-make clean
mingw32-make 
cd ../pdif
qmake -r -spec win32-g++ "CONFIG+=release"
mingw32-make clean
mingw32-make 
cd ../pdme_v2
qmake -r -spec win32-g++ "CONFIG+=release"
mingw32-make clean
mingw32-make 
cd ../pdppt
qmake -r -spec win32-g++ "CONFIG+=release"
mingw32-make clean
mingw32-make 
cd ../../tmp_win_installer
gibuild PowerDEVS.gpr
cd ../
copy tmp_win_installer\Output\* .
rem rmdir /s /q tmp_win_installer 
