#include "objects.h"
#include "Angel.h"
#include "display.h"
#include "globals.h"
// Shader variables
GLuint program;
GLuint ModelView, Projection;
GLuint vao[NumObjects];
GLuint buffer[NumObjects];
GLuint vPosition, vColor;

// Transformation matrices
mat4 model_view;
mat4 projection;

// Traffic light dimensions
const float poleHeight = 2.0f;
const float lightBoxHeight = 1.5f;
const float connectorPoleHeight = 3.0f;
const float connectorPoleWidth = 0.1f;

// External variables for objects
std::vector<TrafficLight> trafficLights;
Object carBody;
Object carWheel;
std::vector<mat4> wheelTransforms;
std::vector<Object> buildings;
Object ground;
Object roads;

// Car color
color4 carBodyColor = color4(0.0, 0.0, 1.0, 1.0); // Blue
color4 markerColor = color4(1.0, 0.0, 0.0, 1.0);  // Red color for the marker

// Wheel color
color4 wheelColor = color4(0.0, 0.0, 0.0, 1.0); // Black

// Ground color
color4 groundColor = color4(0.3, 0.7, 0.3, 1.0); // Green

// Road color
color4 roadColor = color4(0.2, 0.2, 0.2, 1.0); // Dark gray

// Grid parameters
const int gridSize = 10;
const float blockSize = 10.0f;
const float roadWidth = 2.0f;

// Car body geometry
point4 carBodyVertices[] = {
    // Lower body
    point4(-1.5, 0.0, 0.8, 1.0),  // right side down
    point4(1.5, 0.0, 0.8, 1.0),   // front side right up
    point4(1.5, 1.0, 1.0, 1.0),   // front side right down
    point4(-1.5, 1.0, 0.8, 1.0),  // right side up
    point4(-1.5, 0.0, -0.8, 1.0), // left side down
    point4(1.5, 0.0, -1.0, 1.0),  // front side left down
    point4(1.5, 1.0, -1.0, 1.0),  // front side left up
    point4(-1.5, 1.0, -0.8, 1.0), // left side up
    // Upper body (roof)
    point4(-1.5, 1.0, 0.8, 1.0),  // 8
    point4(1.0, 1.0, 0.8, 1.0),   // 9
    point4(1.0, 1.8, 0.0, 1.0),   // 10
    point4(-1.0, 1.8, 0.0, 1.0),  // 11
    point4(-1.5, 1.0, -0.8, 1.0), // 12
    point4(1.0, 1.0, -0.8, 1.0)   // 13
};

// Indices for drawing the car body using triangles
GLubyte carBodyIndices[] = {
    // Lower body
    0, 1, 2,
    2, 3, 0,
    1, 5, 6,
    6, 2, 1,
    5, 4, 7,
    7, 6, 5,
    4, 0, 3,
    3, 7, 4,
    // Upper body (roof)
    8, 9, 10,
    10, 11, 8,
    9, 13, 10,
    12, 8, 11,
    12, 11, 10,
    10, 13, 12,
    // Front and back
    3, 2, 9,
    9, 8, 3,
    7, 6, 13,
    13, 12, 7};

// Function definitions
void createCar()
{
    // Car body
    {
        int numVertices = sizeof(carBodyIndices) / sizeof(GLubyte);

        // Assign data to carBody object
        carBody.points.resize(numVertices);
        carBody.colors.resize(numVertices);
        for (int i = 0; i < numVertices; ++i)
        {
            carBody.points[i] = carBodyVertices[carBodyIndices[i]];
            carBody.colors[i] = carBodyColor;
        }
        carBody.numVertices = numVertices;

        // Create VAO and buffer for the car body
        glGenVertexArrays(1, &carBody.vao);
        glGenBuffers(1, &carBody.buffer);

        glBindVertexArray(carBody.vao);
        glBindBuffer(GL_ARRAY_BUFFER, carBody.buffer);
        glBufferData(GL_ARRAY_BUFFER, carBody.points.size() * sizeof(point4) + carBody.colors.size() * sizeof(color4), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, carBody.points.size() * sizeof(point4), &carBody.points[0]);
        glBufferSubData(GL_ARRAY_BUFFER, carBody.points.size() * sizeof(point4), carBody.colors.size() * sizeof(color4), &carBody.colors[0]);

        // Set up vertex arrays
        glEnableVertexAttribArray(vPosition);
        glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

        glEnableVertexAttribArray(vColor);
        glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(carBody.points.size() * sizeof(point4)));
    }

    // Wheels
    {
        // Define wheel geometry (cylinder)
        const int NumWheelSlices = 20;
        float radius = 0.4f;
        float width = 0.2f;
        std::vector<point4> wheelVertices;
        std::vector<color4> wheelColors;

        // Generate vertices for the wheel
        for (int i = 0; i < NumWheelSlices; ++i)
        {
            float theta = (2.0f * M_PI * i) / NumWheelSlices;
            float nextTheta = (2.0f * M_PI * (i + 1)) / NumWheelSlices;

            // Side face
            wheelVertices.push_back(point4(radius * cos(theta), width, radius * sin(theta), 1.0));
            wheelVertices.push_back(point4(radius * cos(theta), -width, radius * sin(theta), 1.0));
            wheelVertices.push_back(point4(radius * cos(nextTheta), -width, radius * sin(nextTheta), 1.0));

            wheelVertices.push_back(point4(radius * cos(theta), width, radius * sin(theta), 1.0));
            wheelVertices.push_back(point4(radius * cos(nextTheta), -width, radius * sin(nextTheta), 1.0));
            wheelVertices.push_back(point4(radius * cos(nextTheta), width, radius * sin(nextTheta), 1.0));

            // Top face
            wheelVertices.push_back(point4(0.0, width, 0.0, 1.0));
            wheelVertices.push_back(point4(radius * cos(nextTheta), width, radius * sin(nextTheta), 1.0));
            wheelVertices.push_back(point4(radius * cos(theta), width, radius * sin(theta), 1.0));

            // Bottom face
            wheelVertices.push_back(point4(0.0, -width, 0.0, 1.0));
            wheelVertices.push_back(point4(radius * cos(theta), -width, radius * sin(theta), 1.0));
            wheelVertices.push_back(point4(radius * cos(nextTheta), -width, radius * sin(nextTheta), 1.0));

            // Add colors for the side faces
            for (int j = 0; j < 6; ++j)
            {
                if (i == 0) // Mark the first segment
                    wheelColors.push_back(markerColor);
                else
                    wheelColors.push_back(wheelColor);
            }

            // Add colors for the top and bottom faces
            for (int j = 0; j < 6; ++j)
            {
                wheelColors.push_back(wheelColor);
            }
        }

        int numVertices = wheelVertices.size();

        // Assign data to carWheel object
        carWheel.points = wheelVertices;
        carWheel.colors = wheelColors;
        carWheel.numVertices = numVertices;

        // Create VAO and buffer for the wheel
        glGenVertexArrays(1, &carWheel.vao);
        glGenBuffers(1, &carWheel.buffer);

        glBindVertexArray(carWheel.vao);
        glBindBuffer(GL_ARRAY_BUFFER, carWheel.buffer);
        glBufferData(GL_ARRAY_BUFFER, carWheel.points.size() * sizeof(point4) + carWheel.colors.size() * sizeof(color4), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, carWheel.points.size() * sizeof(point4), &carWheel.points[0]);
        glBufferSubData(GL_ARRAY_BUFFER, carWheel.points.size() * sizeof(point4), carWheel.colors.size() * sizeof(color4), &carWheel.colors[0]);

        // Set up vertex arrays
        glEnableVertexAttribArray(vPosition);
        glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

        glEnableVertexAttribArray(vColor);
        glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(carWheel.points.size() * sizeof(point4)));

        // Wheel positions relative to the car body
        wheelTransforms.resize(4);
        wheelTransforms[0] = Translate(-1.0, 0.0, 0.8);  // Front left
        wheelTransforms[1] = Translate(1.0, 0.0, 0.8);   // Front right
        wheelTransforms[2] = Translate(-1.0, 0.0, -0.8); // Back left
        wheelTransforms[3] = Translate(1.0, 0.0, -0.8);  // Back right
    }
}

void createBuildings()
{
    // Create buildings in the blocks between roads
    float halfGrid = gridSize / 2.0f;
    int buildingCount = 0;

    for (int i = -gridSize; i <= gridSize; ++i)
    {
        for (int j = -gridSize; j <= gridSize; ++j)
        {
            // Skip roads
            if (i % 2 == 0 || j % 2 == 0)
                continue;

            Object building;
            // Random color for building
            color4 buildingColor = color4(
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                1.0);

            // Random height between 2 and 5 units
            float height = 2.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (5.0f - 2.0f)));

            // Building base size
            float size = 1.5f;

            // Define shape for the building
            point4 vertices[] = {
                // Cube base
                point4(-size, 0.0, size, 1.0),     // 0
                point4(size, 0.0, size, 1.0),      // 1
                point4(size, height, size, 1.0),   // 2
                point4(-size, height, size, 1.0),  // 3
                point4(-size, 0.0, -size, 1.0),    // 4
                point4(size, 0.0, -size, 1.0),     // 5
                point4(size, height, -size, 1.0),  // 6
                point4(-size, height, -size, 1.0), // 7
                // Pyramid top
                point4(0.0, height + 2.0, 0.0, 1.0) // 8
            };

            // Indices for drawing the building using triangles
            GLubyte indices[] = {
                // Cube base
                0, 1, 2,
                2, 3, 0,
                1, 5, 6,
                6, 2, 1,
                5, 4, 7,
                7, 6, 5,
                4, 0, 3,
                3, 7, 4,
                // Top pyramid
                3, 2, 8,
                2, 6, 8,
                6, 7, 8,
                7, 3, 8};

            int numVertices = sizeof(indices) / sizeof(GLubyte);

            // Assign data to building object
            building.points.resize(numVertices);
            building.colors.resize(numVertices);
            for (int k = 0; k < numVertices; ++k)
            {
                building.points[k] = vertices[indices[k]];
                building.colors[k] = buildingColor;
            }
            building.numVertices = numVertices;

            // Create VAO and buffer for the building
            glGenVertexArrays(1, &building.vao);
            glGenBuffers(1, &building.buffer);

            glBindVertexArray(building.vao);
            glBindBuffer(GL_ARRAY_BUFFER, building.buffer);
            glBufferData(GL_ARRAY_BUFFER, building.points.size() * sizeof(point4) + building.colors.size() * sizeof(color4), NULL, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, building.points.size() * sizeof(point4), &building.points[0]);
            glBufferSubData(GL_ARRAY_BUFFER, building.points.size() * sizeof(point4), building.colors.size() * sizeof(color4), &building.colors[0]);

            // Set up vertex arrays
            glEnableVertexAttribArray(vPosition);
            glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

            glEnableVertexAttribArray(vColor);
            glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(building.points.size() * sizeof(point4)));

            // Set building position
            float x = i * (blockSize / 2.0f);
            float z = j * (blockSize / 2.0f);

            building.modelMatrix = Translate(x, 0.0, z);

            buildings.push_back(building);
            buildingCount++;

            if (buildingCount >= 70)
                return;
        }
    }
}

void createGround()
{
    point4 groundPoints[6];
    color4 groundColors[6];

    float size = gridSize * blockSize;

    // Ground plane vertices (two triangles)
    groundPoints[0] = point4(-size, 0.0, -size, 1.0);
    groundPoints[1] = point4(size, 0.0, -size, 1.0);
    groundPoints[2] = point4(size, 0.0, size, 1.0);
    groundPoints[3] = point4(-size, 0.0, -size, 1.0);
    groundPoints[4] = point4(size, 0.0, size, 1.0);
    groundPoints[5] = point4(-size, 0.0, size, 1.0);

    // Ground color
    for (int i = 0; i < 6; ++i)
    {
        groundColors[i] = groundColor;
    }

    ground.points.assign(groundPoints, groundPoints + 6);
    ground.colors.assign(groundColors, groundColors + 6);
    ground.numVertices = 6;

    // Create VAO and buffer for the ground
    glGenVertexArrays(1, &ground.vao);
    glGenBuffers(1, &ground.buffer);

    glBindVertexArray(ground.vao);
    glBindBuffer(GL_ARRAY_BUFFER, ground.buffer);
    glBufferData(GL_ARRAY_BUFFER, ground.points.size() * sizeof(point4) + ground.colors.size() * sizeof(color4), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, ground.points.size() * sizeof(point4), &ground.points[0]);
    glBufferSubData(GL_ARRAY_BUFFER, ground.points.size() * sizeof(point4), ground.colors.size() * sizeof(color4), &ground.colors[0]);

    // Set up vertex arrays
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(ground.points.size() * sizeof(point4)));
}

// Create the roads
void createRoads()
{
    // Create roads along the grid lines
    float size = gridSize * blockSize;
    std::vector<point4> roadPoints;
    std::vector<color4> roadColors;

    // Vertical roads
    for (int i = -gridSize; i <= gridSize; ++i)
    {
        if (i % 2 != 0)
            continue;

        float x = i * (blockSize / 2.0f);
        float halfSize = size;

        roadPoints.push_back(point4(x - roadWidth, 0.01, -halfSize, 1.0));
        roadPoints.push_back(point4(x + roadWidth, 0.01, -halfSize, 1.0));
        roadPoints.push_back(point4(x + roadWidth, 0.01, halfSize, 1.0));

        roadPoints.push_back(point4(x - roadWidth, 0.01, -halfSize, 1.0));
        roadPoints.push_back(point4(x + roadWidth, 0.01, halfSize, 1.0));
        roadPoints.push_back(point4(x - roadWidth, 0.01, halfSize, 1.0));
    }

    // Horizontal roads
    for (int j = -gridSize; j <= gridSize; ++j)
    {
        if (j % 2 != 0)
            continue;

        float z = j * (blockSize / 2.0f);
        float halfSize = size;

        roadPoints.push_back(point4(-halfSize, 0.01, z - roadWidth, 1.0));
        roadPoints.push_back(point4(halfSize, 0.01, z - roadWidth, 1.0));
        roadPoints.push_back(point4(halfSize, 0.01, z + roadWidth, 1.0));

        roadPoints.push_back(point4(-halfSize, 0.01, z - roadWidth, 1.0));
        roadPoints.push_back(point4(halfSize, 0.01, z + roadWidth, 1.0));
        roadPoints.push_back(point4(-halfSize, 0.01, z + roadWidth, 1.0));
    }

    int numVertices = roadPoints.size();
    roadColors.resize(numVertices, roadColor);

    roads.points = roadPoints;
    roads.colors = roadColors;
    roads.numVertices = numVertices;

    // Create VAO and buffer for the roads
    glGenVertexArrays(1, &roads.vao);
    glGenBuffers(1, &roads.buffer);

    glBindVertexArray(roads.vao);
    glBindBuffer(GL_ARRAY_BUFFER, roads.buffer);
    glBufferData(GL_ARRAY_BUFFER, roads.points.size() * sizeof(point4) + roads.colors.size() * sizeof(color4), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, roads.points.size() * sizeof(point4), &roads.points[0]);
    glBufferSubData(GL_ARRAY_BUFFER, roads.points.size() * sizeof(point4), roads.colors.size() * sizeof(color4), &roads.colors[0]);

    // Set up vertex arrays
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(roads.points.size() * sizeof(point4)));
}

// Create the traffic lights
void createTrafficLights()
{
    // Define cube vertices for pole and box
    point4 cubeVertices[] = {
        point4(-0.1, 0.0, 0.1, 1.0),  // 0
        point4(0.1, 0.0, 0.1, 1.0),   // 1
        point4(0.1, 1.0, 0.1, 1.0),   // 2
        point4(-0.1, 1.0, 0.1, 1.0),  // 3
        point4(-0.1, 0.0, -0.1, 1.0), // 4
        point4(0.1, 0.0, -0.1, 1.0),  // 5
        point4(0.1, 1.0, -0.1, 1.0),  // 6
        point4(-0.1, 1.0, -0.1, 1.0)  // 7
    };

    GLubyte cubeIndicesWithoutFrontFace[] = {
        // Right face
        1, 5, 6, 6, 2, 1,
        // Back face
        5, 4, 7, 7, 6, 5,
        // Left face
        4, 0, 3, 3, 7, 4,
        // Top face
        3, 2, 6, 6, 7, 3,
        // Bottom face
        4, 5, 1, 1, 0, 4};

    int numCubeVerticesWithoutFrontFace = sizeof(cubeIndicesWithoutFrontFace) / sizeof(GLubyte);

    // Create 5 traffic lights
    for (int i = 0; i < 5; ++i)
    {
        TrafficLight tl;
        tl.state = RED;
        tl.stateTime = 0.0f;

        // Position the traffic light at some intersection
        int gridX = -2 + i * 2; // Adjusted to place them near the center
        int gridZ = 0;          // Place them along the z=0 line

        float x = gridX * (blockSize / 2.0f) + roadWidth;
        float z = gridZ * (blockSize / 2.0f) + roadWidth;

        tl.modelMatrix = Translate(x, poleHeight + 1, z) * RotateZ(90.0f);

        // Create the main pole (base)
        {
            tl.base.points.resize(numCubeVerticesWithoutFrontFace);
            tl.base.colors.resize(numCubeVerticesWithoutFrontFace);
            for (int j = 0; j < numCubeVerticesWithoutFrontFace; ++j)
            {
                point4 p = cubeVertices[cubeIndicesWithoutFrontFace[j]];
                p.y *= poleHeight; // Scale to poleHeight
                tl.base.points[j] = p;
                tl.base.colors[j] = color4(0.8f, 0.2f, 0.2f, 1.0f); // Dark gray
            }
            tl.base.numVertices = numCubeVerticesWithoutFrontFace;

            // Create VAO and buffer for the pole
            glGenVertexArrays(1, &tl.base.vao);
            glGenBuffers(1, &tl.base.buffer);

            glBindVertexArray(tl.base.vao);
            glBindBuffer(GL_ARRAY_BUFFER, tl.base.buffer);
            glBufferData(GL_ARRAY_BUFFER,
                         tl.base.points.size() * sizeof(point4) + tl.base.colors.size() * sizeof(color4),
                         NULL, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, tl.base.points.size() * sizeof(point4), &tl.base.points[0]);
            glBufferSubData(GL_ARRAY_BUFFER, tl.base.points.size() * sizeof(point4),
                            tl.base.colors.size() * sizeof(color4), &tl.base.colors[0]);

            // Set up vertex arrays
            glEnableVertexAttribArray(vPosition);
            glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

            glEnableVertexAttribArray(vColor);
            glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(tl.base.points.size() * sizeof(point4)));
        }

        // Create the light box
        {
            tl.lightBox.points.resize(numCubeVerticesWithoutFrontFace);
            tl.lightBox.colors.resize(numCubeVerticesWithoutFrontFace);
            for (int j = 0; j < numCubeVerticesWithoutFrontFace; ++j)
            {
                point4 p = cubeVertices[cubeIndicesWithoutFrontFace[j]];
                p.y *= lightBoxHeight; // Scale to lightBoxHeight
                p.y += poleHeight;     // Position on top of pole
                p.x *= 0.5f;           // Make box thinner
                p.z *= 0.5f;           // Reduce depth
                tl.lightBox.points[j] = p;
                tl.lightBox.colors[j] = color4(0.1f, 0.1f, 0.1f, 1.0f); // Darker gray
            }
            tl.lightBox.numVertices = numCubeVerticesWithoutFrontFace;

            // Create VAO and buffer for the light box
            glGenVertexArrays(1, &tl.lightBox.vao);
            glGenBuffers(1, &tl.lightBox.buffer);

            glBindVertexArray(tl.lightBox.vao);
            glBindBuffer(GL_ARRAY_BUFFER, tl.lightBox.buffer);
            glBufferData(GL_ARRAY_BUFFER,
                         tl.lightBox.points.size() * sizeof(point4) + tl.lightBox.colors.size() * sizeof(color4),
                         NULL, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, tl.lightBox.points.size() * sizeof(point4), &tl.lightBox.points[0]);
            glBufferSubData(GL_ARRAY_BUFFER, tl.lightBox.points.size() * sizeof(point4),
                            tl.lightBox.colors.size() * sizeof(color4), &tl.lightBox.colors[0]);

            // Set up vertex arrays
            glEnableVertexAttribArray(vPosition);
            glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

            glEnableVertexAttribArray(vColor);
            glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(tl.lightBox.points.size() * sizeof(point4)));
        }

        // Create the connector pole
        {
            // Define cube vertices for the connector pole (similar to the main pole)
            point4 connectorVertices[] = {
                point4(-connectorPoleWidth, 0.0, connectorPoleWidth, 1.0),  // 0
                point4(connectorPoleWidth, 0.0, connectorPoleWidth, 1.0),   // 1
                point4(connectorPoleWidth, 1.0, connectorPoleWidth, 1.0),   // 2
                point4(-connectorPoleWidth, 1.0, connectorPoleWidth, 1.0),  // 3
                point4(-connectorPoleWidth, 0.0, -connectorPoleWidth, 1.0), // 4
                point4(connectorPoleWidth, 0.0, -connectorPoleWidth, 1.0),  // 5
                point4(connectorPoleWidth, 1.0, -connectorPoleWidth, 1.0),  // 6
                point4(-connectorPoleWidth, 1.0, -connectorPoleWidth, 1.0)  // 7
            };

            GLubyte connectorIndicesWithoutFrontFace[] = {
                // Right face
                1, 5, 6, 6, 2, 1,
                // Back face
                5, 4, 7, 7, 6, 5,
                // Left face
                4, 0, 3, 3, 7, 4,
                // Top face
                3, 2, 6, 6, 7, 3,
                // Bottom face
                4, 5, 1, 1, 0, 4};

            int numConnectorVertices = sizeof(connectorIndicesWithoutFrontFace) / sizeof(GLubyte);

            // Assign data to connectorPole object
            tl.connectorPole.points.resize(numConnectorVertices);
            tl.connectorPole.colors.resize(numConnectorVertices);
            for (int j = 0; j < numConnectorVertices; ++j)
            {
                point4 p = connectorVertices[connectorIndicesWithoutFrontFace[j]];
                p.y *= connectorPoleHeight; // Scale to connectorPoleHeight
                tl.connectorPole.points[j] = p;
                tl.connectorPole.colors[j] = color4(0.8f, 0.2f, 0.2f, 1.0f); // Dark gray color
            }
            tl.connectorPole.numVertices = numConnectorVertices;

            // Create VAO and buffer for the connector pole
            glGenVertexArrays(1, &tl.connectorPole.vao);
            glGenBuffers(1, &tl.connectorPole.buffer);

            glBindVertexArray(tl.connectorPole.vao);
            glBindBuffer(GL_ARRAY_BUFFER, tl.connectorPole.buffer);
            glBufferData(GL_ARRAY_BUFFER,
                         tl.connectorPole.points.size() * sizeof(point4) + tl.connectorPole.colors.size() * sizeof(color4),
                         NULL, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, tl.connectorPole.points.size() * sizeof(point4), &tl.connectorPole.points[0]);
            glBufferSubData(GL_ARRAY_BUFFER, tl.connectorPole.points.size() * sizeof(point4),
                            tl.connectorPole.colors.size() * sizeof(color4), &tl.connectorPole.colors[0]);

            // Set up vertex arrays
            glEnableVertexAttribArray(vPosition);
            glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

            glEnableVertexAttribArray(vColor);
            glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(tl.connectorPole.points.size() * sizeof(point4)));

            // Position the connector pole
            tl.connectorPole.modelMatrix = RotateZ(-90.0f) * Translate(0.0f, -connectorPoleHeight, 0.0f);
        }

        // Create the lights
        for (int k = 0; k < 3; ++k)
        {
            // Create small squares representing the lights
            point4 lightVertices[] = {
                point4(-0.15f, -0.25f, 0.051f, 1.0),
                point4(0.15f, -0.25f, 0.051f, 1.0),
                point4(0.15f, 0.25f, 0.051f, 1.0),
                point4(-0.15f, 0.25f, 0.051f, 1.0)};

            GLubyte lightIndices[] = {0, 1, 2, 2, 3, 0};

            int numLightVertices = 6;

            tl.lights[k].points.resize(numLightVertices);
            tl.lights[k].colors.resize(numLightVertices);
            for (int j = 0; j < numLightVertices; ++j)
            {
                point4 p = lightVertices[lightIndices[j]];

                // Position the lights inside the light box
                float boxStartY = poleHeight;          // Bottom of the light box
                float boxHeight = lightBoxHeight;      // Height of the light box
                float lightSpacing = boxHeight / 3.0f; // Space for each light

                // Center each light within its allocated space
                p.y += boxStartY + lightSpacing * (2 - k) + lightSpacing / 2.0f - boxHeight / 2.0f;

                tl.lights[k].points[j] = p;
                tl.lights[k].colors[j] = color4(0.1f, 0.1f, 0.1f, 1.0f); // Start with off color
            }
            tl.lights[k].numVertices = numLightVertices;

            // Create VAO and buffer for the light
            glGenVertexArrays(1, &tl.lights[k].vao);
            glGenBuffers(1, &tl.lights[k].buffer);

            glBindVertexArray(tl.lights[k].vao);
            glBindBuffer(GL_ARRAY_BUFFER, tl.lights[k].buffer);
            glBufferData(GL_ARRAY_BUFFER,
                         tl.lights[k].points.size() * sizeof(point4) + tl.lights[k].colors.size() * sizeof(color4),
                         NULL, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, tl.lights[k].points.size() * sizeof(point4), &tl.lights[k].points[0]);
            glBufferSubData(GL_ARRAY_BUFFER, tl.lights[k].points.size() * sizeof(point4),
                            tl.lights[k].colors.size() * sizeof(color4), &tl.lights[k].colors[0]);

            // Set up vertex arrays
            glEnableVertexAttribArray(vPosition);
            glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

            glEnableVertexAttribArray(vColor);
            glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(tl.lights[k].points.size() * sizeof(point4)));
        }

        // Add the traffic light to the vector
        trafficLights.push_back(tl);
    }
}

bool checkCollision(Angel::vec3 newPosition)
{
    // Check if the new position is within the road grid
    float halfGridSize = gridSize * (blockSize / 2.0f);
    if (newPosition.x < -halfGridSize || newPosition.x > halfGridSize ||
        newPosition.z < -halfGridSize || newPosition.z > halfGridSize)
    {
        return true; // Collision with boundary
    }

    // Check if the new position is on the road
    int gridX = static_cast<int>(round(newPosition.x / (blockSize / 2.0f)));
    int gridZ = static_cast<int>(round(newPosition.z / (blockSize / 2.0f)));

    // Car can only be on even grid lines (roads)
    if ((gridX) % 2 != 0 && (gridZ) % 2 != 0)
    {
        return true; // Collision with building area
    }

    // Check for collision with traffic light connector poles
    for (const auto &tl : trafficLights)
    {
        // Extract the position of the traffic light
        vec4 tlPosition = tl.modelMatrix * vec4(0.0, 0.0, 0.0, 1.0);
        float poleX = tlPosition.x;
        float poleZ = tlPosition.z;

        // Define the bounding box around the connector pole
        float poleHalfWidth = connectorPoleWidth / 2.0f + 0.97f;
        float minX = poleX - poleHalfWidth;
        float maxX = poleX + poleHalfWidth;
        float minZ = poleZ - poleHalfWidth;
        float maxZ = poleZ + poleHalfWidth;

        // Check if the new position intersects the bounding box
        if (newPosition.x >= minX && newPosition.x <= maxX &&
            newPosition.z >= minZ && newPosition.z <= maxZ)
        {
            return true; // Collision with traffic light connector pole
        }
    }

    return false; // No collision
}