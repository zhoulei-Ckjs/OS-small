#!/bin/bash
#利用下面命令创建磁盘
bximage -q -hd=32 -func=create -sectsize=512 -imgmode=flat hdb.img

#利用下面命令往磁盘写点东西
dd if=./build/boot/mbr.o of=hdb.img bs=512 seek=0 count=1 conv=notrunc