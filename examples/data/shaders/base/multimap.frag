uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D illumTexture;
uniform sampler2D normalTexture;
uniform sampler2D envTexture;

uniform float d3d_SceneLuminance = 1.0;

varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vLightDir2;
varying vec3 vPos;
varying vec3 vCamera;
varying vec2 vReflectTexCoord;
varying vec3 vViewDir;

float saturate(float inValue)
{
   return clamp(inValue, 0.0, 1.0);
}



// External Functions
void lightContribution(vec3, vec3, vec3, vec3, out vec3);
float computeLinearFog(float, float, float);
float computeExpFog(float);
vec3 computeWorldSpaceNormal(vec3 vertPos, vec3 vertNormal, vec3 mapNormal, vec2 vertUV, out mat3 tbn);



void main(void)
{
   vec2 uv = gl_TexCoord[0].xy;
   
   vec3 diffuseColor = texture2D(diffuseTexture, uv).rgb;
   vec3 specColor = texture2D(specularTexture, uv).rgb;
   vec3 illumColor = d3d_SceneLuminance * texture2D(illumTexture, uv).rgb;
   vec3 normalColor = normalize(texture2D(normalTexture, uv).rgb);
   vec3 envColor = texture2D(envTexture, vReflectTexCoord).rgb;
   
   mat3 tbn;
   vec3 WorldMapNormal = computeWorldSpaceNormal(vPos, normalize(vNormal), normalize(normalColor.rgb), uv, tbn);

   // Normalize all incoming vectors
   vec3 lightDir = normalize(vLightDir);   
   vec3 lightDir2 = normalize(vLightDir2);   
   vec3 viewDir = normalize(vViewDir);

   // Compute the Light Contribution
   vec3 lightContribSun;
   vec3 lightContribMoon;

   lightContribution(WorldMapNormal, lightDir, gl_LightSource[0].diffuse.xyz, gl_LightSource[0].ambient.xyz, lightContribSun);
   lightContribution(WorldMapNormal, lightDir2, gl_LightSource[1].diffuse.xyz, gl_LightSource[1].ambient.xyz, lightContribMoon);
  
   vec3 lightContrib = lightContribSun + lightContribMoon;
  
   
   // Compute the specular & reflection contribution
   vec3 reflectVec = reflect(vLightDir, WorldMapNormal);
   float reflectionAngle =  dot(reflectVec, viewDir);
   float reflectContrib = max(0.0,reflectionAngle);
   vec3 specularContrib = specColor * (pow(reflectContrib, 16.0));
   
   vec3 minLightSpec = min(lightContrib, specColor);
   vec3 color = mix(lightContrib * diffuseColor, d3d_SceneLuminance * envColor, minLightSpec);
   
   // Don't apply specular greater than the light contrib or objects will glow in the dark...
   color += min(specularContrib, lightContrib) + illumColor;		
   
   //don't clamp color for hdr
   //color = clamp(color, 0.0, 1.0);
   
   
   // Apply Fog 
   float dist = length(vPos - vCamera);
   float fogAmt = computeExpFog(dist);
   vec4 fogColor = gl_Fog.color;
   vec3 result = mix(fogColor.rgb, color, fogAmt);
   
   gl_FragColor = vec4(color.rgb, 1.0);//diffuseColor.a);
   
}
