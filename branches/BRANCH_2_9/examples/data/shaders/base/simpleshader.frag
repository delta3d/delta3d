#version 120

uniform sampler2D diffuseTexture;

uniform vec4 WaterColor;
uniform float WaterHeight;
uniform float ReflectMode;

varying vec3 vLightDir;
varying vec3 vLightDir2;
varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vCamera;

float computeExpFog(float);
float computeLinearFog(float, float, float); //for underwater fog
float SampleShadowTexture();
void lightContribution(vec3, vec3, vec3, vec3, out vec3);

//TODO- this needs to be set from a property on the water
uniform float UnderWaterViewDistance;

void main(void)
{
   float altitude = ReflectMode * vPos.z;

   if(altitude < WaterHeight)
   {
      if(ReflectMode < 0.0)
      {
         discard;
      }
   }

   

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
   float fogAmt = computeExpFog(dist);   
   vec3 fogColor = gl_Fog.color.rgb;
   
   if(altitude < WaterHeight)
   {
      //camera height over the water
      float heightOverWater = 20.5 * max(vCamera.z - WaterHeight, 0.0);

      fogAmt = computeLinearFog(0.0, 3.0 * UnderWaterViewDistance, (dist - heightOverWater));

      //fade under water fog in over depth
      float depth = clamp(WaterHeight - altitude, 0.0, 30.0 * UnderWaterViewDistance);
      vec3 waterColorAtDepth = 0.85 * WaterColor.rgb;
      fogColor = (gl_LightSource[0].ambient.rgb * waterColorAtDepth)  + (gl_LightSource[0].diffuse.rgb * waterColorAtDepth);
      
      //considering the underwater color essentially removing light
      result = mix(result, 1.2 * vec3(result.rgb * WaterColor.rgb), depth / (3.0 * UnderWaterViewDistance));
   }

   
   result = mix(fogColor, result, fogAmt);

   gl_FragColor = vec4(result, diffuse.a);
}

