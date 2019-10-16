# honda-canbus-project
A CAN-bus project for the Raspberry Pi using a Honda Civic.

This is both a personal project and also a school project for my C++ class.  It originally started as a simple way to get experience for fun and also to help with a getting a job.

I have a RaspberryPi 3 with a PiCan2 board hooked up to my 2015 Honda Civic Si thru the OBDII port to access the Fast Can. Eventually I'll get the PiCanDuo board to access both the Fast Can and the Body Can, but that's later. I want to access some things like when Vtec is on, RPM is in a certain range, load of the engine is above a certain amount, etc. I want to then run my own program that will actuate an exhaust cutout. So when "vtec just kicked in yo" lol the exhaust will become a straight thru exhaust, but only during high load, high rpm situations. There's a product out there that already does this. I think it's called Vtec Climax but it's several hundred dollars, the company is gone now I think, and I want to do it myself, as I'm sure you all can understand.

With everything I'm learning about the CAN bus and every message I decode, ideas for more projects have come up that I want to add.  I want to have underdash LEDs that light up at certain times, and also want a small screen to display a dash of my own showing certain information with more detail.

Now with all of this, the project has taken on a life of its own as I've learned more about what can be done with the Raspberry Pi (RPi) and the more I learn about the CAN bus.

Most of the code is based on the linux-can-utils software (from Volkswagen Group I think, haha).  I have decoded a fair amount of the CAN packets, but it is difficult.

The original code was in C but part of this project for school is now to convert it to C++ so a lot of the updates will be getting sections of code converted over to C++.

A lot of the updates to the project will also be on a blog: https://www.raspberrypi.org/forums/viewtopic.php?f=41&t=220138