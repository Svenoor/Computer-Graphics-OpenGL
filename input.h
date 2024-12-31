#ifndef INPUT_H
#define INPUT_H

void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void keyUpSpecial(int key, int x, int y);
void reshape(int width, int height);
void idle();
void updateCamera();

#endif
