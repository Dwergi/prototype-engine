#version 330 core

layout ( location = 0 ) in vec3 Position;

uniform mat4 ModelViewProjection;

void main()
{
	gl_Position = ModelViewProjection * vec4( Position, 1 );
}