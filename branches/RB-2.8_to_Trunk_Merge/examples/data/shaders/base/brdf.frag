#version 120

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D illumTexture;
uniform sampler2D normalTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D envTexture;

varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vLightDir2;
varying vec3 vPos;
varying vec3 vCamera;
varying vec2 vReflectTexCoord;
varying vec3 vViewDir;



// External Types
struct BRDFFragParams
{
   vec3 position;
   vec3 normal; // Varied Surface Normal
   vec2 uv;
   float alpha;
   vec3 albedoColor;
   vec3 normalColor; // Normal Map Texel
   float metallic;
   float roughness;
   vec3 irradianceColor;
   vec3 illumColor;
   vec3 viewDirection;
   vec3 lightDirection;
   vec3 lightPosition;
   vec3 lightColor;
   bool lightIsPoint;
};

// External Functions
void lightContribution(vec3, vec3, vec3, vec3, out vec3);
float computeLinearFog(float, float, float);
float computeExpFog(float);
vec4 computeBRDF(BRDFFragParams params);



float saturate(float inValue)
{
   return clamp(inValue, 0.0, 1.0);
}

void main(void)
{
   BRDFFragParams params;
   
   vec2 uv = gl_TexCoord[0].st;
   
   params.uv = uv;
   params.albedoColor = texture2D(diffuseTexture, uv).rgb;
   params.normalColor = texture2D(normalTexture, uv).xyz;
   params.metallic = texture2D(specularTexture, uv).x;
   params.roughness = texture2D(roughnessTexture, uv).x;
   params.illumColor = texture2D(illumTexture, uv).rgb;
   params.irradianceColor = vec3(0,0,0);
   params.alpha = 1.0f;
   
   params.position = vPos;
   params.normal = vNormal;
   params.viewDirection = normalize(vViewDir);
   params.lightDirection = normalize(vLightDir);
   params.lightPosition = gl_LightSource[0].position.xyz;
   params.lightColor = gl_LightSource[0].diffuse.xyz;
   params.lightIsPoint = false;
    
   vec4 color = computeBRDF(params);
   
   // Apply Fog 
   /*float dist = length(vPos - vCamera);
   float fogAmt = computeExpFog(dist);
   vec4 fogColor = gl_Fog.color;
   vec3 result = mix(fogColor.rgb, color.rgb, fogAmt);*/
   
   gl_FragColor = vec4(color.rgb, 1.0);//vec4(result.rgb, 1.0);//diffuseColor.a);

}
