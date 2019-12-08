rpi-si4703
==========

Raspberry Pi port of SparkFun's Si4703 FM tuner chip 
<a href="https://github.com/sparkfun/Si4703_FM_Tuner_Evaluation_Board">Arduino library</a>.

Usage
=====

You have to install <a href="http://wiringpi.com/download-and-install/">wiringPi</a> first. SDA/SDIO and Reset from the breakout board should be wired to GPIO0 and GPIO23 respectively.
<pre>
	<code>
git clone https://github.com/cthoma/rpi-si4703.git
cd rpi-si4703/
gcc -o Radio example/Radio.cpp Si4703_Breakout.cpp -lwiringPi
sudo ./Radio
	</code>
</pre>
(Hint: If you're not from europe, you have to change the si4703_init() function
accordingly)

License Information
===================

Si4703_Breakout.cpp and Si4703_Breakout.h are open source so please feel free to do anything you want with it; 
you buy me a beer if you use this and we meet someday. 
(<a href="http://en.wikipedia.org/wiki/Beerware">Beerware license</a>)


