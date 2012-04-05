uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D specTexture;

uniform int mode;

varying vec3 vNormal;
varying vec3 vViewDir;
varying vec3 vLightDir;

float saturate(float inValue)
{
   return clamp(inValue, 0.0, 1.0);
}

void main(void)
{  
   vec4 baseColor = texture2D(diffuseTexture, gl_TexCoord[0].st);
   vec4 normalColor = texture2D(normalTexture, gl_TexCoord[0].st);
   vec4 specColor = texture2D(specTexture, gl_TexCoord[0].st);
   
   vec3 normal = normalize((normalColor.xyz * 2.0) - 1.0);

   // normalize all of our incoming vectors
   vec3 lightDir = normalize(vLightDir);
   vec3 viewDir = normalize(vViewDir);
   vec3 reflectionDir = normalize(2.0 * dot(normal, lightDir) * normal - lightDir);

   float NdotL = saturate(dot(normal, lightDir));
   float reflectionAngle =  dot(reflectionDir, viewDir);

   // Calculate the contributions from each shading component
   vec3 ambientColor = vec3(0.2, 0.2, 0.2) * baseColor.rgb;
   vec3 diffuseColor = NdotL * vec3(gl_LightSource[0].diffuse.rgb) * baseColor.rgb;
   vec3 specularColor = specColor.rgb * pow(max(0.0, reflectionAngle), 8.0);

   vec3 result = ambientColor + diffuseColor + specularColor;

   if (mode == 0)
   {
      gl_FragColor = vec4(result, 1.0);
   }
   else if (mode == 1)
   {
      gl_FragColor = baseColor;
   }
   else if (mode == 2)
   {
      gl_FragColor = normalColor;
   }
   else if (mode == 3)
   {
      gl_FragColor = specColor;
   }
   else if (mode == 4)
   {
      gl_FragColor = vec4(NdotL, NdotL, NdotL, 1.0);
   }
   else if (mode == 5)
   {
      gl_FragColor = vec4(ambientColor, 1.0);
   }
   else if (mode == 6)
   {
      gl_FragColor = vec4(diffuseColor, 1.0);
   }
   else if (mode == 7)
   {
      gl_FragColor = vec4(specularColor, 1.0);
   }
   else if (mode == 8)
   {
      gl_FragColor = vec4(lightDir, 1.0);
   }
   else if (mode == 9)
   {
      gl_FragColor = vec4(reflectionDir, 1.0);
   } 
}
