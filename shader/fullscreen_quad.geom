#version 330 core

/**
 * Emits a fullscreen quad
 * Make sure that the input is an empty VAO emitting a dummy GL_POINT
 */
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 UV;

void main()
{
    gl_Position = vec4( 1.0, 1.0, 0.5, 1.0 );
    UV = vec2( 1.0, 0.0 );
    EmitVertex();

    gl_Position = vec4(-1.0, 1.0, 0.5, 1.0 );
    UV = vec2( 0.0, 0.0 );
    EmitVertex();

    gl_Position = vec4( 1.0,-1.0, 0.5, 1.0 );
    UV = vec2( 1.0, 1.0 );
    EmitVertex();

    gl_Position = vec4(-1.0,-1.0, 0.5, 1.0 );
    UV = vec2( 0.0, 1.0 );
    EmitVertex();

    EndPrimitive();
}
