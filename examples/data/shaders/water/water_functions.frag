#version 120
uniform float WaterHeight;
uniform vec4 WaterColor;

//used for planar reflections
//it just needs some reasonable default so its never zero
uniform float ScreenHeight = 1024;
uniform float ScreenWidth = 768;

uniform float d3d_NearPlane;
uniform float d3d_FarPlane;

uniform sampler2D reflectionMap;

const float cDeepWaterScalar = 0.64;
const float cViewDistance = 100.0; 

uniform float UnderWaterViewDistance;

//must include shaders/base/fragment_functions.frag to get these
float samplePreDepthTexture(vec2 fragCoord);
float computeFragDepth(float distance);
vec2 rotateTexCoords(vec2 coords, float angle);
float computeExpFogWithDensity(float fogDistance, float fogDensity);
//end include

vec3 waterSamplePlanarReflectTexture(vec3 normal, vec2 fragCoord)
{
   vec2 refTexCoords = vec2(fragCoord.x / ScreenWidth, (fragCoord.y / ScreenHeight));      
   refTexCoords = clamp(refTexCoords.xy + 0.05 * normal.xy, 0.0, 1.0);
   return texture2D(reflectionMap, refTexCoords).rgb;   
}

float computeWaterColumn(vec4 viewPos, vec2 fragCoord)
{
   float depthAtPixel = samplePreDepthTexture(fragCoord);
      
   vec3 ecPosition = viewPos.xyz / viewPos.w;
   float waterFragDepth = computeFragDepth(length(ecPosition)) * (d3d_FarPlane - d3d_NearPlane);
   float waterDepth = max(depthAtPixel - waterFragDepth, 0.0);
   return computeExpFogWithDensity(waterDepth, 0.25);
}

vec2 ComputeWaveTextureCoords(float distToFragment, vec3 worldPos)
{
   float distDivisor =  75.0 * (10.0 + ( distToFragment / 50.0));
   float distanceStep =  3.5 * 1.0 + floor( (10.0 * distToFragment) / distDivisor);
   distanceStep = 1.5 * pow(distanceStep, 3.0185);  
   float textureScale = clamp(distanceStep, 0.0, 25000.0);
   
   return vec2(worldPos.xy / textureScale);   
}

vec3 GetWaterColorAtDepth(float pDepth)
{
   vec3 cDeepWaterColor = cDeepWaterScalar * WaterColor.xyz;
   float dist = WaterHeight - pDepth;
   dist = clamp(dist, 0.0, cViewDistance);
   float depthScalar = (dist / cViewDistance);

   vec3 color = mix(WaterColor.xyz, cDeepWaterColor, depthScalar);

   return color;
}

vec3 SampleNormalMap(sampler2D tex, vec2 texCoords)
{
   vec4 color = texture2D(tex, texCoords);
   color *= 2.0;
   color -= 1.0;
   return normalize(color.xyz);
}

float edgeFade(float blendStart, vec2 texCoord)
{
   texCoord = mod(texCoord, 1.0);   
   float dx = (0.5 - texCoord.x);
   float dy = (0.5 - texCoord.y);
 
   float dist = clamp(0.5 - length(vec2(dx, dy)), 0.0, 0.5);      

   float fadeAmt = clamp(dist - blendStart, 0.0,  blendStart) / blendStart;
   return fadeAmt;
}

/////////////////////////////////////////////////////////////////////////////
////This triple samples the wave texture in a way that will remove tiling artifacts   
uniform sampler2D waveTexture;
vec3 ComputeNormals(vec2 waveCoords)
{
   float fadeTransition = 0.05;
   vec3 waveNormal = vec3(0.0, 0.0, 0.0); 
   
   float fadeAmt = edgeFade(fadeTransition, waveCoords);
   waveNormal += fadeAmt * SampleNormalMap(waveTexture, waveCoords);

   vec2 waveCoords2 = vec2(0.5, 0.5) + waveCoords;
   float fadeAmt2 = (1.0 - fadeAmt) * edgeFade(fadeTransition, waveCoords2);
   waveNormal += fadeAmt2 * SampleNormalMap(waveTexture, waveCoords2);

   vec2 waveCoords3 = vec2(0.25, 0.25) + waveCoords;
   float fadeAmt3 = 1.0 - clamp(fadeAmt + fadeAmt2, 0.0, 1.0);
   waveNormal += fadeAmt3 * SampleNormalMap(waveTexture, waveCoords3);
   return normalize(waveNormal);
}



