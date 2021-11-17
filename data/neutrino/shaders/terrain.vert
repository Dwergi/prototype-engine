#version 330 core

layout( location = 0 ) in vec3 Position;

layout( location = 1 ) in vec4 ColourInstanced;
layout( location = 2 ) in mat4 TransformInstanced;

out struct VertexData
{
	vec3 Position;
	vec4 Colour;
} 
Vertex;

uniform mat4 View;
uniform mat4 Projection;

void main()
{
	vec4 pos_transformed = TransformInstanced * vec4(Position, 1);

	Vertex.Position = pos_transformed.xyz;
	Vertex.Colour = ColourInstanced;

	gl_Position = Projection * View * pos_transformed;
}