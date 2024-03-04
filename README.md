# csgo-gsi-ui
Representing game state data from the game CS:GO and CS2 using its Game State Integration with the ImGui library. (WIP)

## Purpose
The intention is to create a server that will receive live data from the game CS:GO/CS2 where a user is spectating some match and visualize that data 
with an arbitrary chosen user interface (textual or graphical). One of the goals is to isolate the data provision code from the code running a user interface 
(for example ImGUI).

## The CS:GO/CS2 Game State Integration
The data that the server receives is coming from a running instance of the game thanks to the CS:GO/CS2 Game State Integration. 
This functionality is described here: https://developer.valvesoftware.com/wiki/Counter-Strike:_Global_Offensive_Game_State_Integration.
The payload that the game sends is described in this Reddit post: https://www.reddit.com/r/GlobalOffensive/comments/cjhcpy/game_state_integration_a_very_large_and_indepth/.

## User Interface(s)
Thanks to the Observer design pattern (see: https://refactoring.guru/design-patterns/observer) it is possible to attach various user interfaces that will receive constant
game state updates. One of them could be e.g. ImGUI (https://github.com/ocornut/imgui) which is planned to be implemented.
