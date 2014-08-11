
attribute vec4 tangentAttrib;

varying vec3 vLightDir;
varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vWorldNormal;
varying vec3 vCamera;

uniform mat4 osg_ViewMatrixInverse;

float calculateDistance(mat4, vec4);

void main()
{   
   //Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0;

   //moves the position, normal, and light direction into world space   
   vPos = (osg_ViewMatrixInverse * gl_ModelViewMatrix * gl_Vertex).xyz;
   mat3 inverseView3x3 = mat3(osg_ViewMatrixInverse[0].xyz, osg_ViewMatrixInverse[1].xyz, osg_ViewMatrixInverse[2].xyz);

   vCamera = osg_ViewMatrixInverse[3].xyz;
   vNormal = inverseView3x3 * gl_NormalMatrix * gl_Normal;

   vLightDir = normalize(inverseView3x3 * gl_LightSource[0].position.xyz);

   vWorldNormal = gl_Normal;
   
   //Compute the final vertex position in clip space.
   gl_Position = ftransform();

}
