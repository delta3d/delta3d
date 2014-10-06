#version 120
// Basic  point light implementation
// MG 12/09/09

uniform sampler2D baseTexture;

varying vec4 vColorAmbientEmissive;

varying vec3 vNormal;
varying vec3 vViewDir;

void SpotLight(in int i,
               in vec3 inViewDir,
               in vec3 inNormal,
               inout vec3 diffuse,
               inout vec3 specular);

////////////////////////////////////////////////////////////////////////////////
void main(void)
{
   vec4 baseColor = texture2D(baseTexture, gl_TexCoord[0].st);

   vec3 diffuse = vec3(0.0);
   vec3 specular = vec3(0.0);

   SpotLight(0, vViewDir, vNormal, diffuse, specular);

   vec3 lightContrib = vColorAmbientEmissive.xyz + diffuse + specular;
   lightContrib = clamp(lightContrib, 0.0, 1.0);

   // compute final color with light and base color
   vec3 result = lightContrib * baseColor.rgb;

   gl_FragColor = vec4(result, baseColor.a);
}

