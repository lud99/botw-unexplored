# BotW Unexplored
Easily view what hasn't been discovered in your Breath of the Wild savefile, on your Nintendo Switch.

The korok seeds that haven't been collected and the locations that are left to visit are shown on a map, where it's easy to see koroks or locations you've missed. 

## Why?
There are other websites and tools to display and even edit your BotW savefile, but none of them run on your Switch. You would have to backup your savefile and then transfer it to a computer, which is unnecessarily complicated. That's why I created this homebrew app.

## Usage
* Download ```botw-unexplored.nro```, transfer it to your SD card and launch it.
* Choose the profile with your savefile
* Done! Use the analog sticks to move the camera, or use the touch screen to move around

## Building
switch-mesa, switch-glad, switch-freetype and switch-glm are required for building the project. Install them with the command ```pacman -S switch-mesa switch-glad switch-glm switch-freetype```  
Then run ```Make``` to build the .nro file.

### Credits
Huge thanks to these kind people for making this project possible

https://github.com/marcrobledo/savegame-editors For most of the data and my primary inspiration
https://github.com/MrCheeze/botw-waypoint-map For all the location data
https://github.com/d4mation/botw-unexplored-viewer For the savefile parsing