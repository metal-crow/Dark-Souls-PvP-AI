# Dark-Souls-PvP-AI
AI for playing (and winning) in dark souls pvp  
  
People who if you like this you should also follow:  
https://github.com/infausto. Thanks for watchdog, and the work you have done.  
https://github.com/OrdinaryMagician. You do some fucking SHIT with assembly, man.  
  
-reverse engineer dark souls plantom player pointer and struct  
-code to read darksouls.exe's memory   
-send joystick commands  
-ai logic to play  
  
NOTES:   
  
Can ignore z, view as top down from x,y only  
  
ignore camera, its irrelivent (mostly)  
  
map out phantom struct  
  
ps3 controller  
circle:2  
x:1  
square:3  
triangle:4  
r1:6  
l1:5  
select:7  
start:8  
l3:9  
r3:10  
r2:z-0  
l2:z-max  
  
TODO:  
  Find SubAnimation (when hitbox is created). Found one that works for r1's, but not r2's.  
  Find location of where exactly weapon is.      
  Find range of weapons(hitbox size,bows,etc)  
  Method for determining if animation id is attack animation or different  
  Find name location, try to fix name length crash(test namefind without pvp watchdog enabled)

Using:   
http://vjoystick.sourceforge.net/ (note: figure out better way than just puting .dll in with .exe)(i dont know if i like the program)  
