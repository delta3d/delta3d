uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D illumTexture;
uniform sampler2D normalTexture;
uniform sampler2D envTexture;

varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vPos;
varying vec3 vCamera;
varying vec2 vReflectTexCoord;
varying vec3 vViewDir;

float saturate(float inValue)
{
   return clamp(inValue, 0.0, 1.0);
}

void lightContribution(vec3, vec3, vec3, vec3, out vec3);
float computeLinearFog(float, float, float);
float computeExpFog(float);
mat3 compute_tangent_frame_O3(vec3 N, vec3 p, vec2 uv);

void main(void)
{
   vec3 diffuseColor = texture2D(diffuseTexture, gl_TexCoord[0].st).rgb;
   vec3 specColor = texture2D(specularTexture, gl_TexCoord[0].st).rgb;
   vec3 illumColor = texture2D(illumTexture, gl_TexCoord[0].st).rgb;
   vec3 normalColor = texture2D(normalTexture, gl_TexCoord[0].st).rgb;
   vec3 envColor = texture2D(envTexture, vReflectTexCoord).rgb;
   
   // Change the range from  [-1, 1] to [0, 1]
   vec3 tangentSpaceNormal = (2.0 * normalColor.rgb) - vec3(1.0, 1.0, 1.0);

   vec3 n = normalize(vNormal);
   mat3 TBN = compute_tangent_frame_O3(n, vPos,gl_TexCoord[0].xy);
   
   // Transform the tangent space normal into view space
   vec3 WorldMapNormal;
   WorldMapNormal.xyz = normalize(mul(TBN, tangentSpaceNormal));  

   // Normalize all incoming vectors
   vec3 lightDir = normalize(vLightDir);   
   vec3 viewDir = normalize(vViewDir);

   // Compute the Light Contribution
   vec3 lightContrib;
   lightContribution(WorldMapNormal, lightDir, gl_LightSource[0].diffuse.xyz, gl_LightSource[0].ambient.xyz, lightContrib);
  
   
   // Compute the specular & reflection contribution
   vec3 reflectVec = reflect(vLightDir, WorldMapNormal);
   float reflectionAngle =  dot(reflectVec, viewDir);
   float reflectContrib = max(0.0,reflectionAngle);
   vec3 specularContrib = specColor * (pow(reflectContrib, 16.0));
   
   vec3 minLightSpec = min(lightContrib, specColor);
   vec3 color = mix(lightContrib * diffuseColor, envColor, minLightSpec);
   
   // Don't apply specular greater than the light contrib or objects will glow in the dark...
   color += min(specularContrib, lightContrib) + illumColor;		
   color = clamp(color, 0.0, 1.0);
   
   
   // Apply Fog 
   float dist = length(vPos - vCamera);
   float fogAmt = computeExpFog(dist);
   vec4 fogColor = gl_Fog.color;
   vec3 result = mix(color, fogColor.rgb, fogAmt);
   
   gl_FragColor = vec4(result.rgb, 1.0);//diffuseColor.a);  
}
