#version 330 core

layout( location = 0 ) in vec2 Position;
layout( location = 1 ) in vec2 UV;

layout( location = 5 ) in vec3 PositionInstanced;
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
	Vertex.Colour = ColourInstanced;

	vec3 right = vec3( View[0][0], View[1][0], View[2][0] );
	vec3 up = vec3( View[0][1], View[1][1], View[2][1] );

	vec3 vertexPosition = PositionInstanced + 
		right * Position.x * ScaleInstanced.x + 
		up * Position.y * ScaleInstanced.y;

	gl_Position = Projection * View * vec4( vertexPosition, 1 );

	Vertex.Position = gl_Position.xyz;
};