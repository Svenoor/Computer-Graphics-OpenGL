#ifndef GLOBALS_H
#define GLOBALS_H

#include "Angel.h"

// Camera parameters
extern Angel::vec4 eye;
extern Angel::vec4 at;
extern Angel::vec4 up;

// Car transformation variables
extern Angel::vec3 carPosition;
extern GLfloat carRotation;
extern GLfloat wheelRotation;

// Global variable for view mode
extern int viewMode;

#endif
