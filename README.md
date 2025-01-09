# NAVADUEL
A 3D 2 Player Game made using Raylib

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
Quickstart supports the main 3 desktop platforms:
* Windows
* Linux
* MacOS

## Windows Users
* Run `compile.bat` to compile the source code (one-time required)
* Run `NavalDuel.exe` to run the game

## Linux Users
* CD into the build folder
* run `./premake5 gmake2`
* CD back to the root
* run `make`
* you are good to go

## MacOS Users
* CD into the build folder
* run `./premake5.osx gmake2`
* CD back to the root
* run `make`
* you are good to go

### Output files
The built code will be in the bin dir

# <a id="install"></a>INSTALLING

# <a id="exec"></a>EXECUTING

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
### EXIT:
Kill the game instance and exit back to the desktop
### ABOUT:
The about button takes you to a screen where you can view a brief description about the game and view information about the game's authors, as well as being able to go to their github accounts in your browser of choice.
# <a id="docs"></a>DOCUMENTATION
Code is documented in the source files, where each function has a detailed explanation of its usage and its signature. 
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

# <a id="license"></a>License
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
