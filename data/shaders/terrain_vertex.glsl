#version 330 core

layout( location = 0 ) in vec3 Position;

out struct VertexData
{
	vec3 Position;
	vec4 Colour;
} Vertex;

uniform vec4 ObjectColour;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat3 NormalMatrix;

void main()
{
	Vertex.Position = vec3( Model * vec4( Position, 1 ) );
	Vertex.Colour = ObjectColour;

	gl_Position = Projection * View * Model * vec4( Position, 1 );
};