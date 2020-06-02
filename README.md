# Tesla-ChargePort-Decoder

Quick and dirty code to decode Tesla ChargePort messages over SWCAN. 

Software written for and tested on a beaglebone black: 
https://www.beyondlogic.org/adding-can-to-the-beaglebone-black/

and using a NCV7356/TH8056 breakout board: 
https://circuitmaker.com/Projects/Details/Craig-Peacock-4/SWCAN-Breakout

Compile using gcc:
````
$ gcc ChargePortDecoder.c -o ChargePortDecoder
````
Code hardcoded to use CAN0 interface, but can be changed in code.
