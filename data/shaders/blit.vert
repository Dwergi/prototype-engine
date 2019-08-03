#version 330 core

layout( location = 0 ) in vec3 Position;

out vec2 UV;

void main()
{
    UV = (Position.xy + vec2( 1, 1 )) / 2.0;

    gl_Position = vec4( Position, 1 );
}