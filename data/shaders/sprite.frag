#version 330 core

out vec4 Colour;

in struct VertexData
{
	vec4 Colour;
	vec2 Position;
	vec2 UV;
} Vertex;

uniform sampler2D Texture;

void main()
{
	//Colour = texture(Texture, Vertex.UV) * Vertex.Colour;
	Colour = glm::vec4(1,0,0,1);
}
