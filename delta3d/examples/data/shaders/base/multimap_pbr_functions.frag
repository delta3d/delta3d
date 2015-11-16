#version 120

varying vec3 vLightDir;
varying vec3 vLightDir2;

uniform samplerCube d3d_ReflectionCubeMap;

//used for HDR
uniform float d3d_SceneLuminance = 1.0;
uniform float d3d_SceneAmbience = 1.0;
uniform float d3d_Exposure = 1.0;


// External Functions
void lightContribution(vec3, vec3, vec3, vec3, out vec3);
float computeLinearFog(float, float, float);
float computeExpFog(float);
vec3 computeWorldSpaceNormal(vec3 vertPos, vec3 vertNormal, vec3 mapNormal, vec2 vertUV, out mat3 tbn);
vec2 computeSphereMapCoord(in vec3 viewDir, in vec3 normal);
vec3 sampleCubeMapReflection(vec3 worldPos, vec3 camPos, vec3 normal);
vec3 computeDynamicLightContrib(vec3 wsNormal, vec3 wsPos);
float computeReflectionCoef(vec3 viewDir, vec3 viewSpaceNormal, float refractIndex);


struct FragParams
{
   vec3 pos;
   vec2 uv;
   vec4 color; // varied from vertex shader
   vec3 normal; // varied from vertex shader
   vec3 worldNormal;
   vec3 viewDir;
   vec3 cameraPos;
   mat3 tbn;
   float sceneLuminance;
};

struct EffectParams
{
   vec4 colorContrib;
   vec3 lightContrib;
   vec4 specContrib;
   vec4 envContrib;
   vec4 illumContrib; // self-glow
   vec4 fogContrib;
};

struct MapParams
{
   vec4 diffuse;
   vec4 normal;
   vec4 specular;
   vec4 reflectance;
   vec4 illum;
   vec4 irradiance;
   float refractionIndex;
};

struct LightParams
{
   vec3 pos;
   vec3 dir; // relative to fragment
   vec3 color;
   vec3 colorAmbient;
};



void computeLightContrib(LightParams lp, inout FragParams fp, inout EffectParams ep)
{
   vec3 lightContrib;

   lightContribution(fp.worldNormal, lp.dir, lp.color, lp.colorAmbient, lightContrib);
   
   // Compute the specular & reflection contribution
   vec3 reflectVec = reflect(lp.dir, fp.worldNormal);
   float reflectionAngle =  dot(reflectVec, fp.viewDir);
   float reflectContrib = max(0.0,reflectionAngle);
   
   float specLevel = (pow(reflectContrib, 16.0));
   
   ep.specContrib.a += specLevel;
   ep.lightContrib += lightContrib;
}

void computeLightContrib_SunMoon(inout FragParams fp, inout EffectParams ep)
{
   LightParams lp;
   
   // Moon
   lp.color = gl_LightSource[1].diffuse.rgb;
   lp.colorAmbient = gl_LightSource[1].ambient.rgb;
   lp.dir = normalize(vLightDir2);
   
   computeLightContrib(lp, fp, ep);


   // Sun
   //we give a little extra brightness to the sunlight using d3d_SceneLuminance
   lp.color = d3d_SceneLuminance * gl_LightSource[0].diffuse.rgb;
   lp.colorAmbient = gl_LightSource[0].ambient.rgb;
   lp.dir = normalize(vLightDir);
   
   computeLightContrib(lp, fp, ep);
   
}

void computeRefraction(inout FragParams fp, inout MapParams mp, inout EffectParams ep)
{
   if(mp.diffuse.a < 1.0 && mp.refractionIndex > 0.0)
   {
      float fresnel = computeReflectionCoef(fp.viewDir, fp.worldNormal, 1.02);

      
      vec3 refractionCoords = refract(normalize(fp.pos - fp.cameraPos), fp.worldNormal, 1.02);
      vec3 refractionColor = textureCube(d3d_ReflectionCubeMap, refractionCoords.xyz).rgb;

      ep.colorContrib.rgb *= refractionColor;//(ep.specContrib.a)* mix(ep.colorContrib.rgb, refractionColor, fresnel);
      ep.colorContrib.a = 1.0;
   }
}


void computeMultiMapColor(inout MapParams mp, inout FragParams fp, inout EffectParams ep)
{
   vec2 uv = gl_TexCoord[0].xy;
   
   vec4 diffuseColor = mp.diffuse;
   vec4 normalColor = mp.normal;
   vec4 illumColor = mp.illum;
   float smoothness = 1.0-mp.reflectance.g;
   vec4 specColor = vec4(mix(vec3(1.0,1.0,1.0), diffuseColor.rgb, mp.reflectance.r).rgb, clamp(smoothness, 0.0, 1.0));

   mp.specular = specColor;
      
   //fp.worldNormal = (2.0 * normalColor.rgb) - vec3(1.0, 1.0, 1.0);
   //fp.worldNormal = normalize(fp.tbn * fp.worldNormal);
   
   fp.worldNormal = normalize(fp.tbn * normalColor.rgb);
   fp.worldNormal = normalize((2.0 * fp.worldNormal.rgb) - vec3(1.0, 1.0, 1.0));

   // Normalize all incoming vectors 
   vec3 viewDir = normalize(fp.viewDir);
   
   ep.colorContrib = diffuseColor * fp.color;

   //computeRefraction(fp, mp, ep);

   
   // Compute the Light & Spec Contribution
   computeLightContrib_SunMoon(fp, ep);

   //add dynamic lights
   vec3 dynamicLightContrib = computeDynamicLightContrib(fp.worldNormal, fp.pos);
   ep.lightContrib = clamp(ep.lightContrib + dynamicLightContrib, vec3(0.0), vec3(d3d_Exposure));


   // Compute the reflection contribution
   vec3 reflectVec = reflect(fp.viewDir, fp.worldNormal.xyz);
   float reflectionAngle =  dot(reflectVec, fp.viewDir);
   float reflectContrib = max(0.0,reflectionAngle) * smoothness;
   
   vec3 minLightSpec = min(ep.lightContrib.rgb, specColor.rgb);
   
   vec3 reflectCubeMap = sampleCubeMapReflection(fp.pos, fp.cameraPos, fp.worldNormal);
   ep.envContrib = vec4(reflectCubeMap, reflectContrib); 
   ep.envContrib *= mp.specular;
   
   // Don't apply specular greater than the light contrib or objects will glow in the dark...
   ep.specContrib.a *= specColor.a;
   float specLevel = ep.specContrib.a;
   ep.specContrib.rgb = ep.lightContrib * specColor.rgb * specLevel;
   ep.illumContrib.rgb += fp.sceneLuminance * illumColor.rgb;
   
   // Apply Fog 
   float dist = length(fp.pos - fp.cameraPos);
   float fogAmt = computeExpFog(dist);
   ep.fogContrib = vec4(gl_Fog.color.rgb, fogAmt);
   
}

void computeMultiMapColorSimple(MapParams mp, inout FragParams fp, inout EffectParams ep)
{
   vec2 uv = gl_TexCoord[0].xy;
   
   vec4 diffuseColor = mp.diffuse;
   vec4 normalColor = mp.normal;
   vec4 specColor = mp.specular;
   vec4 illumColor = mp.illum;
      
   fp.worldNormal = fp.normal;
   
   ep.colorContrib = diffuseColor * fp.color;

   // Compute the Light & Spec Contribution
   computeLightContrib_SunMoon(fp, ep);

   //add dynamic lights
   vec3 dynamicLightContrib = computeDynamicLightContrib(fp.worldNormal, fp.pos);
   ep.lightContrib = clamp(ep.lightContrib + dynamicLightContrib, vec3(0.0), vec3(d3d_Exposure));


   // Compute the reflection contribution
   vec3 reflectVec = reflect(fp.viewDir, fp.worldNormal.xyz);
   float reflectionAngle =  dot(reflectVec, fp.viewDir);
   float reflectContrib = max(0.0,reflectionAngle);
   
   vec3 minLightSpec = min(ep.lightContrib.rgb, specColor.rgb);
   
   vec3 reflectCubeMap = sampleCubeMapReflection(fp.pos, fp.cameraPos, fp.worldNormal);
   ep.envContrib = vec4(reflectCubeMap, reflectContrib); 
   ep.envContrib *= mp.specular;
   
   // Don't apply specular greater than the light contrib or objects will glow in the dark...
   ep.specContrib.a *= specColor.a;
   float specLevel = ep.specContrib.a;
   ep.specContrib.rgb = ep.lightContrib * specColor.rgb * specLevel;
   ep.illumContrib.rgb += fp.sceneLuminance * illumColor.rgb;
   
   // Apply Fog 
   float dist = length(fp.pos - fp.cameraPos);
   float fogAmt = computeExpFog(dist);
   ep.fogContrib = vec4(gl_Fog.color.rgb, fogAmt);
   
}
