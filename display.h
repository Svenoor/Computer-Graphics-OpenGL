#ifndef DISPLAY_H
#define DISPLAY_H

#include "objects.h" // For the Object struct
#include "Angel.h"   // For mat4

void display();
void drawObject(const Object &obj, const Angel::mat4 &mv);

#endif
