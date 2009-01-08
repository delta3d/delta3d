uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

uniform int mode;

varying vec3 lightDir;
varying vec3 eyeDir;

void main(void)
{  
   vec4 baseColor = texture2D(diffuseTexture, gl_TexCoord[0].st);
   vec4 normalColor = texture2D(normalTexture, gl_TexCoord[0].st);
   vec4 normal = normalize((normalColor * 2.0) - 1.0);
   
   float NdotL = clamp(dot(vec3(normal), lightDir), 0.0, 1.0);
   
   vec3 viewDir = normalize(eyeDir);
   vec3 reflectionDir = normalize(2.0 * NdotL * vec3(normal) - lightDir);
   float reflectionAngle =  dot(reflectionDir, viewDir);
   
   // Calculate the contributions from each shading component
   vec3 ambientColor = vec3(gl_LightSource[0].ambient.rgb);
   vec3 diffuseColor = NdotL * vec3(gl_LightSource[0].diffuse.rgb);
   vec3 specularColor = vec3(gl_LightSource[0].specular.rgb) * pow(max(0.0, reflectionAngle), 128.0);

   vec3 lightContrib = ambientColor + diffuseColor + specularColor;
   lightContrib.r = clamp(lightContrib.x, 0.0, 1.0);
   lightContrib.g = clamp(lightContrib.y, 0.0, 1.0);
   lightContrib.b = clamp(lightContrib.z, 0.0, 1.0);

   // compute final color with light and base color
   vec3 result = lightContrib * vec3(baseColor);  
   
   if (mode == 1)
   {
      gl_FragColor = vec4(result, baseColor.a);   
   }
   else if (mode == 2)
   {
      gl_FragColor = baseColor;
   }
   else if (mode == 3)
   {
      gl_FragColor = vec4(normalColor.rgb, 1.0);
   }  
   else if (mode == 4)
   {
      gl_FragColor = vec4(diffuseColor, 1.0);    
   }
   else if (mode == 5)
   {
      gl_FragColor = vec4(specularColor, 1.0);
   }
   else if (mode == 6)
   {
      gl_FragColor = vec4(lightContrib, 1.0);
   }
   else
   {
      // error
      gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
   }
}
