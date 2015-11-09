#version 120

uniform vec4 effectScales;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D illumTexture;
uniform sampler2D normalTexture;
uniform sampler2D alphaTexture;
uniform float d3d_SceneLuminance = 1.0;

uniform bool d3d_ShadowOnlyPass = false;

//TODO- this needs to be set from a property on the water
uniform vec4 WaterColor;
uniform float WaterHeight;
uniform float ReflectMode;
uniform float UnderWaterViewDistance;

varying vec3 vNormal;
varying vec3 vTangent;
varying vec3 vLightDir;
varying vec3 vLightDir2;
varying vec3 vPos;
varying vec3 vCamera;
varying vec4 vViewPos;



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
   float refractionIndex;
};



// External Functions
void computeMultiMapColor(MapParams m, inout FragParams f, inout EffectParams e);
float SampleShadowTexture();
float computeFragDepth(float);
float computeExpFog(float);
float computeLinearFog(float, float, float); //for underwater fog


vec4 combineEffects(EffectParams e)
{
   vec4 result = e.colorContrib;
   result.rgb *= e.lightContrib.rgb;
   result.rgb += e.envContrib.rgb * e.envContrib.a;
   result.rgb += e.specContrib.rgb * e.specContrib.a;
   result.rgb += e.illumContrib.rgb;
   
   // Shadow
   //float shadowAmt = SampleShadowTexture();
   //result.rgb *= shadowAmt;
   
   // Fog
   //result.rgb = mix(result.rgb, e.fogContrib.rgb, e.fogContrib.a);

   return result;
}

void main(void)
{
   if(d3d_ShadowOnlyPass)
   {
      return;
   }
   
   float altitude = ReflectMode * vPos.z;

   if(altitude < WaterHeight)
   {
      if(ReflectMode < 0.0)
      {
         discard;
      }
   }
   
   vec3 ecPosition = vec3(vViewPos.xyz) / vViewPos.w;
   float dist = length(ecPosition);
   vec3 viewDir = ecPosition / dist;
   
   vec4 zeroVec = vec4(0,0,0,0);
   vec2 uv = gl_TexCoord[0].xy;

   float alphaScale = effectScales.x;
   float illumScale = effectScales.y;
   
   FragParams f;
   f.uv = uv;
   f.pos = vPos;
   f.normal = normalize(vNormal);
   f.viewDir = normalize(viewDir);
   f.cameraPos = vCamera;
   f.color = gl_Color;
   f.sceneLuminance = d3d_SceneLuminance;
   f.tbn[0] = normalize(vTangent);
   f.tbn[1] = normalize(cross(f.tbn[0], f.normal));
   f.tbn[2] = f.normal;
   
   EffectParams e;
   e.lightContrib = zeroVec.rgb;
   e.specContrib = zeroVec;
   e.envContrib = zeroVec;
   e.fogContrib = zeroVec;
   e.illumContrib = zeroVec;
 
   MapParams m;
   vec3 alpha = texture2D(alphaTexture, uv).rgb * alphaScale;
   m.diffuse.rgb = texture2D(diffuseTexture, uv).rgb;
   m.diffuse.a = alpha.r;
   m.specular.rgb = texture2D(specularTexture, uv).rgb;
   m.specular.a = alpha.g;
   m.illum.rgb = texture2D(illumTexture, uv).rgb * illumScale;
   m.normal.rgb = texture2D(normalTexture, uv).rgb;
   m.irradiance = vec4(0,0,0,0);
   m.refractionIndex = alpha.b;
   
   computeMultiMapColor(m, f, e);
   
   vec4 result = combineEffects(e);
   
   vec3 fogColor = gl_Fog.color.rgb;

   // --- WATER SECTION --- //
   //This adds the under water effects 
   float fogAmt = 0.0;

   if(altitude < WaterHeight)
   {
      //camera height over the water
      float heightOverWater = 20.5 * max(vCamera.z - WaterHeight, 0.0);

      fogAmt = computeLinearFog(0.0, 3.0 * UnderWaterViewDistance, (dist - heightOverWater));

      //fade under water fog in over depth
      float depth = clamp(WaterHeight - altitude, 0.0, 30.0 * UnderWaterViewDistance);
      vec3 waterColorAtDepth = 0.85 * WaterColor.rgb;
      fogColor = (gl_LightSource[0].ambient.rgb * waterColorAtDepth)  + (gl_LightSource[0].diffuse.rgb * waterColorAtDepth);
      
      //considering the underwater color essentially removing light
      result.rgb = mix(result.rgb, 1.2 * vec3(result.rgb * WaterColor.rgb), depth / (3.0 * UnderWaterViewDistance));
   }
   else
   {
      fogAmt = computeExpFog(dist);
   }

   result.rgb = mix(fogColor, result.rgb, fogAmt);
   
   gl_FragColor = result;

   // DEBUG:
   /*vec3 oneVec = vec3(1,1,1);
   vec3 tan = (f.tbn[0] + oneVec)*0.5;
   vec3 bitan = (f.tbn[1] + oneVec)*0.5;
   vec3 norm = (f.tbn[2] + oneVec)*0.5;
   vec3 mNorm = (m.normal.rgb + oneVec)*0.5;
   vec3 worldNorm = (f.worldNormal + oneVec)*0.5;
   vec3 normVaried = (normalize(vNormal) + oneVec)*0.5;*/
   //gl_FragColor = vec4(m.diffuse.rgb,1.0);
   
   
}
