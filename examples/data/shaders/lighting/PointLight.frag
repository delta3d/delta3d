#version 120
// Basic  point light implementation
// MG 12/09/09

////////////////////////////////////////////////////////////////////////////////
float BlinnSpecular(in vec3 lightDir, in vec3 normal)
{
   vec3 eye = vec3(0.0, 0.0, 1.0);
   vec3 halfVector = normalize(lightDir + eye);
   
   return max(0.0, dot(normal, halfVector));
}

////////////////////////////////////////////////////////////////////////////////
float PhongSpecular(in vec3 lightDir, in vec3 normal)
{
   vec3 eye = vec3(0.0, 0.0, 1.0);
   vec3 reflectionDir = normalize(2.0 * dot(normal, lightDir) * normal - lightDir);
   
   return max(0.0, dot(eye, reflectionDir));
}

////////////////////////////////////////////////////////////////////////////////
void PointLight(in int i,
                in vec3 inViewDir,
                in vec3 inNormal,
                inout vec3 diffuse,
                inout vec3 specular)
{
   // normalize all of our incoming vectors
   vec3 normal   = normalize(inNormal);
   vec3 lightDir = gl_LightSource[i].position.xyz - inViewDir;
   
   float dist = length(lightDir);
   lightDir = normalize(lightDir);
   
   vec3 viewDir  = normalize(inViewDir);
   
   float cosAngle        = max(0.0, dot(normal, lightDir));
   float reflectionAngle = PhongSpecular(lightDir, normal);
   
   float powFactor;
   
   // Calculate the reflection Power
   if (reflectionAngle == 0.0)
   {
      powFactor = 0.0;
   }
   else
   {
      powFactor = pow(reflectionAngle, 32.0);//gl_FrontMaterial.shininess);
   }
   
   // Compute attenuation
   float attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
       gl_LightSource[i].linearAttenuation * dist +
       gl_LightSource[i].quadraticAttenuation * dist * dist);
   
   // Calculate the contributions from each shading component
   diffuse += attenuation * cosAngle * gl_LightSource[i].diffuse.rgb;
   specular += attenuation * gl_LightSource[i].specular.rgb * powFactor;
}