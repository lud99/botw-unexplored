# [Tears of the Kingdom Unexplored is now released](https://github.com/lud99/totk-unexplored)

# BotW Unexplored
Easily view what hasn't been discovered in your Breath of the Wild savefile, on your Nintendo Switch.

The korok seeds that haven't been collected and the locations that are left to visit are shown on a map, where it's easy to see koroks or locations you've missed. You can also see shrines, hinoxes, taluses and moldugas too. 

<img src="https://github.com/lud99/botw-unexplored/blob/master/map3_2.0.jpg?raw=true" width=600>

## Why?
There are other websites and tools to display and even edit your BotW savefile, but none of them run on your Switch. You would have to backup your savefile and then transfer it to a computer, which is unnecessarily complicated. That's why I created this homebrew app.

## Usage
Download ```botw-unexplored.nro``` from the [releases](https://github.com/lud99/botw-unexplored/releases/download/2.0.0/botw-unexplored.nro), transfer it to your SD card and launch it. Most things should be self explanitory, but here's some information just in case:

* Use the analog sticks or the touch screen to move around.  
* Press X to open the legend and use either the touch screen or the D-pad to navigate the menu (note that the analog sticks won't work here) 
* Tap a korok to see a guide on how to find it. You can press B to manually mark it as complete. This is useful if you're using the app while playing BotW, as the app won't be able to load your latest savefile in that case. The koroks you've marked as found will be removed once the savefile is able to be loaded it again, so don't worry if you wrongly marked a korok as found.

## Version History

### 2.0
* Added Shrines (+ DLC if it's present), Taluses, Hinoxes and Moldugas.
* Added a legend where you can toggle which of the collectibles you want to see.
* It's now possible to use the app while playing the game. This is achived by making backups of your savefile, which are then loaded while the game is running.
* Added guides for finding the koroks. The text and images are taken directly from Zeldadungeons interactive map. Simply tap a korok and a guide will pop up. Very helpful for certain korok seeds that are hard to find with only a location on a map.
* When a korok is selected, press B to manually mark it as complete. Very helpful if you're using the app while playing as the latest savedata can't be read. Otherwise you would have to remember all the koroks you've found during this play session, which could become difficult. The correct korok progress will be restored once the game has been closed.
* "Korok paths" have also been added to help finding some koroks. If you've used Zeldadungeons interactive map, then you know what they are.
* Added support for Master Mode. Press Y to toggle it.
* Drastically improved performance when a lot of objects are displayed. Can easily run at 60fps now.
* The app remebers your last camera position and zoom. This makes it easy to get back to the korok you looked at last time, if you for example switch between BotW and the app. (You can also press X in the Homebrew menu to star it for quicker access)
* Now tries to load the last used user so you won't have to use the dialog picker every time you start the app.
* Added more error messages and all logs are saved to a text file in case the app doesn't work
### 1.0
* Initial release

## Building
switch-mesa, switch-glad, switch-freetype and switch-glm are required for building the project. Install them with the command ```pacman -S switch-mesa switch-glad switch-glm switch-freetype```  
Then run ```Make``` to build the .nro file.

### Credits
Huge thanks to these kind people for making this project possible:

* https://github.com/marcrobledo/savegame-editors For most of the data and my primary inspiration.  
* https://github.com/MrCheeze/botw-waypoint-map For all the location data. 
* https://zeldadungeon.net for their amazing korok guides and images that i "borrowed" :) 
* https://github.com/d4mation/botw-unexplored-viewer For the savefile parsing

Extra thanks to d4mation for all their suggestions and issues on github and helping me figure out other things about BotW (you're awesome!)
