#! /bin/sh

MOUNTPOINT=/Volumes/SCInstaller
INSTALLER="/Volumes/SCInstaller/SqueezeCenter Installer.app"

if [ -e $1 ] ; then
	hdiutil unmount $MOUNTPOINT
	hdiutil mount $1 -mountpoint $MOUNTPOINT
fi

if [ -e "$INSTALLER" ] ; then
	open "$INSTALLER"
fi
	
