#include "Angel.h"
#include "display.h"
#include "globals.h"
#include "objects.h"
#include "input.h"

// External variables
extern mat4 model_view;
extern mat4 projection;
extern GLuint ModelView, Projection;
extern GLuint vPosition, vColor;

// Objects from objects.cpp
extern Object ground;
extern Object roads;
extern std::vector<Object> buildings;
extern std::vector<TrafficLight> trafficLights;
extern Object carBody;
extern Object carWheel;
extern std::vector<mat4> wheelTransforms;

// Camera parameters from input.cpp
extern vec4 eye;
extern vec4 at;
extern vec4 up;

// Car transformation variables from input.cpp
extern vec3 carPosition;
extern GLfloat carRotation;
extern GLfloat wheelRotation;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up the view matrix
    model_view = LookAt(eye, at, up);

    // Draw the ground
    drawObject(ground, model_view);

    // Draw the roads
    drawObject(roads, model_view);

    // Draw buildings
    for (const auto &building : buildings)
    {
        drawObject(building, model_view * building.modelMatrix);
    }

    // Draw traffic lights
    for (auto &tl : trafficLights)
    {
        mat4 tl_mv = model_view * tl.modelMatrix;

        // Draw the base (main pole)
        drawObject(tl.base, tl_mv);

        // Draw the light box
        drawObject(tl.lightBox, tl_mv);

        // Draw the connector pole
        mat4 connectorPole_mv = tl_mv * tl.connectorPole.modelMatrix;
        drawObject(tl.connectorPole, connectorPole_mv);

        // Draw the lights
        for (int k = 0; k < 3; ++k)
        {
            color4 offColor = color4(0.1, 0.1, 0.1, 1.0); // Dim color for off lights
            color4 onColor;

            // Determine which light should be on
            if (tl.state == RED && k == 0)
                onColor = color4(1.0, 0.0, 0.0, 1.0); // Red
            else if (tl.state == GREEN && k == 1)
                onColor = color4(0.0, 1.0, 0.0, 1.0); // Green
            else if (tl.state == YELLOW && k == 2)
                onColor = color4(1.0, 1.0, 0.0, 1.0); // Yellow
            else
                onColor = offColor;

            // Update the colors of the light
            for (size_t i = 0; i < tl.lights[k].colors.size(); ++i)
            {
                tl.lights[k].colors[i] = onColor;
            }

            // Update the color buffer
            glBindBuffer(GL_ARRAY_BUFFER, tl.lights[k].buffer);
            glBufferSubData(GL_ARRAY_BUFFER,
                            tl.lights[k].points.size() * sizeof(point4),
                            tl.lights[k].colors.size() * sizeof(color4),
                            &tl.lights[k].colors[0]);

            // Draw the light
            drawObject(tl.lights[k], tl_mv);
        }
    }

    // Draw the car
    mat4 car_mv = model_view * Translate(carPosition + Angel::vec3(0.0, 0.5, 0.0)) * RotateY(carRotation + 90.0) * Scale(0.6, 0.6, 0.6);

    // Draw car body
    drawObject(carBody, car_mv);

    // Draw wheels
    for (const auto &transform : wheelTransforms)
    {
        mat4 wheel_mv = car_mv * transform * RotateX(90) * RotateY(-wheelRotation);
        drawObject(carWheel, wheel_mv);
    }

    glutSwapBuffers();
}

void drawObject(const Object &obj, const mat4 &mv)
{
    glUniformMatrix4fv(ModelView, 1, GL_TRUE, mv);

    glBindVertexArray(obj.vao);
    glDrawArrays(GL_TRIANGLES, 0, obj.numVertices);
}
