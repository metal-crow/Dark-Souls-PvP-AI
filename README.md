# Dark-Souls-PvP-AI
AI for playing (and winning) in dark souls pvp  
  
People who if you like this you should also follow:  
https://github.com/infausto. Thanks for watchdog, and the work you have done.  
https://github.com/OrdinaryMagician. You do some fucking SHIT with assembly, man.  
  
-reverse engineer dark souls plantom player pointer and struct  
-code to read darksouls.exe's memory   
-send joystick commands  
-ai logic to play  
  
Setup:  
Vjoy controller device config:  
	x,y,z,rx,ry,10 buttons,1 discrete pov

vJoy button hex mappings(wtf is this done this way)  
square: 0x00000001  
triangle: 0x00000002  
square and triangle: 0x00000003  
cross: 0x00000004  
circle: 0x00000008  
l1: 0x000000010
r1: 0x000000020
  
TODO:  
  pointers have to be reread for characters because end acress change when invaing (low priority, can just not invade for now)
  stamia management(low priority, very complex)
  the subanimation state address is different when two handing!(high priority)  
  must lock camera(x and y pos and rotation), rotation x set to PI. or 0. It changed?(low priority, use CE for now)  
  Find range of weapons(hitbox size,bows,etc)(low priority)  

Using:   
http://vjoystick.sourceforge.net/ (note: figure out better way than just puting .dll in with .exe)(i dont know if i like the program)  

NOTES:  
have a build flag which determines if the program uses my handwritten neural netowrk or someone elses library neural network  
