#version 330 core

layout( location = 0 ) in vec3 Position;

out struct VertexData
{
	vec3 Position;
	vec4 Colour;
} 
Vertex;

uniform vec4 ObjectColour;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
	Vertex.Position = (Model * vec4( Position, 1 )).xyz;
	Vertex.Colour = ObjectColour;

	gl_Position = Projection * View * Model * vec4( Position, 1 );
}