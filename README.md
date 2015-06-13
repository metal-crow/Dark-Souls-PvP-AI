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
  the subanimation state address is different when two handing!  

  must lock camera(x and y pos and rotation), rotation x set to PI. or 0. It changed?  
  another animation id is the second swing  

  Find SubAnimation (when hitbox is created). Found one that works for r1's, but not r2's.  
  Find location of where exactly weapon is.      
  Find range of weapons(hitbox size,bows,etc)  
  Method for determining if animation id is attack animation or different  

TODO2:
  Find adverse affects not blocked by watchdog
  Find invisibility that works with watchdog

Using:   
http://vjoystick.sourceforge.net/ (note: figure out better way than just puting .dll in with .exe)(i dont know if i like the program)  

NOTES:  
weight attached to windup is float from 1-0(weight is transition from 1 animation to another)(this is subanimation state)  
windup is a move from id 0 to the attack id. when 0 reaches weight 0 or attackid reaches weight 1, the attack is now fully active  
  
speed  
time is arbitrary time animation runs. counts up. determines now long attack is, must be some array somewhere.  
over is number of times animation repeated  
  