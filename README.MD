# Hello my dudes here is the thing I made to control PWM on the pi pico
the thing is very cool and neat.
I will add more here in a minute after I do some more updating and implement my new command format
basically you get 4 bytes over i2c to control the shit u wanna control and then its real cool and 
good. Working on a python library to control it as well using :sparkles:SMBus2:sparkles:

Expect some more explanation of the command structure but basically you get 127 commands to assign as you please, and then you have the following 21 bits to send ur message. Any command goes at the beginning of the 4 byte message and begins with a zero, such that shit lines up all on it's own and there's no prolem. This means that one bit out of every other byte is also a one so it does not get mistaken for a zero which would be awful and break everything so were not doing that. 


And if any experienced github user comes here to laugh at me uploading my build directory fuck off it makes me happy this way. and also like I cant be bothered to remove it so it's just here now get over it my dude. Too late. I really should have followed a proper git tutorial but instead i am winging it so goddamn hard.