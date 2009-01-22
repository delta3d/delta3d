uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

uniform int mode;

varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vViewDir;

float saturate(float inValue)
{
   return clamp(inValue, 0.0, 1.0);
}

void main(void)
{  
   vec4 baseColor = texture2D(diffuseTexture, gl_TexCoord[0].st);
   vec4 normalColor = texture2D(normalTexture, gl_TexCoord[0].st);
   
   vec4 normal = normalize((normalColor * 2.0) - 1.0);
   //normal.y = -normal.y;
   
   float originalNdotL = saturate(dot(vNormal, vLightDir));
   float NdotL = saturate(dot(vec3(normal), vLightDir));
   
   vec3 viewDir = normalize(vViewDir);
   vec3 reflectionDir = normalize(2.0 * NdotL * vec3(normal) - vLightDir);
   
   float reflectionAngle =  saturate(dot(reflectionDir, viewDir));
   float specularFactor = 0.25;
   
   // Calculate the contributions from each shading component
   vec3 ambientColor  = baseColor.xyz * 0.2;
   vec3 diffuseColor  = NdotL * baseColor.xyz * 0.8;
   vec3 specularColor = vec3(pow(max(0.0, reflectionAngle), 16.0)) * specularFactor;
   
   vec3 finalColor;
   
   if (mode == 1)
   {
	  finalColor = baseColor;
   }
   else if (mode == 2)
   {
	  finalColor = normalColor;
   }
   else if (mode == 3)
   {
	  finalColor = ambientColor;
   }
   else if (mode == 4)
   {
      finalColor = diffuseColor;
   }
   else if (mode == 5)
   { 
	  finalColor = specularColor;
   }
   else if (mode == 6)
   {
      finalColor = vec3(NdotL);
   }
   else if (mode == 7)
   {
	  finalColor = normal;
   }
   else if (mode == 8)
   {
      finalColor = vNormal.xyz;
   }
   else if (mode == 9)
   {
      //finalColor = vViewDir;
	  finalColor = normalize(vLightDir);
   }   
   else if (mode == 0)
   {
      finalColor = ambientColor + diffuseColor + specularColor;
   }
   else
   {
      gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
   }
   
   gl_FragColor = vec4(finalColor, 1.0);
}
