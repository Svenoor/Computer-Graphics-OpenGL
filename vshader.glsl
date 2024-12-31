#version 150

in vec4 vPosition;
in vec4 vColor;

uniform mat4 uModelView;
uniform mat4 uProjection;

out vec4 color;

void main()
{
    gl_Position = uProjection * uModelView * vPosition;
    color = vColor;
}

