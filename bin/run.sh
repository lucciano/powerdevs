#!/bin/bash
if [ ! -d $HOME/powerdevs ]
then
	echo "Installing PowerDEVS..."
	cp -a /opt/powerdevs $HOME/powerdevs
	rm $HOME/powerdevs/bin/run.sh
fi
	cd $HOME/powerdevs/bin
	./pdme
