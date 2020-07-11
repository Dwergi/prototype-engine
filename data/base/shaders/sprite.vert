#version 330 core

layout( location = 0 ) in vec2 Position;
layout( location = 1 ) in vec2 UV;

layout( location = 5 ) in vec2 PositionInstanced;
layout( location = 6 ) in vec2 SizeInstanced;
layout( location = 7 ) in vec4 ColourInstanced;
layout( location = 8 ) in vec2 UVOffsetInstanced;
layout( location = 9 ) in vec2 UVScaleInstanced;

out struct VertexData
{
	vec4 Colour;
	vec2 Position;
	vec2 UV;
} Vertex;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
	vec2 vertexPosition = PositionInstanced + Position * SizeInstanced;

	gl_Position = Projection * vec4( vec3(vertexPosition, 0), 1 );

	Vertex.UV = UV * UVScaleInstanced + UVOffsetInstanced;
	Vertex.Colour = ColourInstanced;
	Vertex.Position = vertexPosition.xy;
};