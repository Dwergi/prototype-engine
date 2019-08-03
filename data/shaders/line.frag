#version 330 core

out vec4 OutColour;

uniform vec4 Colour;

void main()
{
	OutColour = Colour;
}
