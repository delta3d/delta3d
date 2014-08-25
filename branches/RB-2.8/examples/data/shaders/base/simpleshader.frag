#version 120

uniform sampler2D diffuseTexture;

varying vec3 vLightDir;
varying vec3 vLightDir2;
varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vCamera;

float computeExpFog(float);
float SampleShadowTexture();
void lightContribution(vec3, vec3, vec3, vec3, out vec3);

void main(void)
{
   vec4 diffuse = texture2D(diffuseTexture, gl_TexCoord[0].st);

   // normalize all of our incoming vectors
   vec3 lightDir = normalize(vLightDir);
   vec3 lightDir2 = normalize(vLightDir2);
   
   //Compute the Light Contribution
   vec3 lightContribSun;
   vec3 lightContribMoon;

   lightContribution(vNormal, lightDir, gl_LightSource[0].diffuse.xyz, gl_LightSource[0].ambient.xyz, lightContribSun);
   lightContribution(vNormal, lightDir2, gl_LightSource[1].diffuse.xyz, gl_LightSource[1].ambient.xyz, lightContribMoon);
  
   vec3 lightContrib = lightContribSun + lightContribMoon;

   float shadowAmt = 1.0;//SampleShadowTexture();
   vec3 result = shadowAmt * lightContrib * diffuse.rgb;
  
   float dist = length(vPos - vCamera);
   
   vec3 fogColor = gl_Fog.color.rgb;

   //This adds the under water effects 
   float fogAmt = computeExpFog(dist);   

   result = mix(fogColor, result, fogAmt);

   gl_FragColor = vec4(result, 1.0);
}

