uniform sampler2D diffuseTexture;
uniform sampler2D detailTexture;

varying vec3 vNormal;
varying vec3 vHalfVec;
varying vec3 vLightDir;

/**
 * This is the fragment shader to a simple set of shaders that calculates
 * per pixel lighting assuming one directional light source.  It also
 * combines a diffuse texture located in texture unit 0 with a detail map
 * located in texture unit 1.
 */
void main()
{
   //For now this constant is defined..  this value should, however, be set
   //and used through gl_LightSource...
   const vec3 SPECULAR_COLOR = vec3(0.3,0.3,0.3);

   vec4 diffuseColor = texture2D(diffuseTexture,gl_TexCoord[0].st);
   vec3 detailColor = vec3(texture2D(detailTexture,gl_TexCoord[0].st * 8.0)) * 0.5;

   vec3 color = vec3(gl_LightSource[0].ambient);
   vec3 normal = normalize(vNormal);
   vec3 lightDir = normalize(vLightDir);

   float diffuseContrib = max(dot(lightDir,normal),0.0);
   if (diffuseContrib > 0.0)
   {
      //Diffuse + Texture + DetailMap
      color += (vec3(gl_LightSource[0].diffuse) * (vec3(diffuseColor) * 0.5) + detailColor) * diffuseContrib;

      //Specular...
      vec3 halfV = normalize(vHalfVec);
      color += SPECULAR_COLOR * pow(max(0.0,dot(normal,halfV)),16.0);
   }

   gl_FragColor = vec4(color,diffuseColor.a);
}
