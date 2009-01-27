uniform mat4 osg_ViewMatrixInverse;

varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vViewDir;

void main()
{   
   //Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0;

   vLightDir = normalize(-gl_LightSource[0].position.xzy);
   vLightDir.y = -vLightDir.y;
   
   // View position  -  vertex position
   vViewDir = normalize(osg_ViewMatrixInverse[3].xyz - gl_Vertex.xyz);
   
   mat4 worldMatrix = gl_ModelViewMatrix * osg_ViewMatrixInverse;
   
   // Get the world space normal
   vec3 test = (gl_NormalMatrix * gl_Normal);
   test = osg_ViewMatrixInverse * vec4(test, 0.0);
   vNormal = normalize(test);
  
   //vNormal = normalize(gl_NormalMatrix * gl_Normal);
   
   //Compute the final vertex position in clip space.
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
