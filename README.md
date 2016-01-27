# Project King, The Dark Souls PvP AI
AI for playing (and winning) in dark souls pvp  
  
People who if you like this you should also follow:  
https://github.com/eur0pa. Creator of dark souls pvp watchdog. Thanks man!  
https://github.com/OrdinaryMagician. Various projects in dark souls 2, and assembly master.   
https://www.youtube.com/user/Birdulon. Master reverse engineerer, seriously he found the Dark Souls 2 bullet params a week after it came out.  
  
The most recent commit to master should always have been tested and verified to perform well. Any work that may be breaking or may not function well should be done in a branch.  
  
##Setup
* Install [Vjoy](http://vjoystick.sourceforge.net/site/index.php/download-a-install).
* Create Vjoy controller with the config: x,y,z,rx,ry,20 buttons,1 discrete pov
* Install [Cheat Engine](http://www.cheatengine.org/)
* Download [Technojacker's DARKSOULS.CT table](https://drive.google.com/folderview?id=0B_f11g1DlLhDV1RfV0VSdnBfOVE&usp=sharing)  
 * Cheat Engine and the table are only used to lock the camera's x rotation to 3.141592. If you dont want to use them, figure out how to do that yourself.
 * Ill soon have a better table up where locking the camera doesnt crash Dark Souls. Just use a script that AOB scans `F3 0F 11 83 44 01 00 00 80 BB 63 02 00 00 00`, noops, then lock camera x normally.
* Download the source code and compile with [FANN](http://leenissen.dk/). Add the FANN dlls and the vJoyInterface dll to the exe's folder.
* Train the defense neural network (see the methods in InitalizeFANN, use the main method in there with them) and put the .net file in a folder in the .exe's location called "Neural Nets"
 * Or use the already trained nets in the latest release
* (If you want the gui) Import the folder for the GUI into eclipse and run that. Or run the included .jar file.
* Run the PvP AI whenever the opponent is ready.

## Neural Networks
Defense Network Inputs:  

  * Player Distance from Enemy  
  * The angle the enemy is off from directly in front of the player  
  * The enemy velocity  
  * The rotation difference between the enemy and the player  

Attack Network Inputs:  

  * Array of distance between AI and enemy over 5 second period
  * Estimated stamina of enemy
  * TODO estimated poise of enemy
  * TODO poise damage of AI's attack
  * NOTES:
    * usually tell how they attack by how long it's been since they last attacked

## TODO
resolution at 1440x810

  * **TOP Priority**
    * walking off edge on respawn on win.
    * If behind enemy, dont have to dodge, if their attack wont hit me.
    * spear game too strong. Dynamic range fix?
    * any attack can be parried. allow ai to parry sometimes.
    * should attack even when enemy attacking, just determine their direction and attack to their side or behind.
    * impove bs neural network accuracy. try to bs with neural network on and using strafing, train with that
    * CE camera lock script causes crashes. Fix under testing.

  * **HIGH Priority**
    * Not dodging avelin
    * If taking heavy damage, change to more dodging/careful tactics.
    * When poise low, change to dodging until it regens
    * refine behind enemy safe state to be more behind enemy. Cone like?
    * more specifications on neural network output. Defense net should return how exactly to avoid bs, not just detect it. Likewise for attack.  
    * use more than 1 attack types, dynamic range for weapon attacks  
    * get dynamic weapon range working, teach range. Should not keep moving forward once in weapon attack range.  
    * detect when player in backstab or parry, and when enemy in backstab or parry. know not to do stuff when they are.
    * make this strafe in the same direction as the enemy strafe  
    * Teach poise. Additional inputs for attack neural network: Poise damage of the poise damage of AI's attack and poise of enemy. Their max is sent over wire, but how do i detect hits? Including phantom poise damage.  
    * at closer ranges, perfect block and strafe for bs.

  * **MEDIUM Priority**
    * movement bug. jerky and sometimes off
    * true pathfinding. Read ripped maps
    * Get different base addresses for all possible enemies, allow switching of main target.
    * projectile or lingering hurtboxes.
    * Lock camera(x and y pos) programaticly, rotation x set to PI or 0 depending on map
    * Store all info to be printed in buffer and only print/send to gui on tick end, to save socket writes

  * **LOW Priority**
    * Some cacheing layer. The first time ex:distance is computed in a tick, it is stored in a cache struct, and all subsequent distance calls use the already computed value. Easier then passing it around. When the method is called, it checks if there is an entry in the cache, if not, it computes it and adds it, then returns the value. If there is, it returns that value. AT the end of each logic tick, the cache is invalidated.  
    * ReadPlayer should read memory in chunks, not individual calls to ReadMemory  
    * Better Vjoy loading/unloading    
    * Have a build flag which determines if the program uses my handwritten neural network or a library's neural network    

## Using:   
http://vjoystick.sourceforge.net/ (VJOY)  
http://leenissen.dk/ (FANN)  
