#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;

uniform mat4 mvp;

out vec4 passColor;

void main()
{
   passColor = color;
   gl_Position = mvp * vec4(pos.xyz, 1.0);
}
