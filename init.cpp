#include "Angel.h"
#include "init.h"
#include "objects.h"
#include "display.h"
#include "globals.h"
#include "input.h"

// External variables from other files
extern GLuint program;
extern GLuint vPosition, vColor;
extern mat4 projection;
extern GLuint ModelView, Projection;

// Function prototypes for object creation
void createCar();
void createBuildings();
void createGround();
void createRoads();
void createTrafficLights();

void init()
{
    // Load shaders and use the resulting shader program
    program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // Get attribute locations
    vPosition = glGetAttribLocation(program, "vPosition");
    vColor = glGetAttribLocation(program, "vColor");

    // Create objects
    createCar();
    createBuildings();
    createGround();
    createRoads();
    createTrafficLights();

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Set clear color
    glClearColor(0.5, 0.8, 0.92, 1.0); // Sky blue

    // Get uniform locations
    ModelView = glGetUniformLocation(program, "uModelView");
    Projection = glGetUniformLocation(program, "uProjection");

    // Set up projection matrix
    projection = Perspective(45.0, 800.0 / 600.0, 0.1, 1000.0);
    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

    // Initialize camera position
    updateCamera();
}
