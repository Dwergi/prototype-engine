#version 330 core
out vec4 color;

in vec3 FragmentPosition;
flat in vec3 FragmentNormal;
in vec4 FragmentColour;

uniform vec3 ViewPosition;

#define MAX_LIGHTS 10

uniform struct Light {
	vec4 Position;
	vec3 Colour;
	float Intensity;
	float AmbientStrength;
	float SpecularStrength;
	float Attenuation;
} Lights[ MAX_LIGHTS ];

uniform int LightCount;

vec3 ApplyLight( Light light, vec3 fragColour, vec3 fragNormal, vec3 fragPosition, vec3 viewDir )
{
	vec3 lightColour = light.Colour * light.Intensity;
	vec3 ambientColour = light.AmbientStrength * lightColour;
	vec3 lightDir;

	float attenuation = 1.0;

	// directional lights
	if( light.Position.w == 0.0 )
	{
		lightDir = normalize( light.Position.xyz );
		attenuation = 0.0; 
	}
	else
	{
		vec3 fragmentToLight = light.Position.xyz - fragPosition;
		lightDir = normalize( fragmentToLight );
		float distanceToLight = length( fragmentToLight );
		attenuation = 1.0 / (1.0 + light.Attenuation * (distanceToLight * distanceToLight) );
	}

	float diffAmount = max( dot( fragNormal, lightDir ), 0.0 );
	vec3 diffuseColour = diffAmount * lightColour;

	vec3 reflectDir = reflect( -lightDir, fragNormal );
	float specularAmount = pow( max( dot( viewDir, reflectDir ), 0.0 ), 32 );
	vec3 specularColour = light.SpecularStrength * specularAmount * lightColour;

	vec3 linearColour = attenuation * (specularColour + diffuseColour) + ambientColour;
	vec3 finalColour = linearColour * fragColour;
	return finalColour;
}

void main()
{
	vec3 normal = normalize( FragmentNormal );
	vec3 viewDir = normalize( ViewPosition - FragmentPosition );

	vec3 finalColour = vec3( 0 );
	for( int i = 0; i < LightCount; ++i )
	{
		finalColour += ApplyLight( Lights[ i ], FragmentColour.rgb, normal, FragmentPosition, viewDir );
	}
	//color.rgb = (vec3(1,1,1) + FragmentNormal) * 0.5;
	color.rgb = finalColour;
	color.a = FragmentColour.a;
};
