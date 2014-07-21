//////////////////////////////////////////////
//A generic ocean water shader
//by Bradley Anderegg
//////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//The wave parameters are packed into two vec4's like so
// [Length, Speed, Amplitude, Frequency], [Q, reserved for later use, Direction.x, Direction.y]
//////////////////////////////////////////////////////////////////////////////////////////////////
const int MAX_WAVES = 32;

uniform vec4 waveArray[2 * MAX_WAVES];
uniform float WaterHeight;

uniform float ScreenHeight;
uniform float ScreenWidth;
uniform float waveDirection;
uniform float elapsedTime;
uniform float maxComputedDistance;
uniform mat4 inverseViewMatrix;
uniform vec4 WaterColor;// = vec4(30.0 / 256.0, 80.0 / 256.0, 150.0 / 256.0, 1.0);
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
   float textureScale = 20.0 + clamp((50.0 * floor(distToFragment / 50.0)), 0.0, 2000.0);
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


   float waveNormalFadeOut = clamp(distToFragment / 1000.0, 0.0, 1.0);
   waveNormal = mix(waveNormal, vec3(0.0, 0.0, 1.0), waveNormalFadeOut);
   waveNormal = normalize(waveNormal);
   
   float waveNormalContribution = 1.0;//max(vec3(0.0), dot(vertexNormal, vec3(1.0, 1.0, 0.0)));
   waveNormal = waveNormalContribution * waveNormal;
   
   vec3 normal = vertexNormal + waveNormal;
   normal = normalize(normal);   

   //this inverts the normal if we are underwater
   normal.z *= -1.0 * (float(gl_FrontFacing) * -1.0);

   float waveNDotL = max(0.0, dot(-viewDir, normal));   
   float fresnel = FastFresnel(waveNDotL, 0.05, 6.0);

   if (gl_FrontFacing)
   {     
      vec3 refTexCoords = vec3(gl_FragCoord.x / ScreenWidth, gl_FragCoord.y / ScreenHeight, gl_FragCoord.z);
      refTexCoords.xy = clamp(refTexCoords.xy + 0.4 * normal.xy, 0.0, 1.0);
      vec3 reflectColor = texture2D(reflectionMap, refTexCoords.xy).rgb;
      reflectColor = (deepWaterColor.xyz + fresnel * (reflectColor - deepWaterColor.xyz));
      
      vec3 lightVect = normalize(lightVector);
      
      vec3 lightContribFinal;
      lightContribution(normal, lightVector, gl_LightSource[0].diffuse.xyz, 
         gl_LightSource[0].ambient.xyz, lightContribFinal);

      lightContribFinal = sqrt(lightContribFinal);

/*    COMMMENTING OUT FOAM 
      vec3 noiseTexCoords = vec3(combinedPos.x / 10.0, combinedPos.y / 10.0, 0.05 * elapsedTime);
      float noisevalue = abs(texture3D(noiseTexture, noiseTexCoords).a);

      //this is the nearby foam effect contribution
      float foamAmt = max(-1.0 * dot(normal, vec3(vertexWaveDir.xy, 0.0)), 0.0);
      float foamNoise = clamp(10.0 * pow(noisevalue, 16.0), 0.0, 1.0);         
      foamAmt = clamp(100.0 * pow(foamAmt, 3.0), 0.0, 1.0);
      foamAmt *= foamNoise;

      //this is the distant foam effect contribution - it's really nutty. It is the outer band of 
      // foam value. And, it changes based on the field of view! What we want is for the foam to 
      // appear in little patches based on the normal on the wave.       
      float distfoamAmt = max(-1.0 * dot(vertexNormal, vec3(vertexWaveDir.xy, 0.0)), 0.0);
      // What we want.  0.5 at wide (90 HFov).  0.7 at med (45 HFov).  0.9 at narrow (< 10 HFov)
      float distFoamExpFOVScalar = clamp(0.95 - (0.4 * (modForFOV)), 0.55, 0.95);
      float distFoamExp = distFoamExpFOVScalar * (10.0 + (camPos.z / 75.0));
      distfoamAmt = clamp(1000.0 * pow(distfoamAmt, distFoamExp), 0.0, 1.0);
      float foamDistScale = pow(46.0 * (1.0 - distanceScale), 6.0);
      float foamDistScaleFar = pow(distanceScale, 12.0);
      float distFoamNoise = foamNoise; //clamp(10.0 * pow(noisevalue, 16.0), 0.0, 1.0);         
      distfoamAmt = clamp(distfoamAmt * foamDistScale * foamDistScaleFar * distFoamNoise, 0.0, 1.0);
      

      vec2 foamTexCoords = 0.1 * vec2(combinedPos.x * 3.0, (0.66 * elapsedTime) - combinedPos.y * 3.0);
      foamTexCoords = rotateTexCoords(foamTexCoords, waveDirection);
      vec4 foamColor = texture2D(foamTexture, foamTexCoords);
   
      //this is the cumulative foam effect contribution
      foamColor.xyz = lightContribFinal * foamColor.xyz * (clamp(foamAmt + distfoamAmt, 0.0, 1.0));
*/

      
      vec3 waterColorContrib = lightContribFinal * (mix(reflectColor.xyz, 0.2 * deepWaterColor.xyz, waveNDotL));
      
      //calculates a specular contribution
      vec3 normRefLightVec = reflect(lightVect, normal);
      float specularContrib = max(0.0, dot(normRefLightVec, viewDir));
      specularContrib = (0.1 * pow(specularContrib, 8.0)) + (0.8 * pow(specularContrib, 200.0));
      vec3 resultSpecular = vec3(gl_LightSource[0].specular.xyz * specularContrib);     
      
      //adds in the fog contribution
      vec4 finalColor = vec4(mix(/*foamColor.xyz +*/ waterColorContrib + resultSpecular, gl_Fog.color.rgb, vFog.x), WaterColor.a);
      gl_FragColor = finalColor;
   }
   else
   {

      vec3 waterColorAtDepth = GetWaterColorAtDepth(camPos.z);

      vec3 resultColor = (0.5 * fresnel) + mix(WaterColor.xyz, waterColorAtDepth, fresnel);

      vec3 lightContribFinal;
      lightContribution(vec3(0.0, 0.0, 1.0), lightVector, gl_LightSource[0].diffuse.xyz, 
         gl_LightSource[0].ambient.xyz, lightContribFinal);

      float alpha = clamp(vFog.x + 0.95, 0.0, 1.0);      
      vec3 combinedColor = lightContribFinal * mix(resultColor, waterColorAtDepth, vFog.y);
      gl_FragColor = vec4(combinedColor, WaterColor.a);
   }
}
