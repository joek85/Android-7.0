#!/system/bin/sh

let "size = 10"
let "count = 0"
let i=size-1
while [ $i -ge 0 ]
do
    klog="/data/anr/aw_${i}_kernel_boot.log"
    slog="/data/anr/aw_${i}_system_boot.log"
    klogr="/data/anr/aw_`expr ${i} + 1`_kernel_boot.log"
    slogr="/data/anr/aw_`expr ${i} + 1`_system_boot.log"
    if [ -e ${klog} ]
    then
        mv ${klog} ${klogr}
    fi

    if [ -e ${slog} ]
    then
        mv ${slog} ${slogr}
    fi
    let i-=1
done

klog="/data/anr/aw_${size}_kernel_boot.log"
slog="/data/anr/aw_${size}_system_boot.log"
if [ -e ${klog} ]
then
    rm ${klog}
fi
if [ -e ${slog} ]
then
    rm ${slog}
fi

let "count = 0"
cat /proc/kmsg >> /data/anr/aw_${count}_kernel_boot.log &
sleep 4
logcat -v time >> /data/anr/aw_${count}_system_boot.log &
wait

