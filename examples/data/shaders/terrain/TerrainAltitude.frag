uniform sampler2D SandTexture;
uniform sampler2D GrassTexture;
uniform sampler2D RockTexture;
uniform sampler2D BlendMaskTexture;
uniform sampler2D normalTexture;
uniform sampler2D specTexture;
uniform vec4 Altitudes;
uniform vec4 TextureScales;
uniform vec4 WaterColor;
uniform float AltitudeScale;
uniform float DetailScale;
uniform float WaterHeight;
uniform float ReflectMode;

varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vLightDir2;
varying vec3 vPos;
varying vec3 vWorldNormal;
varying vec3 vCamera;

float saturate(float inValue)
{
   return clamp(inValue, 0.0, 1.0);
}

//TODO- this needs to be set from a property on the water
const float UnderWaterViewDistance = 15.0;

void lightContribution(vec3, vec3, vec3, vec3, out vec3);
float computeLinearFog(float, float, float);
float computeExpFog(float);
vec3 GetWaterColorAtDepth(float);
float SampleShadowTexture();

void main(void)
{
   float altitude = vPos.z * AltitudeScale;

   if(altitude < WaterHeight)
   {
      if(ReflectMode < 0.0)
      {
         discard;
      }
   }
     
   float NdUp = dot(vNormal, vec3(0,0,1));

   vec3 detailColor = texture2D(SandTexture, gl_TexCoord[0].st * DetailScale).rgb;
   vec3 sandColor = texture2D(SandTexture, gl_TexCoord[0].st * TextureScales.x).rgb;
   vec3 grassColor = texture2D(GrassTexture, gl_TexCoord[0].st * TextureScales.y).rgb;
   vec3 rockColor = texture2D(RockTexture, gl_TexCoord[0].st * TextureScales.z).rgb;
   vec4 blendMask = texture2D(BlendMaskTexture, gl_TexCoord[0].st * TextureScales.w);
   
   float alphaGrass = blendMask.a;
   float alphaRock = 1.0 - alphaGrass;

   float grassRange = Altitudes.x;
   float rockRange = Altitudes.y - grassRange;

   float grassFullAlt = Altitudes.x;
   float rockFullAlt = Altitudes.y;
   
   float slope = clamp(1.0 - (dot(vec3(0,0,1), vWorldNormal) * 2.0), 0.0, 1.0);

   float grassRatio = clamp(altitude, 0.0, grassFullAlt) / grassRange;
   grassRatio = clamp(grassRatio, 0.0, 1.0);

   float rockRatio = clamp(altitude - grassFullAlt, 0.0, rockFullAlt) / rockRange;
   rockRatio = clamp(rockRatio * (1.0 - slope), 0.0, 1.0);

   vec3 baseColor = sandColor;
   baseColor = mix(baseColor, grassColor, grassRatio);
   baseColor = mix(baseColor, rockColor, rockRatio);
   
   float useRock = ceil(rockRatio) * (1.0 - clamp(1.0 - rockRatio, 0, 1));
   float useGrass = ceil(grassRatio) * (1.0 - clamp(1.0 - grassRatio, 0, 1)) - useRock;
   
   float overrideAngle = 0.9;
   float overrideAngleDiff = 1.0 - overrideAngle;
   float effectOverride = 0.0;
   if (grassRatio == 1.0)
   {
      effectOverride = clamp((NdUp - overrideAngle)/overrideAngleDiff, 0, 1);
      float effectRatio = max(effectOverride, clamp(2.0 - altitude / rockFullAlt, 0.0, 1.0));
      useRock *= effectRatio;
   }
   
   float useMask = clamp(useGrass + useRock, 0.0, 1.0);
   float maskRatio = useMask * (useGrass * alphaGrass + useRock * alphaRock);
   vec3 baseWithGrass = baseColor;
   vec3 baseWithRock = baseColor * max(blendMask.rgb, vec3(0.75,0.75,0.75));
   vec3 blendColor = mix(baseWithGrass, baseWithRock, useRock);
   baseColor = mix(baseColor, blendColor, maskRatio);
   
   baseColor = mix(baseColor, grassColor, effectOverride);
   
   // Modulate the texture with finer light/dark details.
   float avgerage = (detailColor.r + detailColor.g + detailColor.b)/3.0;
   baseColor.rgb += (avgerage - vec3(0.5, 0.5, 0.5));
   baseColor.rgb = clamp(baseColor, vec3(0,0,0), vec3(1,1,1));

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
   vec3 result = shadowAmt * lightContrib * baseColor.rgb;
  
   float dist = length(vPos - vCamera);
   
   vec3 fogColor = gl_Fog.color.rgb;

   //This adds the under water effects 
   float fogAmt = 0.0;

   if(altitude < WaterHeight)
   {
      //camera height over the water
      float heightOverWater = max(vCamera.z - WaterHeight, 0.0);

      fogAmt = computeLinearFog(0.0, 2.0 * UnderWaterViewDistance, (dist - heightOverWater));

      //fade under water fog in over depth
      float depth = clamp(WaterHeight - altitude, 0.0, 3.0 * UnderWaterViewDistance);
  
      fogColor = gl_LightSource[0].ambient.rgb * WaterColor.rgb;
      
      //considering the underwater color essentially removing light
      result = mix(result, 1.2 * vec3(result.rgb * WaterColor.rgb), depth / (3.0 * UnderWaterViewDistance));
   }
   else
   {
      fogAmt = computeExpFog(dist);
   }

   result = mix(fogColor, result, fogAmt);

   //gl_FragColor = vec4(useGrass, useRock, useMask, 1.0);
   gl_FragColor = vec4(result, 1.0);
}
