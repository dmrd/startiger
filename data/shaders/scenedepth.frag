varying vec3 V;
varying vec3 N;
varying float depth;

attribute vec4 gl_Color;

uniform sampler2D diffuseMap;

void main() 
{
    vec4 dsum = vec4(0, 0, 0, 0);
    vec4 ssum = vec4(0, 0, 0, 0);
    N = normalize(N);
    vec3 P = V;
    V = normalize(V);
    vec3 R = reflect(V, N);

    // lights
    for (int i = 0; i < 3; ++i)
    {
        vec3 L;
        float f = 1;

        if (gl_LightSource[i].position.w == 0.0)
        {
            L = normalize(vec3(gl_LightSource[i].position));
        }
        else
        {
            L = vec3(gl_LightSource[i].position - P);
            float d = length(L);
            L /= d;

            f = 1.0 / (gl_LightSource[i].constantAttenuation
                    + gl_LightSource[i].linearAttenuation * d
                    + gl_LightSource[i].quadraticAttenuation * d*d);
        }

        dsum += f * gl_LightSource[i].diffuse * max(dot(L, N), 0.0);
        ssum += f * gl_LightSource[i].specular * pow(max(dot(R, L), 0.0), 3);
    }

    vec4 tex = texture2D(diffuseMap, vec2(gl_TexCoord[0].s, 1 - gl_TexCoord[0].t));

    vec4 ambient = gl_FrontMaterial.ambient * gl_FrontLightModelProduct.sceneColor;
    vec4 diffuse = gl_FrontMaterial.diffuse * tex * dsum;
    vec4 specular = gl_FrontMaterial.specular * ssum;

    // output
    gl_FragColor = vec4((gl_FrontMaterial.emission + ambient + diffuse + specular).xyz, depth);
}
