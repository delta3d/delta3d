#version 120

uniform sampler2D UnderWaterSandTexture;
uniform sampler2D SandTexture;
uniform sampler2D GrassTexture;
uniform sampler2D RockTexture;
uniform sampler2D BlendMaskTexture;
uniform sampler2D MountainsideTexture;
uniform sampler2D normalTexture;
uniform sampler2D specTexture;
uniform vec4 Altitudes;
uniform vec4 TextureScales;
uniform vec4 WaterColor;
uniform float AltitudeScale;
uniform float DetailScale;
uniform float WaterHeight = 0.0;
uniform float ReflectMode = 1.0;
uniform float d3d_SceneLuminance = 1.0;
uniform float d3d_SceneAmbience = 1.0;
uniform float d3d_Exposure = 1.0;
uniform bool d3d_DepthOnlyPass = false;
uniform bool d3d_ShadowOnlyPass = false;


varying vec3 vLightDir;
varying vec3 vLightDir2;
varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vWorldNormal;
varying vec3 vCamera;
varying vec4 vViewPos;

float saturate(float inValue)
{
   return clamp(inValue, 0.0, 1.0);
}

//TODO- this needs to be set from a property on the water
uniform float UnderWaterViewDistance;

void lightContribution(vec3, vec3, vec3, vec3, out vec3);
vec3 computeDynamicLightContrib(vec3 wsNormal, vec3 wsPos);
float computeLinearFog(float, float, float);
float computeExpFog(float);
vec3 GetWaterColorAtDepth(float);
float SampleShadowTexture();
float computeFragDepth(float);

uniform mat4 osg_ViewMatrixInverse;

void main(void)
{
   vec3 model = mat3(osg_ViewMatrixInverse) * vec3(vViewPos.xyz);
   vec3 upDir = normalize(model.xyz);
   float altitude = vPos.z;   
   
   if(ReflectMode < 0.0)
   {
      altitude *= -1.0;

      if(altitude < WaterHeight)
      {
         discard;
      }
   }
   
   //if your shader ever sets the frag depth it must always
   if(d3d_DepthOnlyPass)
   {
      vec3 ecPosition3 = vViewPos.xyz / vViewPos.w;
      float dist = length(ecPosition3);
      
      gl_FragDepth = computeFragDepth(dist);
   }
   else
   {
      gl_FragDepth = gl_FragCoord.z;
   }

   if(d3d_DepthOnlyPass || d3d_ShadowOnlyPass)
   {
      return;
   }
     

   float NdUp = dot(vNormal, upDir);
   
   // Red verts mark a path/road.
   float roadRatio = 1.0 - gl_Color.g * gl_Color.b * 0.5;

   float sandRange = Altitudes.x;
   float grassRange = Altitudes.y - sandRange;
   float rockRange = Altitudes.z - grassRange;

   float sandFullAlt = Altitudes.x;
   float grassFullAlt = Altitudes.y;
   float rockFullAlt = Altitudes.z;

   float altRatio = 0.0;
   float rockRatio = 0.0;
   float slope = clamp(1.0 - (dot(vec3(0,0,1), vWorldNormal) * 2.0), 0.0, 1.0);
   
   vec3 baseColor;
   vec3 detailColor;
   vec3 lowAltColor;
   vec3 highAltColor;
   vec4 blendMask = texture2D(BlendMaskTexture, gl_TexCoord[0].st * TextureScales.w);
   vec4 blendColor;
   vec3 grassColor;
   
   vec4 grassBlendColor = blendMask;
   vec4 rockBlendColor = vec4(max(blendMask.rgb, vec3(0.75,0.75,0.75)), 1.0 - blendMask.a);
   
   if (altitude < sandFullAlt)
   {
      lowAltColor = texture2D(UnderWaterSandTexture, gl_TexCoord[0].st * 150.0).rgb;
      highAltColor = texture2D(SandTexture, gl_TexCoord[0].st * TextureScales.x).rgb;
      
      altRatio = clamp(altitude / sandRange, 0.0, 1.0);
      float effect = clamp(pow(altRatio,2.0), 0.0, 1.0);
      baseColor = mix(lowAltColor, highAltColor, effect);
      
      detailColor = mix(
         texture2D(UnderWaterSandTexture, gl_TexCoord[0].st * 50.0).rgb,
         texture2D(SandTexture, gl_TexCoord[0].st * DetailScale * TextureScales.x).rgb,
         effect);
      
      blendColor.rgb = highAltColor.rgb;
      blendColor.a = grassBlendColor.a * effect;
   }
   else if (altitude < grassFullAlt)
   {
      lowAltColor = texture2D(SandTexture, gl_TexCoord[0].st * TextureScales.x).rgb;
      highAltColor = texture2D(GrassTexture, gl_TexCoord[0].st * TextureScales.y).rgb;
      
      altRatio = clamp(altitude / grassRange, 0.0, 1.0);
      float effect = clamp(pow(altRatio,4.0), 0.0, 1.0);
      baseColor = mix(lowAltColor, highAltColor, effect);
      
      detailColor = mix(
         texture2D(SandTexture, gl_TexCoord[0].st * DetailScale * TextureScales.x).rgb,
         texture2D(GrassTexture, gl_TexCoord[0].st * DetailScale * TextureScales.y).rgb,
         effect);
         
      blendColor.rgb = grassBlendColor.rgb;
      blendColor.a = grassBlendColor.a * effect;
   }
   else if (altitude < rockFullAlt)
   {
      lowAltColor = texture2D(GrassTexture, gl_TexCoord[0].st * TextureScales.y).rgb;
      highAltColor = texture2D(RockTexture, gl_TexCoord[0].st * TextureScales.z).rgb;
      
      grassColor = lowAltColor;
      
      altRatio = clamp(altitude / rockRange, 0.0, 1.0);
      float effect = clamp(pow(altRatio,2.0), 0.0, 1.0);
      baseColor = mix(lowAltColor, highAltColor, effect);
      
      detailColor = mix(
         texture2D(GrassTexture, gl_TexCoord[0].st * DetailScale * TextureScales.y).rgb,
         texture2D(RockTexture, gl_TexCoord[0].st * TextureScales.z).rgb,
         effect);
         
      rockBlendColor.rgb *= baseColor;
      blendColor = mix(grassBlendColor, rockBlendColor, effect);

      rockRatio = effect;
   }
   else
   {
      detailColor = texture2D(MountainsideTexture, gl_TexCoord[0].st * TextureScales.z).rgb;
      lowAltColor = texture2D(RockTexture, gl_TexCoord[0].st * TextureScales.z).rgb;
      highAltColor = lowAltColor;
      
      altRatio = 1.0;
      baseColor = highAltColor;
      
      rockBlendColor.rgb *= baseColor;
      blendColor.rgb = rockBlendColor.rgb;
      blendColor.a = rockBlendColor.a * clamp(0.0, 1.0, 1.0 - (altitude - rockFullAlt)/rockRange);
      
      rockRatio = 1.0;
   }
   
   baseColor = mix(baseColor, blendColor.rgb, blendColor.a);
   
   float overrideAngle = 0.9;
   float overrideAngleDiff = 1.0 - overrideAngle;
   float effectOverride = 0.0;
   if (altitude >= grassFullAlt)
   {
      float effectRatio = (NdUp - overrideAngle)/overrideAngleDiff;
      effectOverride = clamp(effectRatio * rockRatio, 0.0, 1.0);
      vec3 mountainColor = texture2D(MountainsideTexture, gl_TexCoord[0].st * DetailScale * TextureScales.z).rgb;
      baseColor = mix(baseColor, mountainColor, effectOverride);
   }
   
   // Modulate the texture with finer light/dark details.
   float avgerage = (detailColor.r + detailColor.g + detailColor.b)/3.0;
   baseColor.rgb += (avgerage - vec3(0.5, 0.5, 0.5));
   baseColor.rgb = clamp(baseColor, vec3(0.0,0.0,0.0), vec3(1.0,1.0,1.0));
   
   // Determine if road texture needs to be overlayed.
//   baseColor = mix(baseColor, sandColor, roadRatio);

   // normalize all of our incoming vectors
   vec3 lightDir = normalize(vLightDir);
   vec3 lightDir2 = normalize(vLightDir2);
   
   //Compute the Light Contribution
   vec3 lightContribSun;
   vec3 lightContribMoon;

   lightContribution(vNormal, lightDir, gl_LightSource[0].diffuse.xyz, gl_LightSource[0].ambient.xyz, lightContribSun);
   lightContribution(vNormal, lightDir2, gl_LightSource[1].diffuse.xyz, gl_LightSource[1].ambient.xyz, lightContribMoon);
  
   vec3 dynamicLightContrib = computeDynamicLightContrib(vNormal, vPos);

   vec3 lightContrib = clamp(lightContribSun + lightContribMoon + dynamicLightContrib, vec3(0.0), vec3(d3d_Exposure, d3d_Exposure, d3d_Exposure)) ;

   float shadowAmt = SampleShadowTexture();
   vec3 result = shadowAmt * lightContrib * baseColor.rgb;
  
   float dist = length(vPos - vCamera);
   
   vec3 fogColor = gl_Fog.color.rgb;

   //This adds the under water effects 
   float fogAmt = 0.0;

   if(altitude < WaterHeight - 1.0)
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
   else
   {
      fogColor *= gl_LightSource[0].diffuse.rgb;
      fogAmt = computeExpFog(dist);
   }

   result = mix(fogColor, result, fogAmt);

   gl_FragColor = vec4(result.rgb, 1.0);
   //gl_FragColor = grassBlendColor;
}
