/***********************************************************
*   Simple GLSL demo code for COS426 Computer Graphics     *
*                           ****                           *
*   Most is borrowed from                                  *
*   http://www.lighthouse3d.com/opengl/glsl/               *
*                           ****                           *
*   Dependencies: glew, glut, gl                           *
*                           ****                           *
*   Pulled together by Aleksey Boyko(aboyko@princeton.edu) *
************************************************************/

varying vec3 normal;
	
void main()
{
	normal = gl_NormalMatrix * gl_Normal;
        gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
	gl_FrontColor = gl_Color;
	gl_BackColor = vec4(0.0,0.0,0.0,1.0);
} 
