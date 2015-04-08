#version 120

uniform sampler2D diffuseTexture;

uniform float d3d_SceneLuminance = 1.0;
uniform float d3d_SceneAmbience = 1.0;

varying vec4 vertexColor;
varying float vDistance;

float computeFragDepth(float, float);

void main(void)
{
   vec4 baseColor = texture2D(diffuseTexture, gl_TexCoord[0].st); 
   
   baseColor.rgb *= vec3(d3d_SceneLuminance);
   
   gl_FragColor = baseColor * vertexColor;
}


