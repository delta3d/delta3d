#version 120

uniform vec4 effectScales;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D illumTexture;
uniform sampler2D normalTexture;
uniform sampler2D alphaTexture;
uniform float d3d_SceneLuminance; // = 1.0;
uniform bool d3d_DepthOnlyPass = false;
uniform bool d3d_ShadowOnlyPass = false;

varying vec3 vNormal;
varying vec3 vTangent;
varying vec3 vBitangent;
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
};



// External Functions
void computeMultiMapColor(MapParams m, inout FragParams f, inout EffectParams e);
float SampleShadowTexture();
float computeFragDepth(float);


vec4 combineEffects(EffectParams e)
{
   vec4 result = e.colorContrib;
   result.rgb *= e.lightContrib.rgb;
   result.rgb += e.envContrib.rgb * e.envContrib.a;
   result.rgb += e.specContrib.rgb * e.specContrib.a;
   result.rgb += e.illumContrib.rgb;
   
   float shadowAmt = SampleShadowTexture();
   result.rgb *= shadowAmt;

   return result;
}

void main(void)
{
   vec3 ecPosition = vec3(vViewPos.xyz) / vViewPos.w;
   float dist = length(ecPosition);
   vec3 viewDir = ecPosition / dist;
   
   //if your shader ever sets the frag depth it must always
   if(d3d_DepthOnlyPass)
   {
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
   f.tbn[1] = normalize(vBitangent);
   f.tbn[2] = f.normal;
   
   EffectParams e;
   e.lightContrib = zeroVec.rgb;
   e.specContrib = zeroVec;
   e.envContrib = zeroVec;
   e.fogContrib = zeroVec;
   e.illumContrib = zeroVec;
 
   MapParams m;
   vec2 alpha = texture2D(alphaTexture, uv).rg * alphaScale;
   m.diffuse.rgb = texture2D(diffuseTexture, uv).rgb;
   m.diffuse.a = alpha.r;
   m.specular.rgb = texture2D(specularTexture, uv).rgb;
   m.specular.a = alpha.g;
   m.illum.rgb = texture2D(illumTexture, uv).rgb * illumScale;
   m.normal.rgb = normalize(texture2D(normalTexture, uv).rgb);
   m.irradiance = vec4(0,0,0,0);
   
   computeMultiMapColor(m, f, e);
   
   vec4 result = combineEffects(e);
   gl_FragColor = result;

   // DEBUG:
   //gl_FragColor = vec4( (vec3(1.0) + f.worldNormal.rgb) * 0.5,1.0);
   //gl_FragColor = vec4(m.specular.rgb,1.0);
   //gl_FragColor = vec4(mNorm,1.0);
   //gl_FragColor = vec4(e.specContrib.rgb,1.0);
   //gl_FragColor = vec4(e.specContrib.rgb * m.diffuse.rgb,1.0);
   //gl_FragColor = vec4(e.lightContrib.rgb,1.0);
   //gl_FragColor = vec4((e.lightContrib.rgb + f.envContrib.rgb) * m.diffuse.rgb,1.0);
   //gl_FragColor = vec4(e.lightContrib.rgb * m.diffuse.rgb,1.0);
   //gl_FragColor = vec4(e.envContrib.rgb + m.diffuse.rgb,1.0);
   //gl_FragColor = vec4(e.envContrib.rgb,1.0);
   //gl_FragColor = vec4(e.illumContrib.rgb,1.0);
   //gl_FragColor = vec4(tan,1.0);
   //gl_FragColor = vec4(bitan,1.0);
   //gl_FragColor = vec4(norm,1.0);
   //gl_FragColor = vec4(worldNorm,1.0);
   //gl_FragColor = vec4(worldNorm.rrr,1.0);
   //gl_FragColor = vec4(normVaried,1.0);
   //gl_FragColor = vec4(f.viewDir,1.0);
   //gl_FragColor = vec4(gl_Color.rgb,1.0);
   //gl_FragColor = vec4(e.colorContrib.aaa, 1.0);
   
   
}
