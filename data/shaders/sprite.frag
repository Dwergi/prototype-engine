#version 330 core

out vec4 Colour;

in struct VertexData
{
	vec4 Colour;
	vec2 Position;
	vec2 UV;
} Vertex;

uniform struct LightData
{
	vec2 Position;
	int Type;	// 1 is teleporter, 2 is player, 3 is exit, 4 is red, 5 is yellow
} Lights[15];

uniform int LightCount;
uniform sampler2D Texture;
uniform float Time;

const float MAX_INTENSITY = 0.9;
const float MIN_INTENSITY = 0.001;

void main()
{
	vec4 light_colour = vec4(0, 0, 0, 1);

	for (int i = 0; i < LightCount; ++i)
	{
		vec2 to_light = Lights[i].Position - Vertex.Position;
		float distance = length(to_light);
		float attenuation = 2.0 / (distance * distance);

		float intensity = 1;
		vec3 colour = vec3(1, 1, 1);

		if (Lights[i].Type == 1)
		{
			// teleporter
			intensity = (3.5 + sin(Time * 8) / 5) * 900;
			colour = vec3(0.7, 0.5, 0.4);
		}
		else if (Lights[i].Type == 2)
		{
			// player
		}
		else if (Lights[i].Type == 3)
		{
			// exit
			intensity = (1.5 + sin(Time * 2) / 15) * 300;
			colour = vec3(0.7, 0.6, 0.5);
		}
		else if (Lights[i].Type == 4)
		{
			// red
			intensity = (0.1 + cos((Time+i%3) * 8) / 8) * 900;
			colour = vec3(0, 0, 0);
		}
		else if (Lights[i].Type == 5)
		{
			// yellow
			intensity = (0.1 + cos((Time+i%3) * 8) / 8) * 900;
			colour = vec3(0, 0, 0);
		}

		attenuation *= intensity;
		attenuation = smoothstep(MIN_INTENSITY, MAX_INTENSITY, attenuation);

		light_colour += vec4(colour * attenuation, 1);
	}

	Colour = texture(Texture, Vertex.UV) * Vertex.Colour * light_colour;
}
