# Tesla-ChargePort-Decoder

Quick and dirty code to decode Tesla ChargePort messages over SWCAN. 

Software written for and tested on a beaglebone black: 
https://www.beyondlogic.org/adding-can-to-the-beaglebone-black/

and using a NCV7356/TH8056 breakout board: 
https://www.beyondlogic.org/swcan-single-wire-can-transceiver-breakout-board/

Compile using gcc:
````
$ gcc ChargePortDecoder.c -o ChargePortDecoder
````
Code hardcoded to use CAN0 interface, but can be changed in code. SWCAN link runs at 33,300bps. Setup link using: 
````
sudo /sbin/ip link set can0 up type can bitrate 33300
````
