#define MAX_LIGHTS 10

uniform struct LightData 
{
	int Type; // 0 = directional, 1 = point, 2 = spot
	vec4 Position;
	vec3 Direction;
	vec3 Colour;
	float CosInnerAngle;
	float CosOuterAngle;
	float Intensity;
	float AmbientStrength;
	float SpecularStrength;
	float Attenuation;
} 
Lights[ MAX_LIGHTS ];

uniform int LightCount;

uniform struct MaterialData
{
	float Shininess;
	float Specular;
	float Diffuse;
	float Ambient;
} 
Material;

//
// Apply a single light to the pixel. 
// Deals with both directional and point lights.
//
vec3 ApplyLight( LightData light, MaterialData material, vec3 fragColour, vec3 fragNormal, vec3 fragPosition, vec3 viewDir )
{
	vec3 lightColour = light.Colour * light.Intensity;
	vec3 ambientColour = light.AmbientStrength * material.Ambient * lightColour;
	vec3 lightDir;

	float attenuation = 1.0;

	if( light.Type == 0 )
	{
		// directional lights
		attenuation = 1.0; 
		lightDir = -light.Direction;
	}
	else if( light.Type == 1 )
	{
		// point lights
		vec3 fragmentToLight = light.Position.xyz - fragPosition;
		float distance = length( fragmentToLight );
		lightDir = fragmentToLight / distance;
		attenuation = 1.0 / (1.0 + light.Attenuation * distance * distance);
	}
	else if( light.Type == 2 )
	{
		// spot lights
		vec3 fragmentToLight = light.Position.xyz - fragPosition;
		float distance = length( fragmentToLight );
		lightDir = fragmentToLight / distance;

		float theta = dot( lightDir, -light.Direction );
		float epsilon = light.CosInnerAngle - light.CosOuterAngle;
		float intensity = clamp( (theta - light.CosOuterAngle) / epsilon, 0.0, 1.0 );
		
		attenuation = 1.0 / (1.0 + light.Attenuation * distance * distance);
		attenuation *= intensity;
	}

	if( attenuation < 0.0001 )
		return vec3( 0 );

	float diffAmount = max( dot( fragNormal, lightDir ), 0.0 );
	vec3 diffuseColour = diffAmount * material.Diffuse * lightColour;

	vec3 reflectDir = reflect( -lightDir, fragNormal );
	float specularAmount = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.Shininess );
	vec3 specularColour = light.SpecularStrength * material.Specular * specularAmount * lightColour;

	vec3 linearColour = attenuation * (specularColour + diffuseColour) + ambientColour;
	vec3 finalColour = linearColour * fragColour;
	return finalColour;
}