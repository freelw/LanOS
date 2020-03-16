#!/bin/bash

shopt -s expand_aliases
source ./lan_os_rc
cd src &&
echo 'builing lan os...'
lan_make > /dev/null 2>&1 &&
cp a.vfd ../../ &&
echo 'cleaning temp files...'
lan_clean > /dev/null 2>&1
echo "file [a.vfd] is our os image with a simple ramdisk filesystem."