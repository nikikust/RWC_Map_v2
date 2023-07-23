# RailwayCraft Map, version 2
This is a database program for RWC.

**Important!** To get a database you need to be a RWC Discord member (contact me: nikikust)!
To succesfully run a program you need "data.rr" file and Maps folder.

---

## List of stored information
- Railroads data:
    - existing lines on map;
    - accepted plans;
    - railroad Zones (subdivisions, states: Built/In Progress/Plan);
    - AKA names (previous names, abbreviations, simplified names);
    - associated players (owners).
- Players data:
    - server nicknames;
    - discord nicknames;
    - associated railroads (as owners).
- Map data:
    - current map (latest downloaded copy);
    - old map (previous copy);
    - difference map (diff. between Current and Old maps, marked with red).

![](https://raw.githubusercontent.com/nikikust/RWC_Map_v2/main/Raw/images/main_view.png "Main view")

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

![](https://raw.githubusercontent.com/nikikust/RWC_Map_v2/main/Raw/images/line_edit.png "Line editing")


**Railroads list:**
- add new railroad;
- search among added railroad (with partial search and search in AKA names);
- move camera to the first point of railroad.

![](https://raw.githubusercontent.com/nikikust/RWC_Map_v2/main/Raw/images/railroads_list.png "Railroads list")


**Railroad info:**
- view and edit associated players;
- edit AKA names;
- control zones:
    - create/delete zones,
    - select and show specific zone of railroad,
    - change zone type (Built/In Progress/Plan),
    - move zone between railroads (when bought or joined).
- hide on map view.

![](https://raw.githubusercontent.com/nikikust/RWC_Map_v2/main/Raw/images/railroad_info.png "Railroad info")


**Players list:**
- add new player;
- search among added players (with partial search).

![](https://raw.githubusercontent.com/nikikust/RWC_Map_v2/main/Raw/images/players_list.png "Players list")

**Player info:**
- set in game nickname;
- set discord nickname;
- move camera to associated railroads.

![](https://raw.githubusercontent.com/nikikust/RWC_Map_v2/main/Raw/images/player_info.png "Player info")

**Settings:**
- setup autosave in minutes;
- move camera to coordinates;
- show hidden railroads;
- hide map (for lines overview);
- select map (current/difference/old);
- enable ulimited scaling (by default scaling is limited).

![](https://raw.githubusercontent.com/nikikust/RWC_Map_v2/main/Raw/images/settings.png "Settings")

**Length top:**
- list of all added railroads with their lengths;
- copy list to clipboard;
- select which zone types should be counted;
- total tracks length;
- search specific railroad (with partial search and search in AKA names).

![](https://raw.githubusercontent.com/nikikust/RWC_Map_v2/main/Raw/images/length_top.png "Length Top")

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
RMB               | Move camera (when in 'Line Edit' mode													                   |

---

### Settings:
In the "Data" folder there is a file named "settings.json". In it you can set the path to the folder with maps.
