#include "globals.h"

// Camera parameters
Angel::vec4 eye;
Angel::vec4 at;
Angel::vec4 up(0.0, 1.0, 0.0, 0.0);

// Car transformation variables
Angel::vec3 carPosition(0.0, 0.0, 0.0);
GLfloat carRotation = 0.0f;   // In degrees
GLfloat wheelRotation = 0.0f; // For wheel animation

// Global variable for view mode
int viewMode = 1; // Default view (F1)
