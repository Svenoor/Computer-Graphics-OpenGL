#ifndef OBJECTS_H
#define OBJECTS_H

#include "Angel.h"
#include <vector>

// Define constants
const int NumObjects = 4; // Car, Buildings, Ground, Roads

// Global variables
typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

// Structures for objects
struct Object
{
    std::vector<point4> points;
    std::vector<color4> colors;
    GLuint vao;
    GLuint buffer;
    int numVertices;
    Angel::mat4 modelMatrix; // For individual object transformations
};

// Enum for traffic light states
enum TrafficLightState
{
    RED,
    GREEN,
    YELLOW
};

// Structure for traffic light components
struct TrafficLight
{
    Object base;          // The main pole
    Object lightBox;      // The box containing the lights
    Object lights[3];     // The three lights (Red, Green, Yellow)
    Object connectorPole; // The connector pole connecting to the ground
    TrafficLightState state;
    mat4 modelMatrix; // For positioning
    float stateTime;  // Time since last state change
};

// External variables for objects
extern std::vector<TrafficLight> trafficLights;
extern Object carBody;
extern Object carWheel;
extern std::vector<mat4> wheelTransforms;
extern std::vector<Object> buildings;
extern Object ground;
extern Object roads;

// Function prototypes for object creation
void createCar();
void createBuildings();
void createGround();
void createRoads();
void createTrafficLights();

// Collision detection function
bool checkCollision(Angel::vec3 newPosition);

#endif
