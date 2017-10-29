#version 330 core

layout( location = 0 ) in vec3 Position;
layout( location = 1 ) in vec3 Normal;

out vec3 FragmentPosition;
out vec2 FragmentUV;
flat out vec3 FragmentNormal;
out vec4 FragmentColour;

uniform vec4 ObjectColour;
uniform mat4 Model;
uniform mat3 NormalMatrix;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
	FragmentPosition = vec3( Model * vec4( Position, 1 ) );
	FragmentNormal = NormalMatrix * Normal;
	FragmentColour = ObjectColour;

	gl_Position = Projection * View * Model * vec4( Position, 1 );
};