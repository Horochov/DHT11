# DHT11


This lib is dependent of my [elapsedTime](https://github.com/Horochov/avrElapsedTime) lib, which adds a way to measure \<\<you guessed that\>\> elapsed time.

Code will communicate with DHT11 hummidity and temperature sensor via modified 1-wire bus. 

Goal was to create pretty okay code, which doesn't use too much RAM and does not have fully fixed timings. 

When data reception process is started, MCU will init bus, then wait for rising edge - start of data bit. It will wait for a fixed time
and check if pin state has changed. If it's low state - it means '0'. When high - '1' and will wait until falling edge occurs. 
Then the whole cycle repeats.
