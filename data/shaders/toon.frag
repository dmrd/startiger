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
uniform sampler2D texture1;
	
void main()
{
	float intensity;
	float intensity2;
	vec4 color;
	vec3 n = normalize(normal);
	
	intensity = dot(vec3(gl_LightSource[0].position),n);

        intensity2 = ceil(intensity * 5.0)/5.0;
        color = vec4(intensity2, intensity2, intensity2, 1.0);
	
	gl_FragColor = gl_FrontMaterial.diffuse*color;//*texture2D(texture1, gl_TexCoord[0].st);
	//gl_FragColor = vec4(1, 0, 0, 1);
} 
