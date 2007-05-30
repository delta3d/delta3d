uniform sampler2D diffuseTexture;

varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vHalfVec;

void main()
{
   //For now this constant is defined..  this value should, however, be set
   //and used through gl_LightSource...
   const vec3 SPECULAR_COLOR = vec3(0.3,0.3,0.3);

   vec3 color = vec3(gl_LightSource[0].ambient);
   vec4 diffuseColor = texture2D(diffuseTexture,gl_TexCoord[0].st);
   vec3 normal = normalize(vNormal);
   vec3 lightDir = normalize(vLightDir);

   float diffuseContrib = max(dot(lightDir,normal),0.0);
   if (diffuseContrib > 0.0)
   {
      //Diffuse...
      color += (vec3(gl_LightSource[0].diffuse) * vec3(diffuseColor)) * diffuseContrib;

      //Specular...
      vec3 halfV = normalize(vHalfVec);
      color += SPECULAR_COLOR * pow(max(0.0,dot(normal,halfV)),16.0);
   }

   gl_FragColor = vec4(color,diffuseColor.a);
}
