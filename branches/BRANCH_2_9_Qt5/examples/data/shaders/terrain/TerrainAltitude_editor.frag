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
uniform float WaterHeight;

varying vec4 vPos;
varying vec3 vNormal;

uniform mat4 osg_ViewMatrixInverse;

void main(void)
{
   float altitude = vPos.z;   

   vec3 upDir = vec3(0.0, 0.0, 1.0);
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
   float slope = clamp(1.0 - (dot(vec3(0,0,1), vNormal) * 2.0), 0.0, 1.0);
   
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
   

   if(altitude < WaterHeight)
   {
      baseColor.rgb *= WaterColor.rgb;
   }
   
   gl_FragColor = vec4(baseColor.rgb, 1.0);
}
