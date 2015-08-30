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
square: 0x1  
triangle: 0x2  
square and triangle: 0x3  
cross: 0x4  
circle: 0x8  
l1: 0x10
r1: 0x20
r2: 0x80
  

attack when

you can interrupt them and poise break
(need to teach poise)

when they're in their recovery animation
(what about weapon speeds? can they recover faster then i can attack?)
good spacing
(need to teach spacing)

Neural Networks:  
Defense Network Inputs:  
  -Player Distance from Enemy  
  -The angle the enemy is off from directly in front of the player  
  -The enemy velocity  
  -The rotation difference between the enemy and the player  

//TODO what inputs should i give to teach attack neural net?  
Attack Network Inputs:  
  -Player distance from Enemy(also send as non-ranged)  
  -Player stamina (not used as input, just post check)(enemy stamina not sent over wire)  


TODO:  
  bug with lock on[X]
  need to break out of counterstrafe if attacked[X] 
  r2 seem not to trigger dodge
  attacking causes wierd rotating in place, dodging not working?

  [high priority] make this strafe in the same direction as the enemy strafe  
  [high priority] try to bs with neural network on and using strafing, train with that  
  [] Test ai with dummy attack(random)
  [high priority] teach spacing
  [high priority] teach poise

  [bug priority: LOW] when attacking, and on recovery subanimation, subanimation randomly goes to windup then back to recovery, triggering another roll.  

  [high priority] neural network training, test implementation in main program.  
  [high priority] is neural network fast enough i dont have to thread it?
  [high priority] verify ghost hit works  

  [medium priority] get dynamic angle change while rolling to roll bs.  
  [medium priority, can just not invade for now]	Pointers have to be reread for characters because end address changes when invading  
  [medium priority, use CE for now]	Must lock camera(x and y pos and rotation), rotation x set to PI. or 0. It changed?  

  [low priority, currently sub-millisecond]	ReadPlayer should read memory in chunks, not individual calls to ReadMemory  
  [low priority]	Find range of weapons(hitbox size,bows,etc)  
  [low priority]	Stamia management(for dodge, attack handled)  
  [low priority]	Better Vjoy loading/unloading    
  [low priority, use FANN only for now]		Have a build flag which determines if the program uses my handwritten neural netowrk or someone elses library neural network  

  [cannot fix] lag will sometimes made perfect dodges impossible b/c hurtbox will appear before the windup animation finishes  
  [cannot fix] If broken out of dodge animation, instead of exiting roll i double roll due to queued animation system.   

Using:   
http://vjoystick.sourceforge.net/  