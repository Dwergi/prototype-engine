#version 330 core

layout( location = 0 ) in vec3 Position;
layout( location = 1 ) in vec2 UV;

layout( location = 5 ) in vec2 PositionInstanced;
layout( location = 6 ) in vec2 ScaleInstanced;
layout( location = 7 ) in vec4 ColourInstanced;

out struct VertexData
{
	vec4 Colour;
	vec3 Position;
} Vertex;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
	vec3 vertexPosition = PositionInstance + Position;

	Vertex.Colour = ColourInstanced;
	gl_Position = Projection * View * vec4( vertexPosition, 1 );

	Vertex.Position = gl_Position.xyz;
};