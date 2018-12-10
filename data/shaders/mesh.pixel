#version 330 core

#include "lights.glsl"

out vec4 Colour;

in struct FragmentData
{
	vec3 Position;
	vec4 Colour;
}
Fragment;

uniform vec3 ViewPosition;

uniform struct FogParameters
{
	vec3 Colour;
	float Distance;
	bool Enabled;
}
Fog;

uniform bool DrawNormals;

void main()
{
	vec3 xTangent = dFdx( Fragment.Position );
    vec3 yTangent = dFdy( Fragment.Position );
    vec3 fragmentNormal = normalize( cross( xTangent, yTangent ) );

	if( DrawNormals )
	{
		Colour = vec4( fragmentNormal, 1 );
		return;
	}

	vec3 toView = ViewPosition - Fragment.Position;
	float viewDistance = length( toView );
	vec3 viewDir = toView / viewDistance;

	float fogAmount = clamp( viewDistance / Fog.Distance, 0, 1 );
	fogAmount = fogAmount * fogAmount;

	if( Fog.Enabled && fogAmount >= 1 )
	{
		Colour = vec4( Fog.Colour, 1 );
		return;
	}

	vec4 finalColour = vec4( 0 );
	vec3 fragmentColour = Fragment.Colour.rgb;

	for( int i = 0; i < LightCount; ++i )
	{
		finalColour.rgb += ApplyLight( Lights[ i ], Material, fragmentColour, fragmentNormal, Fragment.Position, viewDir );
	}
	
	vec3 gamma = vec3( 1.0 / 2.2 );
	finalColour.rgb = pow( finalColour.rgb, gamma );
	finalColour.a = Fragment.Colour.a;

	if( Fog.Enabled )
	{
		finalColour = mix( finalColour, vec4( Fog.Colour, 1 ), fogAmount );
	}

	Colour = finalColour;
}
