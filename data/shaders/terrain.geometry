#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in struct VertexData
{
	vec3 Position;
	vec4 Colour;
}
Vertex[];

out struct FragmentData
{
	vec3 Position;
	vec4 Colour;
	vec3 Barycentric;
}
Fragment;

void main()
{
	gl_Position = gl_in[0].gl_Position;
	Fragment.Position = Vertex[0].Position;
	Fragment.Colour = Vertex[0].Colour;
	Fragment.Barycentric = vec3( 1, 0, 0 );
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	Fragment.Position = Vertex[1].Position;
	Fragment.Colour = Vertex[1].Colour;
	Fragment.Barycentric = vec3( 0, 1, 0 );
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	Fragment.Position = Vertex[2].Position;
	Fragment.Colour = Vertex[2].Colour;
	Fragment.Barycentric = vec3( 0, 0, 1 );
	EmitVertex();

	EndPrimitive();
}