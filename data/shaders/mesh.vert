#version 330 core

layout( location = 0 ) in vec3 Position;

out struct FragmentData
{
	vec3 Position;
	vec4 Colour;
} 
Fragment;

uniform vec4 ObjectColour;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
	Fragment.Position = (Model * vec4( Position, 1 )).xyz;
	Fragment.Colour = ObjectColour;

	gl_Position = Projection * View * Model * vec4( Position, 1 );
}