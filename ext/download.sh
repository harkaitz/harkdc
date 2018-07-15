#!/bin/sh
cd "`dirname "$0"`"
DIR="`pwd`"
CDM_DRIVER=$DIR/CDM21228_Setup.exe
echo "CDM Driver: $CDM_DRIVER"
if test ! -f $CDM_DRIVER;then
    wget -O $CDM_DRIVER.zip "http://www.ftdichip.com/Drivers/CDM/CDM21228_Setup.zip"
    7z x $CDM_DRIVER.zip
fi
