uniform sampler2D SandTexture;
uniform sampler2D GrassTexture;
uniform sampler2D RockTexture;
uniform sampler2D SnowTexture;
uniform sampler2D normalTexture;
uniform sampler2D specTexture;
uniform vec4 Altitudes;
uniform vec4 TextureScales;
uniform vec4 WaterColor;
uniform float AltitudeScale;
uniform float DetailScale;
uniform float WaterSurfaceOffset;
uniform float WaterFadeDepth;
uniform float WaterHeight;
uniform float ReflectMode;

uniform int mode;

varying vec3 vNormal;
varying vec3 vLightDir;
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

void main(void)
{
   float h = ReflectMode * vPos.z;

   if(h < WaterHeight)
   {
      if(ReflectMode < 0.0)
      {
         discard;
      }
   }
     

   vec3 detailColor = texture2D(SandTexture, gl_TexCoord[0].st * DetailScale).rgb;
   vec3 sandColor = texture2D(SandTexture, gl_TexCoord[0].st * TextureScales.x).rgb;
   vec3 grassColor = texture2D(GrassTexture, gl_TexCoord[0].st * TextureScales.y).rgb;
   vec3 rockColor = texture2D(RockTexture, gl_TexCoord[0].st * TextureScales.z).rgb;
   vec3 snowColor = texture2D(SnowTexture, gl_TexCoord[0].st * TextureScales.w).rgb;

   float alt = ReflectMode * vPos.z * AltitudeScale;

   float grassRange = Altitudes.x;
   float rockRange = Altitudes.y - grassRange;
   float snowRange = Altitudes.z - rockRange;

   float grassFullAlt = Altitudes.x;
   float rockFullAlt = Altitudes.y;
   float snowFullAlt = Altitudes.z;
   
   float slope = clamp(1.0 - (dot(vec3(0,0,1), vWorldNormal) * 2.0), 0.0, 1.0);

   float waterRatio = 0.0;//clamp(-alt + WaterSurfaceOffset, 0.0, WaterFadeDepth)/WaterFadeDepth;
   vec3 waterColor = mix(sandColor, WaterColor.rgb, WaterColor.a);

   float grassRatio = clamp(alt, 0.0, grassFullAlt) / grassRange;
   grassRatio = clamp(grassRatio, 0.0, 1.0);

   float rockRatio = clamp(alt - grassFullAlt, 0.0, rockFullAlt) / rockRange;
   rockRatio = clamp(rockRatio * (1.0 - slope), 0.0, 1.0);
   
   float snowRatio = clamp(alt - rockFullAlt, 0.0, snowFullAlt) / snowRange;
   snowRatio = clamp(snowRatio, 0.0, 1.0);

   vec3 baseColor = mix(sandColor, waterColor, waterRatio);

   baseColor = mix(baseColor, grassColor, grassRatio);
   baseColor = mix(baseColor, rockColor, rockRatio);
   baseColor = mix(baseColor, snowColor, snowRatio);
   
   // Modulate the texture with finer light/dark details.
   float avgerage = (detailColor.r + detailColor.g + detailColor.b)/3.0;
   baseColor.rgb += (avgerage - vec3(0.5, 0.5, 0.5));
   baseColor.rgb = clamp(baseColor, vec3(0,0,0), vec3(1,1,1));

   // normalize all of our incoming vectors
   vec3 lightDir = normalize(vLightDir);
   
   //Compute the Light Contribution
   vec3 lightContrib;
   lightContribution(vNormal, lightDir, gl_LightSource[0].diffuse.xyz, gl_LightSource[0].ambient.xyz, lightContrib);
  
   vec3 result = lightContrib * baseColor.rgb;
  
   float dist = length(vPos - vCamera);
   
   vec3 fogColor = gl_Fog.color.rgb;

   //This adds the under water effects 
   float fogAmt = 0.0;
   float height = ReflectMode * vPos.z;

   if(height < WaterHeight)
   {
      //camera height over the water
      float heightOverWater = max(vCamera.z - WaterHeight, 0.0);

      fogAmt = computeLinearFog(0.0, 2.0 * UnderWaterViewDistance, (dist - heightOverWater));

      //fade under water fog in over depth
      float depth = clamp(WaterHeight - height, 0.0, 3.0 * UnderWaterViewDistance);
  
      fogColor = gl_LightSource[0].ambient.rgb * WaterColor.rgb;
      
      //considering the underwater color essentially removing light
      result = mix(result, 1.2 * vec3(result.rgb * WaterColor.rgb), depth / (3.0 * UnderWaterViewDistance));
   }
   else
   {
      fogAmt = computeExpFog(dist);
   }

   result = mix(fogColor, result, fogAmt);

   gl_FragColor = vec4(result.rgb, 1.0);
}
