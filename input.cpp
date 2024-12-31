#include "Angel.h"
#include "input.h"
#include "globals.h"
#include "objects.h"
#include "display.h"

// External variables from other files
extern int viewMode;
extern vec3 carPosition;
extern GLfloat carRotation;
extern GLfloat wheelRotation;
extern vec4 eye;
extern vec4 at;
extern vec4 up;
extern mat4 projection;
extern GLuint Projection;

// Movement variables
bool movingForward = false;
bool movingBackward = false;
GLfloat carSpeed = 0.0f;
const GLfloat maxSpeed = 0.1f;
const GLfloat acceleration = 0.05f;
const GLfloat deceleration = 0.01f;

// Collision detection function
bool checkCollision(Angel::vec3 newPosition);

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 033: // Escape key
    case 'q':
    case 'Q':
        exit(EXIT_SUCCESS);
        break;
    }
    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        movingForward = true;
        movingBackward = false;
        break;
    case GLUT_KEY_DOWN:
        movingBackward = true;
        movingForward = false;
        break;
    case GLUT_KEY_LEFT:
        // Turn car left by 90 degrees on the spot
        carRotation += 90.0f;
        wheelRotation = 0.0f; // Reset wheel rotation when turning
        break;
    case GLUT_KEY_RIGHT:
        // Turn car right by 90 degrees on the spot
        carRotation -= 90.0f;
        wheelRotation = 0.0f; // Reset wheel rotation when turning
        break;
    case GLUT_KEY_F1:
        viewMode = 1; // Default view
        updateCamera();
        break;
    case GLUT_KEY_F2:
        viewMode = 2; // Overhead view
        updateCamera();
        break;
    case GLUT_KEY_F3:
        viewMode = 3; // Side view
        updateCamera();
        break;
    case GLUT_KEY_F4:
        viewMode = 4; // Driver view
        updateCamera();
        break;
    }
    glutPostRedisplay();
}

void keyUpSpecial(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        movingForward = false;
        break;
    case GLUT_KEY_DOWN:
        movingBackward = false;
        break;
    }
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    // Update projection matrix
    projection = Perspective(45.0, GLfloat(width) / height, 0.1, 1000.0);
    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

void idle()
{
    bool moved = false;

    if (movingForward)
    {
        // Accelerate
        if (carSpeed < maxSpeed)
            carSpeed += acceleration;
    }
    else if (movingBackward)
    {
        // Accelerate in reverse
        if (carSpeed > -maxSpeed)
            carSpeed -= acceleration;
    }
    else
    {
        // Decelerate to a stop
        if (carSpeed > 0.0f)
        {
            carSpeed -= deceleration;
            if (carSpeed < 0.0f)
                carSpeed = 0.0f;
        }
        else if (carSpeed < 0.0f)
        {
            carSpeed += deceleration;
            if (carSpeed > 0.0f)
                carSpeed = 0.0f;
        }
    }

    if (carSpeed != 0.0f)
    {
        // Calculate new position
        Angel::vec3 newPosition = carPosition;
        newPosition.x -= sin(DegreesToRadians * carRotation) * carSpeed;
        newPosition.z -= cos(DegreesToRadians * carRotation) * carSpeed;

        // Check for collisions
        if (!checkCollision(newPosition))
        {
            carPosition = newPosition;
            moved = true;
            // Update wheel rotation
            wheelRotation += (360.0f * carSpeed) / (2.0f * M_PI * 0.5f); // Assuming wheel radius of 0.5
        }
        else
        {
            // Collision detected, stop the car
            carSpeed = 0.0f;
        }
    }

    // Update the camera to follow the car
    updateCamera();

    // Update traffic lights
    for (auto &tl : trafficLights)
    {
        tl.stateTime += 0.01f; // Increment time

        // Cycle through the states
        if (tl.state == RED && tl.stateTime >= 2.0f)
        {
            tl.state = GREEN;
            tl.stateTime = 0.0f;
        }
        else if (tl.state == GREEN && tl.stateTime >= 2.0f)
        {
            tl.state = YELLOW;
            tl.stateTime = 0.0f;
        }
        else if (tl.state == YELLOW && tl.stateTime >= 1.0f)
        {
            tl.state = RED;
            tl.stateTime = 0.0f;
        }
    }

    glutPostRedisplay();
}

void updateCamera()
{
    // Common look-at point (car's position)
    at = vec4(carPosition.x, carPosition.y + 1.0f, carPosition.z, 1.0f);

    if (viewMode == 1) // Default view (F1)
    {
        // Offset behind the car
        float distanceBehind = 5.0f;
        float heightAbove = 3.0f;

        // Calculate the camera position based on the car's rotation
        float radRotation = DegreesToRadians * carRotation;
        eye.x = carPosition.x + sin(radRotation) * distanceBehind;
        eye.y = carPosition.y + heightAbove;
        eye.z = carPosition.z + cos(radRotation) * distanceBehind;

        // Set up vector for default view
        up = vec4(0.0, 1.0, 0.0, 0.0);
    }
    else if (viewMode == 2) // Overhead view (F2)
    {
        // Position the camera directly above the car
        eye.x = carPosition.x;
        eye.y = carPosition.y + 20.0f; // Height above the car
        eye.z = carPosition.z;

        // Adjust the look-at point to the car's exact position
        at = vec4(carPosition.x, carPosition.y, carPosition.z, 1.0f);

        // Set up vector for overhead view
        up = vec4(0.0, 0.0, -1.0, 0.0); // Adjust up vector when looking down
    }
    else if (viewMode == 3) // Side view (F3)
    {
        // Position the camera slightly above and to the right of the car
        float distanceSide = 5.0f;
        float heightAbove = 3.0f;

        float radRotation = DegreesToRadians * carRotation;

        // Calculate the right side position based on the car's rotation
        eye.x = carPosition.x + cos(radRotation) * distanceSide;
        eye.y = carPosition.y + heightAbove;
        eye.z = carPosition.z + sin(radRotation) * distanceSide;

        // Set up vector for side view
        up = vec4(0.0, 1.0, 0.0, 0.0);
    }
    else if (viewMode == 4) // Driver view
    {
        float eyeHeight = 2.0f;
        float eyeOffsetX = 0.0f;  // Centered inside the car
        float eyeOffsetZ = -0.5f; // Behind the dashboard

        float radRotation = DegreesToRadians * carRotation;

        eye.x = carPosition.x + sin(radRotation) * eyeOffsetZ + cos(radRotation) * eyeOffsetX;
        eye.y = carPosition.y + eyeHeight;
        eye.z = carPosition.z + cos(radRotation) * eyeOffsetZ - sin(radRotation) * eyeOffsetX;

        at.x = carPosition.x - sin(radRotation) * 10.0f;
        at.y = carPosition.y + eyeHeight;
        at.z = carPosition.z - cos(radRotation) * 10.0f;

        up = vec4(0.0, 1.0, 0.0, 0.0);
    }
}
