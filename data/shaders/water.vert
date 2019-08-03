#version 330 core

#include "simplex.glsl"

layout( location = 0 ) in vec3 Position;

out struct FragmentData
{
	vec3 Position;
	vec4 Colour;
} 
Fragment;

uniform float Time;
uniform vec2 WaterOffset;
uniform vec2 WindVelocity;

uniform vec4 ObjectColour;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

const float Period = 5.0;
const float WaveHeight = 1.0;
const float Density = 10.0;

void main()
{
	vec3 world_pos = (Model * vec4( Position, 1 )).xyz;

	vec3 pos = Position;
	pos.y += WaveHeight * snoise( vec2( world_pos.x / Density + Time / Period, world_pos.z / Density + Time / Period ) );

	Fragment.Position = (Model * vec4( pos, 1 )).xyz;
	Fragment.Colour = ObjectColour;

	gl_Position = Projection * View * Model * vec4( pos, 1 );
}
