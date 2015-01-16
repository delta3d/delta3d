#version 120
//////////////////////////////////////////////
//A generic ocean water shader
//by Bradley Anderegg
//////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//The wave parameters are packed into two vec4's like so
// [Length, Speed, Amplitude, Frequency], [Q, reserved for later use, Direction.x, Direction.y]
//////////////////////////////////////////////////////////////////////////////////////////////////


uniform float waveDirection;
uniform float elapsedTime;
uniform float maxComputedDistance;
uniform mat4 inverseViewMatrix;
uniform vec4 WaterColor;
uniform vec3 cameraRecenter;
uniform float UnderWaterViewDistance;
uniform float d3d_Exposure = 1.0;

uniform float modForFOV;	
uniform float foamMaxHeight;	
uniform sampler2D foamTexture;
uniform sampler3D noiseTexture;
uniform samplerCube d3d_ReflectionCubeMap;

varying vec4 pos;
varying vec4 viewPos;
varying vec3 lightVector;
varying vec3 lightVector2;
varying float distanceScale;
varying float distBetweenVertsScalar;
varying vec2 vFog;
varying vec2 vertexWaveDir;
varying vec3 shaderVertexNormal;
varying vec3 vOffsetPos;


void lightContribution(vec3, vec3, vec3, vec3, out vec3);
vec3 computeDynamicLightContrib(vec3 wsNormal, vec3 wsPos);
vec3 GetWaterColorAtDepth(float);
float computeLinearFog(float startFog, float endFog, float fogDistance);
float computeFragDepth(float distance);
float computeReflectionCoef(vec3 viewDir, vec3 viewSpaceNormal, float refractIndex);
vec3 ComputeNormals(vec2);
vec2 ComputeWaveTextureCoords(float distToFragment, vec3 worldPos);
float FastFresnel(float nDotL, float fbias, float fpow);
vec3 waterSamplePlanarReflectTexture(vec3 normal, vec2 fragCoord);
float computeWaterColumn(vec4 viewPos, vec2 fragCoord);
vec2 rotateTexCoords(vec2 coords, float angle);


void main (void)
{   
   vec3 normal = vec3(0.0, 0.0, 0.0);
   vec3 vertexNormal = normalize(shaderVertexNormal);
   vec3 camPos = inverseViewMatrix[3].xyz;
   vec3 combinedPos = pos.xyz + vec3(camPos.x, camPos.y, 0.0);
   vec3 viewDir = normalize(combinedPos - camPos);
   float distToFragment = length(pos.xy);

   vec2 waveCoords = ComputeWaveTextureCoords(distToFragment, combinedPos);
   vec3 waveNormal = ComputeNormals(waveCoords);
   
   vec2 waveCoords2 = 2.75 * vec2(combinedPos.xy / 1000.5);   
   waveNormal = (0.5 * waveNormal) + (0.5 * ComputeNormals(waveCoords2));
   
   normal = (0.5 * vertexNormal) + (0.5 * dot(vertexNormal, waveNormal) * waveNormal);
   normal = normalize(normal);
   
   //this inverts the normal if we are underwater
   //normal.z *= -1.0 * (float(gl_FrontFacing) * -1.0);

   float waveNDotL = max(0.0, dot(-1.0 * viewDir, normal));   
   //float fresnel = FastFresnel(waveNDotL, 0.15, 5.15);
   //float fresnel = FastFresnel(waveNDotL, 0.15, 6.15);
   float fresnel = computeReflectionCoef(-normalize(viewPos.xyz / viewPos.w), normalize(gl_NormalMatrix * normal), 1.333);
   fresnel = clamp(fresnel, 0.0, 1.0);
   
   vec3 reflectColor = waterSamplePlanarReflectTexture(normal, gl_FragCoord.xy);
   
   vec3 lightContribSun;
   vec3 lightContribMoon;
   
   vec3 lightVect = normalize(lightVector);
   vec3 lightVect2 = normalize(lightVector2);
      
   lightContribution(normalize(vertexNormal + (0.3 * normal)), lightVect, gl_LightSource[0].diffuse.xyz, gl_LightSource[0].ambient.xyz, lightContribSun);
   lightContribution(vertexNormal, lightVect2, gl_LightSource[1].diffuse.xyz, gl_LightSource[1].ambient.xyz, lightContribMoon);

   vec3 dynamicLightContrib = computeDynamicLightContrib(normal.xyz, combinedPos.xyz);

   vec3 lightContrib =  clamp(lightContribSun + lightContribMoon + dynamicLightContrib, vec3(0.0), vec3(d3d_Exposure)) ;


   //calculates a specular contribution
   //Sun
   vec3 normRefLightVecSun = reflect(lightVect, normal);
   float specularContribSun = max(0.0, dot(normRefLightVecSun, viewDir));
   specularContribSun = 0.5 * pow(specularContribSun, 50.0);
   
   //Moon
   vec3 normRefLightVecMoon = reflect(lightVect, normal);
   float specularContribMoon = max(0.0, dot(normRefLightVecMoon, viewDir));
   specularContribMoon = 0.5 * pow(specularContribMoon, 50.0);
   
   vec3 resultSpecular = specularContribSun * gl_LightSource[0].specular.rgb;     
   resultSpecular += specularContribMoon * gl_LightSource[1].specular.rgb;     
   
   if (gl_FrontFacing)
   {  
      float waterDepth = computeWaterColumn(viewPos, gl_FragCoord.xy);

      vec3 waterColorContrib = (lightContrib * WaterColor.rgb) + resultSpecular;
      
      float dotView = max(dot(-viewDir, normal), 0.0);
      
      
      float minOpacity = 0.1;
      float opaqueDist = 0.15 * UnderWaterViewDistance;
      float opacity = sqrt( min( waterDepth / opaqueDist, 1.0));

      float waterColorTint = dotView * (minOpacity + (1.0 - minOpacity ) * opacity );
      
      vec3 refractionCoords = refract(viewDir, normal, 1.05);
      vec3 refractionColor = textureCube(d3d_ReflectionCubeMap, refractionCoords.xyz).rgb;
      //refractionColor = mix(waterColorContrib, refractionColor, dotView);
      
      refractionColor = (waterColorContrib * (1.0 - waterColorTint)) + (refractionColor * waterColorTint);
      
      ///////////////////////////////////////////////////
      //compute foam on edge of beach
      vec3 noiseTexCoords = vec3(combinedPos.x / 50.0, combinedPos.y / 50.0, 0.05 * elapsedTime);
      float noisevalue = abs(texture3D(noiseTexture, noiseTexCoords).a);

      vec2 foamCoords = vec2(combinedPos.x + elapsedTime * 0.25, combinedPos.y + elapsedTime* -0.25) / 13.5;
      vec2 foamCoords2 = vec2(combinedPos.x + elapsedTime * -0.25, combinedPos.y + elapsedTime * 0.25) / 15.35;
      
      foamCoords = rotateTexCoords(foamCoords, 30.0);
      foamCoords2 = rotateTexCoords(foamCoords2, -120.0);

      vec4 foamColor = 0.5 * texture2D(foamTexture, foamCoords);
      foamColor += 0.5 * texture2D(foamTexture, foamCoords2);
      foamColor.rgb *= lightContrib;
      foamColor *= opacity;

      float foamAmt = max(-1.0 * dot(normal, vec3(vertexWaveDir.xy, 0.0)), 0.0);
      float foamNoise = clamp(2.0 * pow(noisevalue, 2.0), 0.0, 1.0);         
      foamAmt = clamp(15.0 * pow(foamAmt, 3.5), 0.0, 1.0);
      //foamAmt *= foamNoise;
      //foamColor *= foamAmt;
      foamColor *= foamNoise;
      
      foamColor += foamNoise * foamAmt;
      foamColor += opacity * foamNoise * foamAmt;
      
      ////////////////////////////////////////////////////////
      
      //foam overrides reflection
      reflectColor = (opacity * foamColor.rgb * reflectColor) + ((1.0 - opacity) * reflectColor);
   
      //get rid of planar reflections looking back at us
      float planarReflectCoef = max(0.0, dot(vec3(0.0, 0.0, 1.0), vertexNormal));   
      planarReflectCoef = smoothstep(0.97, 1.0, planarReflectCoef);
      
      vec3 reflectLightContrib = min(vec3(0.35) + lightContrib, vec3(1.0));
      reflectColor = mix(reflectLightContrib * WaterColor.rgb, reflectColor, planarReflectCoef);
   

      vec3 resultColor = mix(refractionColor, reflectColor, fresnel);
      
      //gl_FragColor = mix(gl_Fog.color, resultColor, vFog.x);
      float waterAlpha = minOpacity + ((1.0 - opacity) * WaterColor.a);
      vec4 alphaOverlay = vec4(waterAlpha * resultColor, waterAlpha - opacity);
      alphaOverlay += (1.0 - waterAlpha) * vec4(refractionColor, 1.0);
      gl_FragColor = vec4(alphaOverlay.rgb + foamColor.rgb, alphaOverlay.a);
   }
   else
   {
      float wL = max(0.0, dot(viewDir, normal));   
      float fsnel = FastFresnel(wL, 0.05, 0.5);

      vec3 waterColorAtDepth = 0.85 * WaterColor.rgb;

      vec3 combinedColor = WaterColor.xyz;      
      
      combinedColor  = mix(combinedColor,reflectColor, 1.0);
      combinedColor = (gl_LightSource[0].ambient.xyz * waterColorAtDepth) + mix(gl_LightSource[0].diffuse.xyz * waterColorAtDepth, combinedColor, vFog.y);

      gl_FragColor = vec4(combinedColor, 1.0);
   }

   //debug 
   //gl_FragColor = vec4(vec3(WaterColor.rgb) * fresnel, 1.0);
   //gl_FragColor = vec4(0.5 * (reflectCubeCoords.xyz + vec3(1.0)), 1.0);
}
