# Dark-Souls-PvP-AI
AI for playing (and winning) in dark souls pvp  
  
People who if you like this you should also follow:  
https://github.com/eur0pa. Creator of dark souls pvp watchdog. Thanks man!  
https://github.com/OrdinaryMagician. Various projects in dark souls 2, and assembly master.   
https://www.youtube.com/user/Birdulon. Master reverse engineerer, seriously he found the Dark Souls 2 bullet params a week after it came out.  
  
-reverse engineer dark souls plantom player pointer and struct  
-code to read darksouls.exe's memory   
-send joystick commands  
-ai logic to play  
  
Setup:  
Vjoy controller device config:  
	x,y,z,rx,ry,15 buttons,1 discrete pov  

## Neural Networks:  
Defense Network Inputs:  
  -Player Distance from Enemy  
  -The angle the enemy is off from directly in front of the player  
  -The enemy velocity  
  -The rotation difference between the enemy and the player  

//TODO what inputs should i give to teach attack neural net?  
Attack Network Inputs:  
  -Player distance from Enemy(also send as non-ranged)  
  -Player stamina (not used as input, just post check)(enemy stamina not sent over wire)  


## TODO: 
  should check enemy weapon speed, take into acound when deterrmining if roll or reverse roll bs.

  read enemy animaiton type id when in backstab
  [bug] kicking instead of attacking sometimess  

  [TOP priority] should attack even when enemy attacking, just determine their direciton and attack to their side or behind.  
  see aimethods:25. see enemy windup, determine what side/direction it is coming from. immediatly start executing subroutine strafe arround to back in opposing direction. 
  This is attack desicion. WHen we reach back, the auto-bs detection should kick in.
  [TOP] too prone to trading  

  [high] more specifications on neural network output. Defense net should return how exactly to avoid bs, not just detect it. Likewise for attack.  
  [high] use more than 1 attack types, dynamic range for weapon attacks  
  [high] hurtbox for enemy not working on jumping attacks
  [high] toggle escaping(cant dodge everything always, lag)
  [high] get dynamic weapon range working, teach range. Should not keep moving forward once in weapon attack range.  
  [high priority] make this strafe in the same direction as the enemy strafe  
  [high priority] try to bs with neural network on and using strafing, train with that   
  [high priority] teach poise  

  [high priority] fix ghost hit  

  [medium priority] get dynamic angle change while rolling to roll bs.  
  [medium priority, can just not invade for now]	Pointers have to be reread for characters because end address changes when invading  
  [medium priority, use CE for now]	Must lock camera(x and y pos and rotation), rotation x set to PI or 0 depending on map.
  [medium priority] Store all info to be printed in buffer and only print/send to gui on tick end, to save socket writes

  [low priority, currently nothing important can be used with]	Some cacheing layer. The first time ex:distance is computed in a tick, it is stored in a cache struct, and all subsequent distance calls use the already computed value. Easier then passing it around.  
  [low priority, currently sub-millisecond]	ReadPlayer should read memory in chunks, not individual calls to ReadMemory  
  [low priority]	Find range of weapons(hitbox size,bows,etc)  
  [low priority]	Stamia management(for dodge, attack handled)  
  [low priority]	Better Vjoy loading/unloading    
  [low priority, use FANN only for now]		Have a build flag which determines if the program uses my handwritten neural netowrk or someone elses library neural network  
  [low] fix camera to keep it locked for entire fight. Do NOT alter when locked on

  [cannot fix] lag will sometimes made perfect dodges impossible b/c hurtbox will appear before the windup animation finishes  
  [cannot fix] If broken out of dodge animation, instead of exiting roll i double roll due to queued animation system.   
  
## Using:   
http://vjoystick.sourceforge.net/ (VJOY)  
http://leenissen.dk/ (FANN)  
