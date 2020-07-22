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
	vec3 Colour;
	float Intensity;
} Lights[15];

uniform int LightCount;
uniform sampler2D Texture;
uniform float Time;

const float MAX_INTENSITY = 0.9;
const float MIN_INTENSITY = 0.001;

void main()
{
	vec4 light_colour = vec4(1, 1, 1, 1);

	for (int i = 0; i < LightCount; ++i)
	{
		vec2 to_light = Lights[i].Position - Vertex.Position;
		float distance = length(to_light);
		float attenuation = 2.0 / (distance * distance);

		float intensity = Lights[i].Intensity;
		vec3 colour = Lights[i].Colour;

		attenuation *= intensity;
		attenuation = smoothstep(MIN_INTENSITY, MAX_INTENSITY, attenuation);

		light_colour += vec4(colour * attenuation, 1);
	}

	Colour = texture(Texture, Vertex.UV) * Vertex.Colour * light_colour;
}
