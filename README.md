# RailwayCraft Map, version 2
This is a database program for RWC.

**Important!** To get a database you need to be a RWC Discord member (DM me: @nikikust)!
To succesfully run a program you need "data.rr" file and Maps folder.

The program creates a backup copy of the railroads data before each save (in ./Data/data_backups/).

---

## List of stored information
- Railroads data:
    - existing lines on map;
    - accepted plans;
    - railroad Zones (subdivisions, states: Built/In Progress/Plan);
    - AKA names (previous names, abbreviations, simplified names);
    - associated players (owners);
    - length top (and comparision with previous data).
- Players data:
    - server nicknames;
    - discord nicknames;
    - associated railroads (as owners).
- Map data:
    - current map (latest downloaded copy);
    - old map (previous copy);
    - difference map (diff. between Current and Old maps, marked with red).
    
![](https://github.com/nikikust/RWC_Map_v2/blob/main/Raw/images/main_view.png?raw=true "Main view")

---

## Application functionality
**Line editing:** 
- add points and lines, asign them to railroads and zones;
- show/hide: points (for better view or to increase FPS);
- show/hide: lines (for map overview);
- show/hide: Built/In Progress/Plan areas;
- change Railroads view:
    - rename railroad,
    - change color,
    - move points.

![](https://github.com/nikikust/RWC_Map_v2/blob/main/Raw/images/line_edit.png?raw=true "Line editing")

---

**Railroads list:**
- add new railroad;
- search among added railroad (with partial search and search in AKA names);
- move camera to the first point of railroad.

![](https://github.com/nikikust/RWC_Map_v2/blob/main/Raw/images/railroads_list.png?raw=true "Railroads list")

---

**Railroad info:**
- view and edit associated players;
- edit AKA names;
- control zones:
    - create/delete zones,
    - select and show specific zone of railroad,
    - change zone type (Built/In Progress/Plan),
    - move zone between railroads (when bought or joined).
- hide on map view.

![](https://github.com/nikikust/RWC_Map_v2/blob/main/Raw/images/railroad_info.png?raw=true "Railroad info")

---

**Players list:**
- add new player;
- search among added players (with partial search).

![](https://github.com/nikikust/RWC_Map_v2/blob/main/Raw/images/players_list.png?raw=true "Players list")

---

**Player info:**
- set in game nickname;
- set discord nickname;
- move camera to associated railroads.

![](https://github.com/nikikust/RWC_Map_v2/blob/main/Raw/images/player_info.png?raw=true "Player info")

---

**Settings:**
- setup autosave in minutes;
- move camera to coordinates;
- show hidden railroads;
- hide map (for lines overview);
- select map (current/difference/old);
- enable ulimited scaling (by default scaling is limited).

![](https://github.com/nikikust/RWC_Map_v2/blob/main/Raw/images/settings.png?raw=true "Settings")

---

**Length top:**
- list of all added railroads with their lengths (with comparision with previous data - difference in length and position);
- copy list to clipboard;
- select which zone types should be counted;
- total tracks length;
- search specific railroad (with partial search and search in AKA names).

![](https://github.com/nikikust/RWC_Map_v2/blob/main/Raw/images/length_top.png?raw=true "Length Top")

---

## Usage

Menus can be moved and docked inside each other.

**To move Point**, hover it and pull. Press Escape when pulled to return it to original position.

**To select Railroad**, click on line on map or on name at 'Railroads list' menu.

**To select Zone**, first select railroad and then click on name at 'Railroad info' which appeared when you selected railroad.

**To add line**, you need to select railroad and zone. Then just click on any point - you are now in '*Line Edit*' mode. You can use *RMB*, to move camera when in this mode. Press Escape to exit it.

At 'Line edit' menu you can turn on/off **point alignment** at creation and movement.

When in '*Line Edit*' mode you have these ways to **add line**
- Click on free space to put point there and connect line to it.
- Click on the any point to connect line to it (can avoid alignment).
- Click on the any line to split it and add point there.

---

### Shortcuts:

Key               | Description                                                                                                |
---               | ---                                                                                                        |
A                 | Add point (will split line if hovered								                                       |
D/Delete          | Delete hovered point or line															                   |
Escape            | Exit app or disable Tool																                   |
H                 | Hide selected railroad																	                   |
M                 | Open current location on RWC livemap													                   |
Numpad -,+ or [,] | Zoom																					                   |
P                 | Make a screenshot																		                   |
F1-6,10           | Open menus																				                   |
Ctrl+S            | Save data																				                   |
LMB               | Move camera (when on free space).<br/>Move point (when hovered)<br/>Select railroad (when line is clicked) |
RMB               | Move camera (when in 'Line Edit' mode)													                   |

---

### Settings:
In the "Data" folder there is a file named "settings.json". In it you can set the path to the folder with maps.

---

### How to build

Program tested at Windows 10 (Visual Studio 2022) and Ubuntu 22.04 LTS (CMake).

#### Windows (Visual Studio & vcpkg):
- Install Visual Studio and vcpkg;
- Bind vcpkg and VS:
```
vcpkg integrate install
```
- Install required packages:<br/>
```
vcpkg install sfml:x64-windows
vcpkg install imgui-sfml:x64-windows
vcpkg install imgui[docking-experimental]:x64-windows
vcpkg install nlohmann-json
```
- Open .sln project with VS and build.


#### Windows (Visual Studio & CMake)
- Open folder as CMake project
- Build app


#### Windows (CMake)  - Not Competed!
- Install CMake and C++ compiler
- Prepare build folder:
```
mkdir .build && cd .build
```
- Build app:
```
cmake --build .
```


#### Linux (CMake)
- Get required packages:
```
apt-get update
apt-get install -y sudo build-essential git cmake curl unzip tar zip autoconf libtool pkg-config libx11-dev libxrandr-dev libxi-dev libudev-dev libgl1-mesa-dev 
```
- Prepare build folder:
```
mkdir .build && cd .build
```
- Build app:
```
cmake ..
make
```

