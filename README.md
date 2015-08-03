# Dark-Souls-PvP-AI
AI for playing (and winning) in dark souls pvp  
  
People who if you like this you should also follow:  
https://github.com/eur0pa. Creator of dark souls pvp watchdog. Thanks man!  
https://github.com/OrdinaryMagician. Various projects in dark souls 2, and assembly master.   
  
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
  [bug priority] when using combustion, and on recovery subanimation, subanimation randomly goes to windup then back to recoery, triggering another roll.  

  [high priority] neural network training, test implementation in main program.  
  [high priority] sometimes double rolling. issue with subanimation detection?  
  [high priority] If broken out of dodge animation, instead of exiting roll i double roll. fix.   
  [high priority] verify ghost hit works  

  [medium priority] get dynamic angle change while rolling to roll bs.  
  [medium priority]	Stamia management  
  [medium priority, can just not invade for now]	Pointers have to be reread for characters because end address changes when invading  
  [medium priority, use CE for now]	Must lock camera(x and y pos and rotation), rotation x set to PI. or 0. It changed?  

  [low priority, currently sub-millisecond]	ReadPlayer should read memory in chunks, not individual calls to ReadMemory  
  [low priority]	Find range of weapons(hitbox size,bows,etc)  
  [low priority]	Better Vjoy loading/unloading    
  [low priority, use FANN only for now]		Have a build flag which determines if the program uses my handwritten neural netowrk or someone elses library neural network  

Using:   
http://vjoystick.sourceforge.net/ (note: figure out better way than just puting .dll in with .exe)(i dont know if i like the program)  