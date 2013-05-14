attribute vec4 gl_Color;

varying vec4 gl_FrontColor;
varying vec3 V;
varying vec3 N;

varying float depth;

void main() 
{
    gl_Position = ftransform();
    gl_FrontColor = gl_Color;
    gl_TexCoord[0] = gl_MultiTexCoord0;

    // lighting
    V = (gl_ModelViewMatrix * gl_Vertex).xyz;
    N = gl_NormalMatrix * gl_Normal;

    // depth
    depth = (-V.z - 2) / 400.0;
}
