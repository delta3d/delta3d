//////////////////////////////////////////////
//A generic ocean water shader
//by Bradley Anderegg
//////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//The wave parameters are packed into two vec4's like so
// [Length, Speed, Amplitude, Frequency], [Q, reserved for later use, Direction.x, Direction.y]
//////////////////////////////////////////////////////////////////////////////////////////////////

uniform float ScreenHeight;
uniform float ScreenWidth;
uniform float waveDirection;
uniform float elapsedTime;
uniform float maxComputedDistance;
uniform mat4 inverseViewMatrix;
uniform vec4 WaterColor;
uniform vec3 cameraRecenter;

uniform float modForFOV;	
uniform float foamMaxHeight;	
uniform sampler2D waveTexture;
uniform sampler2D reflectionMap;
uniform sampler2D foamTexture;
uniform sampler3D noiseTexture;

varying vec4 pos;
varying vec3 lightVector;
varying float distanceScale;
varying float distBetweenVertsScalar;
varying vec2 vFog;
varying vec2 vertexWaveDir;
varying vec3 shaderVertexNormal;
varying vec3 vOffsetPos;

vec4 deepWaterColor = 0.74 * WaterColor;  

vec2 rotateTexCoords(vec2 coords, float angle)
{
   float degInRad = radians(angle);   
   
   vec2 coordsRot;
   coordsRot.x = dot(vec2(cos(degInRad), -sin(degInRad)), coords);
   coordsRot.y = dot(vec2(sin(degInRad), cos(degInRad)), coords);
   return coordsRot;
}

float FastFresnel(float nDotL, float fbias, float fpow)
{
   float facing = 1.0 - nDotL;
   return max(fbias + ((1.0 - fbias) * pow(facing, fpow)), 0.0);
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

void lightContribution(vec3, vec3, vec3, vec3, out vec3);
vec3 GetWaterColorAtDepth(float);


void main (void)
{   
   vec3 camPos = inverseViewMatrix[3].xyz;
   vec3 combinedPos = pos.xyz + vec3(camPos.x, camPos.y, 0.0);
   vec3 viewDir = normalize(combinedPos - camPos);

   vec3 vertexNormal = normalize(shaderVertexNormal);

   /////////////////////////////////////////////////////////////////////////////
   ////This samples the wave texture in a way that will remove tiling artifacts
   float fadeTransition = 0.05;
   float distToFragment = length(pos.xy);
   float textureScale = 20.0 + clamp((50.0 * floor(distToFragment / 50.0)), 0.0, 1000.0);
   vec3 waveNormal = vec3(0.0, 0.0, 0.0); 
   vec2 waveCoords = 0.025 * vertexNormal.xy + vec2(combinedPos.xy / textureScale);   
   waveCoords /= (0.5 + (modForFOV * 0.5) );
   //waveCoords = rotateTexCoords(waveCoords, waveDirection);

   float fadeAmt = edgeFade(fadeTransition, waveCoords);
   waveNormal += fadeAmt * SampleNormalMap(waveTexture, waveCoords);

   vec2 waveCoords2 = vec2(0.5, 0.5) + waveCoords;
   float fadeAmt2 = (1.0 - fadeAmt) * edgeFade(fadeTransition, waveCoords2);
   waveNormal += fadeAmt2 * SampleNormalMap(waveTexture, waveCoords2);

   vec2 waveCoords3 = vec2(0.25, 0.25) + waveCoords;
   float fadeAmt3 = 1.0 - clamp(fadeAmt + fadeAmt2, 0.0, 1.0);
   waveNormal += fadeAmt3 * SampleNormalMap(waveTexture, waveCoords3);
   //////////////////////////////////////////////////////////////////////////////

   waveNormal = normalize(waveNormal);
   vec3 normal = vertexNormal + waveNormal;
   normal = normalize(normal);   

   //this inverts the normal if we are underwater
   normal.z *= -1.0 * (float(gl_FrontFacing) * -1.0);

   vec3 fresnelViewAngle = -1.0 * vec3(viewDir.x, viewDir.y, 0.0);
   fresnelViewAngle = normalize(fresnelViewAngle);

   float waveNDotL = max(0.0, dot(fresnelViewAngle, normal));   
   float waveNDotL2 = max(0.0, dot(-1.0 * viewDir, normal));   
   float fresnel = FastFresnel(waveNDotL, 0.5, 20.15);
   float fresnel2 = FastFresnel(waveNDotL2, 0.05, 6.15);

   fresnel = 0.5 * (fresnel + fresnel2);
   
   vec3 refTexCoords = vec3(gl_FragCoord.x / ScreenWidth, (gl_FragCoord.y / ScreenHeight), gl_FragCoord.z);      
   refTexCoords.xy = clamp(refTexCoords.xy + 0.15 * normal.xy, 0.0, 1.0);
   vec3 reflectColor = texture2D(reflectionMap, refTexCoords.xy).rgb;

   vec3 lightContribFinal;
   vec3 lightVect = normalize(lightVector);
      
   lightContribution(normal, lightVector, gl_LightSource[0].diffuse.xyz, gl_LightSource[0].ambient.xyz, lightContribFinal);

   if (gl_FrontFacing)
   {     
      reflectColor = (deepWaterColor.xyz + fresnel * (reflectColor - deepWaterColor.xyz));
      
      lightContribFinal = sqrt(lightContribFinal);
      
      vec3 waterColorContrib = lightContribFinal * (mix(reflectColor.xyz, 0.2 * deepWaterColor.xyz, waveNDotL));
      
      //calculates a specular contribution
      vec3 normRefLightVec = reflect(lightVect, normal);
      float specularContrib = max(0.0, dot(normRefLightVec, viewDir));
      specularContrib = (0.1 * pow(specularContrib, 8.0)) + (0.8 * pow(specularContrib, 200.0));
      vec3 resultSpecular = vec3(gl_LightSource[0].specular.xyz * specularContrib);     
      
      //adds in the fog contribution, computes alpha
      vec4 resultColor = vec4(waterColorContrib + resultSpecular, 0.35 + fresnel);
      gl_FragColor = mix(gl_Fog.color, resultColor, vFog.x);
   }
   else
   {
      float wL = max(0.0, dot(viewDir, normal));   
      float fsnel = FastFresnel(wL, 0.05, 0.5);

      vec3 waterColorAtDepth = WaterColor.rgb;

      vec3 combinedColor = WaterColor.xyz;      
      
      combinedColor  = mix(combinedColor,reflectColor, fsnel);
      combinedColor = mix(gl_LightSource[0].ambient.xyz * waterColorAtDepth, combinedColor, vFog.y);

      gl_FragColor = vec4(combinedColor, 1.0);

   }
}
