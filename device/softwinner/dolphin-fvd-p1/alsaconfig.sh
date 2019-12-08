#!/system/bin/busybox sh

#BUSYBOX="/sbin/busybox"

alsa_amixer -c Device cset iface=MIXER,name='PCM Capture Source' 1 #// set pcm capture to Line
alsa_amixer -c Device cset iface=MIXER,name='Mic Capture Volume' 0
alsa_amixer -c Device cset iface=MIXER,name='Mic Capture Switch' 0
alsa_amixer -c Device cset iface=MIXER,name='Line Capture Switch' 1
alsa_amixer -c Device cset iface=MIXER,name='Line Capture Volume' 4098 #// 0db
alsa_amixer -c Device cset iface=MIXER,name='PCM Capture Switch' 0
alsa_amixer -c Device cset iface=MIXER,name='PCM Capture Volume' 0
alsa_amixer -c Device cset iface=MIXER,name='Mic Playback Switch' 0
alsa_amixer -c Device cset iface=MIXER,name='Mic Playback Volume' 0
alsa_amixer -c Device cset iface=MIXER,name='Line Playback Switch' 0
alsa_amixer -c Device cset iface=MIXER,name='Line Playback Volume' 0
alsa_amixer -c Device set Speaker,0 100% unmute

#alsa_amixer -c Device_1 cset iface=MIXER,name='PCM Capture Source' 1 #// set pcm capture to Line
#alsa_amixer -c Device_1 cset iface=MIXER,name='Mic Capture Volume' 0
#alsa_amixer -c Device_1 cset iface=MIXER,name='Mic Capture Switch' 0
#alsa_amixer -c Device_1 cset iface=MIXER,name='Line Capture Switch' 1
#alsa_amixer -c Device_1 cset iface=MIXER,name='Line Capture Volume' 4098 #// 0db
#alsa_amixer -c Device_1 cset iface=MIXER,name='PCM Capture Switch' 0
#alsa_amixer -c Device_1 cset iface=MIXER,name='PCM Capture Volume' 0
#alsa_amixer -c Device_1 cset iface=MIXER,name='Mic Playback Switch' 0
#alsa_amixer -c Device_1 cset iface=MIXER,name='Mic Playback Volume' 0
#alsa_amixer -c Device_1 cset iface=MIXER,name='Line Playback Switch' 0
#alsa_amixer -c Device_1 cset iface=MIXER,name='Line Playback Volume' 0
#alsa_amixer -c Device_1 set Speaker,0 100% unmute
