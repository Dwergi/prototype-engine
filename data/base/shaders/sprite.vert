#version 330 core

layout( location = 0 ) in vec2 Position;
layout( location = 1 ) in vec2 UV;

layout( location = 5 ) in vec4 ColourInstanced;
layout( location = 6 ) in vec2 UVOffsetInstanced;
layout( location = 7 ) in vec2 UVScaleInstanced;
layout( location = 8 ) in mat3 TransformInstanced;

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
	vec3 vertexPosition = TransformInstanced * vec3(Position, 1);

	gl_Position = Projection * View * vec4(vertexPosition, 1);
	gl_Position.z = 0;

	Vertex.UV = UV * UVScaleInstanced + UVOffsetInstanced;
	Vertex.Colour = ColourInstanced;
	Vertex.Position = vertexPosition.xy;
};