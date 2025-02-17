# NAVADUEL
A 3D 2 Player Game made using Raylib.\
This project (v1.0) served as a University project for the Structured Programming (004) University course of AUTh (Aristotle University of Thessaloniki), but we decided to take it to the next level.

## Chapters:
* [DESCRIPTION](#desc)
* [BUILDING](#build)
* [INSTALLING](#install)
* [EXECUTION](#exec)
* [INSTRUCTIONS](#instructions)
* [DOCUMENTATION](#docs)
* [REFERENCES](#refs)
* [LICENSE](#license)

# <a id="desc"></a>DESCRIPTION
Navaduel is a game where you and up to 7 friends can all duke it out with pirate ships trying to sink each other in the open sea full of randomly generated obstacles by precisely aiming your cannons at each others current positions until only one team is left.\
The game also features a real-time 1v1 game mode where you can face off against one friend in real-time combat instead of taking turns and analysing each other players movement.

# <a id="build"></a>BUILDING
### Supported Platforms
* Windows
* Linux
* MacOs

The program will be built upon trying to install via the provided install script.\
To manually build program, you need cmake. Refer to [cmake's official download page](https://cmake.org/download/) for instructions
### Windows:
To keep root dir clean, create a build directory: `mkdir build`\
To generate makefiles using cmake, run: `cmake -B build`. Keep in mind you might have to use a different generator, specified with the -G option. (e.g. -G "MinGW Makefiles")\
To create binary, run: `cmake --build build`\
Executable should be in ./build/Navaduel\
To ensure default settings are applied and work correctly, along with saves and other resources, always keep the executable in the root directory.\
You can do that in the command line with:\ 
`cd build`\
'move "Navaduel.exe" ".."`

### Linux/MacOs:
To keep root dir clean, create a build directory: `mkdir build`\
To generate makefiles using cmake, run: `cmake -B build`\
To create binary, run: `cmake --build build`\
Binary should be in ./build/Navaduel\
If needed, make it executable with: `sudo chmod +x ./build/Navaduel`\
To ensure default settings are applied and work correctly, along with saves and other resources, always keep the binary in the root directory.

# <a id="install"></a>INSTALLING
### Windows:
Clone repository: `git clone https://github.com/panos21sk/Navaduel`\
cd into cloned repo: `cd Navaduel`\
To install the game, if you have MinGW build tools installed and on your systems path, simply double click install.bat.\
Else you might have to modify `cmake -S . -B build -G "MinGW Makefiles"` and change MinGW Makefiles into the build system youre using (Refer to CMake Docs)\
If the install script ran successfully, a batch script named run_navaduel.bat should have appeared on your desktop. 

### Linux/MacOs:
Clone repository: `git clone https://github.com/panos21sk/Navaduel`\
cd into cloned repo: `cd Navaduel`\
Ensure install.sh is executable: `sudo chmod +x ./install.sh`\
Run install.sh: `./install.sh`\
The install will build the binary, move it to the root folder, and also create a .desktop file in your $HOME/Desktop folder, as well as in your $HOME/.local/share/applications/ folder

# <a id="exec"></a>EXECUTING
### Windows:
After installing the game, either open the game's root directory and double click Navaduel.exe or double click run_navaduel.bat, which by default should be on your desktop.

### Linux/MacOs:
You can either: Open your terminal in the game's root folder and run the binary with: `./Navaduel`\
Or you can also execute (or double-click) the Navaduel.desktop file on your desktop and $HOME/desktop folder\
Or if using programs like rofi, you can search for it there since the .desktop is in your $HOME/.local/share/applications/ folder.

# <a id="instructions"></a>INSTRUCTIONS
### MAIN MENU:
On execution, the game puts you in the main menu. Here, all screens can be accessed by clicking the corresponding button.
### PLAY:
Start a session. By clicking game, you are put in the gamemode selection screen, where you are prompted to enter the desired amount of players by hovering over the player count box and inputting a number ranging from 2-8 on your keyboard. Default is 2.\
After that you can decide between entering a turn-based game or a real-time game. Only if you are to play with 2 players, is the real-time mode accessible. Also note that with 2 players in both real-time and turn-based mode each ship has different controls, whereas with over 3 players in turn based each player shares the same controls, as is specified in the controls screen.\
Regardless of the mode of your choice, you will be asked to select a ship for each player, and you will be informed about the advantages of each ship. Select a ship by clicking on the corresponding box. Current ship selected has a darker hue on its box. Default is ship 1.\
If you are to play with 3 or more players, after selecting your ship in the turn-based game mode, you will be prompted to select each players team. Team selection resets every new game, unlike ship selection. Game ends when either only 1 team is alive or when 1 player with no team is alive.\
### REAL-TIME / TURN-BASED GAME DIFFERENCES:
* Real-time: You and 1 other person can control 1 ship at a time and face off concurrently with a split screen setup. You can always move around freely and fire as many times as you have to in order to destroy your opponent.\
* Turn-based: You and up to 7 other people each taking turns controlling the displayed ship. The game starts by selecting a random player to go first. Each player has a move period, where they can move and relocate their ship, followed by a fire period where they can aim their shot and shoot it. After that the game finds and hands over control to the next player alive in the queue. 
### PAUSE MENU:
By pressing escape in game you go to the pause menu, where the game's process is blocked and its state remains the same, allowing you to continue when you wish by clicking continue.\
You can also save the game's state to a .json file and return to that game state at a later time using the save game button.\
You can also return to the main menu using the exit button. 
### LOAD GAME:
During the game, after pressing the escape button, you can choose to save the game state to an external .json file.
\By clicking the LOAD GAME button on the main menu, you can restore the saved game state.
### OPTIONS:
In this screen you can enable/disable the reticle(sightline + target), first person mode, fullscreen, background music, and sound effects.
### CONTROLS:
In this screen you change the key bindings to the several actions in game, by hovering over the desired key you want to change, clicking the left mouse button, and then pressing your new desired key. Note that the new input will be canceled if your mouse leaves the binding's box. Also, after clicking, you will be given a description of the binding's specified action.\
For a 2 player game, the 2 players need to have different controls. In a game with more than 2 players, all players share the same controls.
Given actions:\
* Fire: Hold fire button to shoot a cannonball from your cannon. Holding increases power. Lower bar at bottom right has a yellow bar indicating power of last shot \
* Forward/Backwards: Move ship forward or backward.\
* Left/Right: Steer ship towards left/right.\
* Cannon left/Cannon right: Turn your cannon towards the left or towards the right, to better aim your shot.\
Note: Fire, along with cannon left and cannon right buttons only work during the fire phase of the turn based gamemode.
### EXIT:
Kill the game instance and exit back to the desktop
### ABOUT:
The about button takes you to a screen where you can view a brief description about the game and view information about the game's authors, as well as being able to go to their github accounts in your browser of choice.
# <a id="docs"></a>DOCUMENTATION
Code is documented in the source files, where each function has a detailed explanation of its usage and its signature.\
docs.pdf is also provided as a pdf containing all function signatures, along with descriptions for their functionality within the program, as well as information about their parameters, and return values. The contents were not included in the readme to save space and increase readability.
# <a id="refs"></a>REFERENCES
* https://www.raylib.com/cheatsheet/cheatsheet.html
* https://www.raylib.com/examples.html
### Examples referenced:
* basic screen manager
* 3d camera split screen
* sprite anim
* sprite explosion
* input box
* loading gltf
* geometric shapes
* box collisions
* yaw pitch roll
* skybox
* draw cube texture
* music stream
* sound loading
### Raylib cmake template (Modified further by Navaduel authors):
* https://github.com/SasLuca/raylib-cmake-template

# <a id="license"></a>License

### Third-Party Libraries
This project uses the following libraries:
- [cJSON](https://github.com/DaveGamble/cJSON) - MIT License
- [inih](https://github.com/benhoyt/inih) - New BSD License

See ./COPYING for details

Copyright (C) 2024 Panagiotis Skoulis, Dimitrios Kakagiannis

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
