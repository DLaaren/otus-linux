#!/bin/bash
rmmod ex_list 2>/dev/null
dmesg -C
insmod build/ex_list.ko
rmmod ex_list
dmesg | grep "ex_list:" | tail -30
