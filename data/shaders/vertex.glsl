#version 330 core

layout( location = 0 ) in vec3 Position;
layout( location = 1 ) in vec3 Normal;
layout( location = 2 ) in vec3 Wireframe;

out vec3 FragmentPosition;
flat out vec3 FragmentNormal;
out vec4 FragmentColour;

uniform vec4 ObjectColour;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat3 NormalMatrix;
uniform bool UseWireframe;

void main()
{
	FragmentPosition = vec3( Model * vec4( Position, 1 ) );
	FragmentNormal = NormalMatrix * Normal;

	if( UseWireframe )
	{
		FragmentColour = vec4( Wireframe, 1 );
	}
	else
	{
		FragmentColour = ObjectColour;
	}

	gl_Position = Projection * View * Model * vec4( Position, 1 );
};