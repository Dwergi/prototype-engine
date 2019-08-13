#version 330 core

layout( location = 0 ) in vec3 Position;

layout( location = 1 ) in vec4 ColourInstanced;
layout( location = 2 ) in mat4 TransformInstanced;

out struct FragmentData
{
	vec3 Position;
	vec4 Colour;
} 
Fragment;

uniform mat4 View;
uniform mat4 Projection;

void main()
{
	vec4 posTransformed = TransformInstanced * vec4( Position, 1 );

	Fragment.Position = posTransformed.xyz;
	Fragment.Colour = ColourInstanced;

	gl_Position = Projection * View * posTransformed;
}