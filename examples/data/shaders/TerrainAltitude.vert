
attribute vec4 tangentAttrib;

varying vec3 vLightDir;
varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vWorldNormal;

void main()
{   
   //Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0;

   vWorldNormal = gl_Normal;
   vNormal = normalize(gl_NormalMatrix * gl_Normal);
   vec3 lightDir = normalize(gl_LightSource[0].position.xyz);
   vLightDir = lightDir;
   
   vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

   //Compute the final vertex position in clip space.
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   vPos = gl_Vertex.xyz;
}
