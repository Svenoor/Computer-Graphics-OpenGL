#include "Angel.h"
#include "init.h"
#include "display.h"
#include "input.h"
#include "objects.h"

// External variables (from other files)
extern GLuint program;
extern mat4 projection;

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    // Set up display mode: double buffering, RGBA, depth buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Project");

    glewExperimental = GL_TRUE;
    glewInit();

    init();

    // Register callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutSpecialUpFunc(keyUpSpecial); // For key release events
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    // Enter the main loop
    glutMainLoop();
    return 0;
}
