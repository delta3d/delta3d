uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

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
   
   vec3 normal = normalize((normalColor.xyz * 2.0) - 1.0);

   // normalize all of our incoming vectors
   vec3 lightDir = normalize(vLightDir);
   vec3 viewDir = normalize(vViewDir);
   vec3 reflectionDir = normalize(2.0 * dot(normal, lightDir) * normal - lightDir);

   float NdotL = saturate(dot(normal, lightDir));
   float reflectionAngle =  dot(reflectionDir, viewDir);

   // Calculate the contributions from each shading component
   vec3 ambientColor = vec3(gl_LightSource[0].ambient.rgb);
   vec3 diffuseColor = NdotL * vec3(gl_LightSource[0].diffuse.rgb);
   vec3 specularColor = NdotL * vec3(gl_LightSource[0].specular.rgb) * pow(max(0.0, reflectionAngle), 128.0);

   vec3 result = (ambientColor + diffuseColor) * baseColor + specularColor;  

   gl_FragColor = vec4(result, 1.0);
}
