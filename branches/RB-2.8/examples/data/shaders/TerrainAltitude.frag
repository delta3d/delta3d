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
uniform float WaterSurfaceOffset;
uniform float WaterFadeDepth;

uniform int mode;

varying vec3 vNormal;
varying vec3 vViewDir;
varying vec3 vLightDir;
varying vec3 vPos;

float saturate(float inValue)
{
   return clamp(inValue, 0.0, 1.0);
}

void main(void)
{  
   vec3 sandColor = texture2D(SandTexture, gl_TexCoord[0].st * TextureScales.x).rgb;
   vec3 grassColor = texture2D(GrassTexture, gl_TexCoord[0].st * TextureScales.y).rgb;
   vec3 rockColor = texture2D(RockTexture, gl_TexCoord[0].st * TextureScales.z).rgb;
   vec3 snowColor = texture2D(SnowTexture, gl_TexCoord[0].st * TextureScales.w).rgb;

   float alt = vPos.z * AltitudeScale;

   float grassRange = Altitudes.x;
   float rockRange = Altitudes.y - grassRange;
   float snowRange = Altitudes.z - rockRange;

   float grassFullAlt = Altitudes.x;
   float rockFullAlt = Altitudes.y;
   float snowFullAlt = Altitudes.z;

   float waterRatio = clamp(-alt + WaterSurfaceOffset, 0.0, WaterFadeDepth)/WaterFadeDepth;
   vec3 waterColor = mix(sandColor, WaterColor.rgb, WaterColor.a);

   float grassRatio = clamp(alt, 0.0, grassFullAlt) / grassRange;
   grassRatio = clamp(grassRatio, 0.0, 1.0);

   float rockRatio = clamp(alt - grassFullAlt, 0.0, rockFullAlt) / rockRange;
   rockRatio = clamp(rockRatio, 0.0, 1.0);
   
   float snowRatio = clamp(alt - rockFullAlt, 0.0, snowFullAlt) / snowRange;
   snowRatio = clamp(snowRatio, 0.0, 1.0);

   vec3 baseColor = mix(sandColor, waterColor, waterRatio);

   baseColor = mix(baseColor, grassColor, grassRatio);
   baseColor = mix(baseColor, rockColor, rockRatio);
   baseColor = mix(baseColor, snowColor, snowRatio);

   // normalize all of our incoming vectors
   vec3 lightDir = normalize(vLightDir);
   vec3 viewDir = normalize(vViewDir);
   vec3 reflectionDir = normalize(2.0 * dot(vNormal, lightDir) * vNormal - lightDir);

   float NdotL = saturate(dot(vNormal, lightDir));
   float reflectionAngle =  dot(reflectionDir, viewDir);

   // Calculate the contributions from each shading component
   vec3 ambientColor = vec3(0.2, 0.2, 0.2) * baseColor.rgb;
   vec3 diffuseColor = NdotL * vec3(gl_LightSource[0].diffuse.rgb) * baseColor.rgb;
   
   vec3 result = ambientColor + diffuseColor;

   gl_FragColor = vec4(result, 1.0);
}
