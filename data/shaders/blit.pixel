#version 330 core

const float Near = 0.01;
const float Far = 2000;

in vec2 UV;

out vec4 Colour;

uniform sampler2D Texture;
uniform bool DrawDepth;

void main()
{
	if( DrawDepth )
	{
		float depth = texture( Texture, UV ).r;
		float inverse = (2 * Near) / depth;
		float linear = clamp( inverse / Far, 0, 1 );
    	Colour = vec4( vec3( linear ), 1 );
	}
	else
	{
		Colour = texture( Texture, UV );
	}
}