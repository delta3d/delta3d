uniform mat4 osg_ViewMatrixInverse;

uniform mat4 RotationMatrix; //Rotation matrix from model

varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vViewDir;
varying vec3 position;	//view space pos

void main()
{   
   //Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0;

   vLightDir = normalize(-gl_LightSource[0].position.xzy);
   vLightDir.y = -vLightDir.y;
   
   // View position  -  vertex position
   vViewDir = normalize(osg_ViewMatrixInverse[3].xyz - gl_Vertex.xyz);
   
   mat4 worldMatrix = gl_ModelViewMatrix * osg_ViewMatrixInverse;
   
   vNormal = gl_NormalMatrix * gl_Normal;
   position = normalize(gl_ModelViewMatrix * gl_Vertex);
   
   //Compute the final vertex position in clip space.
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
