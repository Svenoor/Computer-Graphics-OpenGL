# City Scene Simulation

This project simulates a small city with roads, traffic lights, buildings, and a drivable car using **OpenGL**, **GLUT**, and **C++**. It demonstrates hierarchical transformations, basic collision detection, multiple camera views, and real-time updates (e.g., traffic light changes).

---

## Features
1. **Car Simulation**  (Use arrow keys to drive around)
   - Drive a car around the grid with adjustable speed, rotation, and animated wheels.

2. **Traffic Lights**  
   - Fully modeled traffic lights with RED, YELLOW, and GREEN states cycling over time.

3. **City Layout**  
   - Roads, buildings, and ground plane arranged in a grid.

4. **Multiple Camera Views**  (F1, F2, F3, and F4)
   - Switch among default, overhead, side, and driver’s (inside) views.

5. **Collision Detection**  
   - Prevents the car from colliding with buildings or going outside map boundaries.

---
- **main.cpp**  
  Contains the main function, sets up the GLUT window, and registers callback functions.

- **init.cpp**  
  Initializes OpenGL, loads shaders, sets up projection matrices, and calls object creation functions.

- **display.cpp**  
  Manages the rendering loop: clearing buffers, drawing objects, and swapping buffers.

- **input.cpp**  
  Handles user input (keyboard, special keys), updates simulation state, manages camera views, and contains idle/reshape callbacks.

- **objects.cpp**  
  Builds geometric data for the car, buildings, ground, roads, and traffic lights. Uses structs (`Object`, `TrafficLight`) for hierarchical transformations.

- **globals.cpp**  
  Stores global variables (camera position, car transformation data, and current view mode).

- **vshader.glsl / fshader.glsl**  
  Vertex and fragment shaders for rendering.

---

## Dependencies
- **OpenGL** (>= 3.2 recommended)
- **GLUT** (e.g., [FreeGLUT](http://freeglut.sourceforge.net/))
- A **C++ compiler** that supports C++11 (or later)
- (Optional) **GLEW** or equivalent extension loader depending on your setup

---

## Installation and Setup

On Ubuntu/Debian:
Download the zip folder
- Open Terminal:
make
- ./project
![Screenshot from 2024-12-30 20-53-02](https://github.com/user-attachments/assets/33b6aac7-46ce-418a-a495-3895bf5cf48d)
