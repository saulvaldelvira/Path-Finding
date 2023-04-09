# Path Finding
Author: Saúl Valdelvira <br/>

A graphic program to visualize a
path finding algorithm.

### Building:
You can use the Makefile. <br>
Requires [SDL2 library](https://www.libsdl.org/).

### Use:
This is a simple program. You have two points. <br>
You can move them (left mouse click) and place obstacles (right mouse click). <br/>
A line will allways draw the shortest path between those
two points, as long as there is one.

### Keys:
- `V`: Also color the blocks which have been visited during the search
- `M`: Fill all the grid with obstacles, so you can draw a maze
- `C`: Clear the grid
- `T`: Switch between dark and light theme
- `H`: Toggle horizontal movement on and off

### How:
This program uses the A* algorithm to search for paths. <br/>
More info: <https://en.wikipedia.org/wiki/A*_search_algorithm>

![Screenshot](https://img.saulv.es/path_finding.gif)
