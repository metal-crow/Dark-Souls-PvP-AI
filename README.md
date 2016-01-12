# Dark-Souls-PvP-AI
AI for playing (and winning) in dark souls pvp  
  
People who if you like this you should also follow:  
https://github.com/eur0pa. Creator of dark souls pvp watchdog. Thanks man!  
https://github.com/OrdinaryMagician. Various projects in dark souls 2, and assembly master.   
https://www.youtube.com/user/Birdulon. Master reverse engineerer, seriously he found the Dark Souls 2 bullet params a week after it came out.  
  
##Setup
* Install [Vjoy](http://vjoystick.sourceforge.net/site/index.php/download-a-install).
* Create Vjoy controller with the config: x,y,z,rx,ry,20 buttons,1 discrete pov
* Install [Cheat Engine](http://www.cheatengine.org/)
* Download [Technojacker's DARKSOULS.CT table](https://drive.google.com/folderview?id=0B_f11g1DlLhDV1RfV0VSdnBfOVE&usp=sharing)  
 * Cheat Engine and the table are only used to lock the camera's x rotation to 3.141592. If you dont want to use them, figure out how to do that yourself.
* Download the source code and compile with [FANN](http://leenissen.dk/). Add the FANN dlls and the vJoyInterface dll to the exe's folder.
* Train the defense neural network and put the .net file in a folder in the .exe's location called "Neural Nets"
 * Or skip the above two and use the folder in release.
* (If you want the gui) Import the folder for the GUI into eclipse and run that. Or run the included .jar file.
* Run the PvP AI whenever the opponent is ready.

## Neural Networks
Defense Network Inputs:  
  -Player Distance from Enemy  
  -The angle the enemy is off from directly in front of the player  
  -The enemy velocity  
  -The rotation difference between the enemy and the player  

Attack Network Inputs:  
  -TODO. Undecided currently.

## TODO
resolution at 1440x810

  * **TOP Priority**
    * spear game too strong
    * glitch with timers not resetting between animations. If the last one ended at say 0.3 and the next one has a dodge timer of 0.5, we will dodge immediatly, and early. Try and find another timer 1.
    * memory for if enemy likes to combo, and dodge away if so. combos wreck its shit.
    * i was wrong. dead angles dont protect against parrying.
    * should attack even when enemy attacking, just determine their direciton and attack to their side or behind.
    * too prone to trading, imrpove attack neural network  
    * impove bs neural network accuracy. try to bs with neural network on and using strafing, train with that
    * CE camera lock script causes crashes. Fix: disable all scripts before invasions and disconnect.

  * **HIGH Priority**
    * have fully automated version. When in world, put down sign. When enemy detected, enable ai. Put on server, steam to switch.
    * refine behind enemy safe state to be more behind enemy. Cone like?
    * more specifications on neural network output. Defense net should return how exactly to avoid bs, not just detect it. Likewise for attack.  
    * use more than 1 attack types, dynamic range for weapon attacks  
    * get dynamic weapon range working, teach range. Should not keep moving forward once in weapon attack range.  
    * detect when player in backstab or parry, and when enemy in backstab or parry. know not to do stuff when they are.
    * make this strafe in the same direction as the enemy strafe  
    * teach poise  

  * **MEDIUM Priority**
    * true pathfinding. Rip maths, use a*
    * Get different base addresses for all possible enemies, allow switching of main target.
    * rolling into mlgs r2 hurtbox. should always roll away from that attack?
    * Pointers have to be reread for characters because end address changes when invading  
    * Must lock camera(x and y pos), rotation x set to PI or 0 depending on map.
    * Store all info to be printed in buffer and only print/send to gui on tick end, to save socket writes

  * **LOW Priority**
    * Some cacheing layer. The first time ex:distance is computed in a tick, it is stored in a cache struct, and all subsequent distance calls use the already computed value. Easier then passing it around.  
    * ReadPlayer should read memory in chunks, not individual calls to ReadMemory  
    * Better Vjoy loading/unloading    
    * Have a build flag which determines if the program uses my handwritten neural network or a library's neural network    

## Using:   
http://vjoystick.sourceforge.net/ (VJOY)  
http://leenissen.dk/ (FANN)  
