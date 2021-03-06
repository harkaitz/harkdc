#!/bin/sh
# ************************************************************************
# * Licensed `GPLv2` for open source use or HARKD's `Commercial License`
# * for commercial use, visit http://sugaar-solutions.com for more information.
# *
# * Copyright 2018 Sugaar Solutions, all rights reserved
# *
# * Authors: Harkaitz Agirre Ezama
# ***********************************************************************
 
dir="`dirname "$0"`"
echo "[+] Searching new device definitions ..."
echo -n "" > $dir/devices.lst
for f in $dir/*.c;do
    sed -n 's|^const harkd_def_t \([a-zA-Z0-9_][a-zA-Z0-9_]*\) = .*|\1|pg' $f \
	>> $dir/devices.lst
done
cat $dir/devices.lst
echo "[+] Generating $dir/devices.h ..."
echo "/* Generated by \`devices.h\`... */"                 > $dir/devices.h
for n in `cat $dir/devices.lst`;do
    echo "extern const harkd_def_t $n;" >> $dir/devices.h
done
echo "const harkd_def_t *harkd_device_list[] = {"         >> $dir/devices.h
for n in `cat $dir/devices.lst`;do echo "    &$n,"        >> $dir/devices.h; done
echo "    NULL"                                           >> $dir/devices.h
echo "};"                                                 >> $dir/devices.h
