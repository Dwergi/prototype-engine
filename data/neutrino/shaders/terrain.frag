#version 330 core
#extension GL_OES_standard_derivatives : enable

#include "lights.glsl"
#include "wireframe.glsl"

out vec4 Colour;

in struct FragmentData
{
	vec3 Position;
	vec4 Colour;
	vec3 Barycentric;
} Fragment;

#define MAX_HEIGHTS 10

uniform struct HeightLevel
{ 
	vec3 Colour;
	float Cutoff;
}
TerrainHeightLevels[ MAX_HEIGHTS ];

uniform float TerrainMaxHeight;
uniform int TerrainHeightCount;

uniform struct FogParameters
{
	vec3 Colour;
	float Distance;
	bool Enabled;
}
Fog;

uniform vec3 ViewPosition;
uniform bool DrawNormals;

//
// Gets the height colour based on TerrainHeightLevels.
// Iterates through until it reach a cutoff point that is larger than its height ratio,
// then mixes with the next colour based on the proportion of its height.
//
vec3 GetTerrainHeightColour( float heightPercent )
{
	int previous = 0;
	int current = 0;
	for( ; current < TerrainHeightCount; ++current )
	{
		if( heightPercent < TerrainHeightLevels[current].Cutoff )
			break;

		previous = current;
	}

	if( current == 0)
	{
		return TerrainHeightLevels[0].Colour;
	}

	float t = (heightPercent - TerrainHeightLevels[previous].Cutoff) / (TerrainHeightLevels[current].Cutoff - TerrainHeightLevels[previous].Cutoff);
	return mix( TerrainHeightLevels[previous].Colour, TerrainHeightLevels[current].Colour, t );
}

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
	if( TerrainHeightCount > 0 )
	{
		fragmentColour *= GetTerrainHeightColour( Fragment.Position.y / TerrainMaxHeight );
	}

	for( int i = 0; i < LightCount; ++i )
	{
		finalColour.rgb += ApplyLight( Lights[ i ], Material, fragmentColour, fragmentNormal, Fragment.Position, viewDir );
	}
	
	vec3 gamma = vec3( 1.0 / 2.2 );
	finalColour.rgb = pow( finalColour.rgb, gamma );
	finalColour.a = Fragment.Colour.a;

	if( Wireframe.Enabled )
	{
		vec4 wireColour = CalculateWireframe( Fragment.Barycentric, Wireframe.Colour, Wireframe.Width, 
			Wireframe.EdgeColour, Wireframe.EdgeWidth, viewDistance, Wireframe.MaxDistance );

		finalColour = mix( finalColour, wireColour, wireColour.a );
	}

	if( Fog.Enabled )
	{
		finalColour = mix( finalColour, vec4( Fog.Colour, 1 ), fogAmount );
	}

	Colour = finalColour;
}
