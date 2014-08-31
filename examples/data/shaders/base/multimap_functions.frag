#version 120

varying vec3 vLightDir;
varying vec3 vLightDir2;



// External Functions
void lightContribution(vec3, vec3, vec3, vec3, out vec3);
float computeLinearFog(float, float, float);
float computeExpFog(float);
vec3 computeWorldSpaceNormal(vec3 vertPos, vec3 vertNormal, vec3 mapNormal, vec2 vertUV, out mat3 tbn);
vec2 computeSphereMapCoord(in vec3 viewDir, in vec3 normal);
vec3 sampleCubeMapReflection(vec3 worldPos, vec3 camPos, vec3 normal);


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
   vec4 roughness;
   vec4 illum;
   vec4 irradiance;
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
   
   // Sun
   lp.color = gl_LightSource[0].diffuse.rgb;
   lp.colorAmbient = gl_LightSource[0].ambient.rgb;
   lp.dir = normalize(vLightDir);
   
   computeLightContrib(lp, fp, ep);
   
   // Moon
   /*lp.color = gl_LightSource[1].diffuse.rgb;
   lp.colorAmbient = gl_LightSource[1].ambient.rgb;
   lp.dir = normalize(vLightDir2);

   computeLightContrib(lp, fp, ep);*/
}

vec4 computeMultiMapColor(MapParams mp, inout FragParams fp, inout EffectParams ep)
{
   vec2 uv = gl_TexCoord[0].xy;
   
   vec4 diffuseColor = mp.diffuse;
   vec4 normalColor = mp.normal;
   vec4 specColor = mp.specular;
   vec4 illumColor = mp.illum;
   
   //mat3 tbn;
   //fp.worldNormal = computeWorldSpaceNormal(fp.pos, normalize(fp.normal), normalize(normalColor.rgb), uv, fp.tbn);
   
   fp.worldNormal = normalize((2.0 * normalize(normalColor.rgb)) - vec3(1.0, 1.0, 1.0));
   fp.worldNormal = normalize(fp.tbn * fp.worldNormal);

   // Normalize all incoming vectors 
   vec3 viewDir = normalize(fp.viewDir);
   
   ep.colorContrib = diffuseColor * fp.color;
   vec3 color = ep.colorContrib.rgb;

   // Compute the Light & Spec Contribution
   computeLightContrib_SunMoon(fp, ep);

   // Compute the reflection contribution
   //vec3 reflectVec = reflect(fp.viewDir, fp.worldNormal);
   vec3 reflectVec = reflect(fp.viewDir, fp.worldNormal.xyz);
   float reflectionAngle =  dot(reflectVec, fp.viewDir);
   float reflectContrib = max(0.0,reflectionAngle);
   
   vec3 minLightSpec = min(ep.lightContrib.rgb, specColor.rgb);
   
   vec3 reflectCubeMap = sampleCubeMapReflection(fp.pos, fp.cameraPos, fp.worldNormal);
   ep.envContrib = vec4(reflectCubeMap, reflectContrib); 
   ep.envContrib *= mp.specular;
   color = mix(ep.lightContrib * color, reflectCubeMap, minLightSpec);

   // Don't apply specular greater than the light contrib or objects will glow in the dark...
   ep.specContrib.a *= specColor.a;
   float specLevel = ep.specContrib.a;
   ep.specContrib.rgb = ep.lightContrib * specColor.rgb * specLevel;
   ep.illumContrib.rgb += fp.sceneLuminance * illumColor.rgb;
   color += min(ep.specContrib.rgb, ep.lightContrib.rgb) + illumColor.rgb;
   
   // Apply Fog 
   float dist = length(fp.pos - fp.cameraPos);
   float fogAmt = computeExpFog(dist);
   ep.fogContrib = vec4(gl_Fog.color.rgb, fogAmt);
   
   vec4 result;
   //result.rgb = mix(ep.fogContrib.rgb, color, fogAmt);
   
   // Comput final alpha.
   result.rgb = color;
   result.a = max(specLevel, diffuseColor.a);
   
   return result;
}
