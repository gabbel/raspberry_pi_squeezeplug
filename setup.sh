#!/bin/sh
#Setup all
#This file helps to install dependencies and compile the sourcecode for serval programms Needed by my Squeezebox radio.
#It will also copy files to serval locations to autostart programms.
#Please run me as root..
#Warning! these are still just notes..


### INSTALL JiveLite ###

#Install dependencies for compiling jivelite
sudo apt-get update; sudo apt-get -y install libsdl1.2-dev libsdl-image1.2-dev libsdl-ttf2.0-dev libsdl-gfx1.2-dev libexpat1-dev

#Jivelite depends on luajit, clone, compile and install this first...
# Luajit from: http://luajit.org/download.html
#clone
git clone http://luajit.org/git/luajit-2.0.git

#compile Luajit
cd luajit*
# this step will require approximately 15min
make
make install
cd ..

# Jivelite from http://www.communitysqueeze.org/
#clone
git clone https://code.google.com/p/jivelite/

#compile jivelite
cd jivelite
make
cd ..


### INSTALL gpio_poller ###

#Install dependencies for compiling "gpio_poller"
sudo apt-get update; sudo apt-get -y install libxtst-dev autotools-dev autoconf automake

#install BCM2835 SOC Support
wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.36.tar.gz 
tar xvfz bcm2835-1.36.tar.gz
cd bcm2835*
./configure
make
make install

cd ..
cd gpio*
./autobuild.sh
cd ..

### Cleanup ###
rm bcm2835-1.36.tar.gz
rm -rf ./bcm2835*
rm -rf ./luajit*

### Copy data to home of User "pi" ###
mkdir /home/pi/raspberry_pi_squeezeplug
cp ./gpio_poller/io_poller /home/pi/raspberry_pi_squeezeplug
chmod a+x /home/pi/raspberry_pi_squeezeplug/io_poller
cp -r ./jivelite /home/pi/raspberry_pi_squeezeplug

### Copy files to autostart openbox window manager, jivelite and to remove Decorations from Openbox ###
#Autostart all
cp ./conf_openbox/autostart /home/pi/.config/openbox/
#Remove Window decorations
cp ./conf_openbox/rc.xml /home/pi/.config/openbox/

### Copy jivlite custom files (Add screen resolutions for "HDSkin")
cp -r ./conf_jivelite /jivelite/share/jive/applets/HDSkin

### Create new boot configuration with new screen resolution
cp ./conf_system/config.txt /boot/config.txt
### Done ##

