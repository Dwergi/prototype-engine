uniform struct WireframeData
{
	bool Enabled;
	vec3 Colour;
	float Width;
	vec3 EdgeColour;
	float EdgeWidth;
	float MaxDistance;
}
Wireframe;

//
// Calculate the colour of a pixel in wireframe mode by finding the minimum distance to an edge,
// and adding an edge of a different colour.
//
vec4 CalculateWireframe( vec3 barycentric, vec3 wireColour, float wireWidth, vec3 edgeColour, float edgeWidth, float viewDistance, float maxDistance )
{
	vec4 colour = vec4( 0.0 );

	if( viewDistance > maxDistance )
		return colour;

	wireWidth = mix( wireWidth, 0.0, viewDistance / maxDistance );
	edgeWidth = mix( edgeWidth, 0.0, viewDistance / maxDistance );

	vec3 d = fwidth( barycentric );
	vec3 tdist = smoothstep( vec3( 0.0 ), d * wireWidth, barycentric );
	float t = min( min( tdist.x, tdist.y ), tdist.z );

	float edge_ratio = edgeWidth / wireWidth;

	if( edge_ratio > 0 )
	{
		float edge_t = clamp( (t - (1.0 - edge_ratio)) / edge_ratio, 0.0, 1.0 );
		colour = mix( vec4( edgeColour, 1 ), vec4( 0.0 ), edge_t );
	}

	if( edge_ratio < 1 )
	{
		float wire_t = clamp( t / (1.0 - edge_ratio), 0.0, 1.0 );
		colour = mix( vec4( wireColour, 1 ), colour, wire_t );
	}

	return colour;
}
