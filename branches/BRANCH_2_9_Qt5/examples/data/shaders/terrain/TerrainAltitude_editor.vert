#version 120

varying vec4 vPos;
varying vec3 vNormal;

uniform mat4 osg_ViewMatrixInverse;


void main()
{   
   //Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0;

   //moves the position, normal, and light direction into world space   
   vPos = osg_ViewMatrixInverse * gl_ModelViewMatrix * gl_Vertex;
   mat3 inverseView3x3 = mat3(osg_ViewMatrixInverse[0].xyz, osg_ViewMatrixInverse[1].xyz, osg_ViewMatrixInverse[2].xyz);

   vNormal = inverseView3x3 * gl_NormalMatrix * gl_Normal;

   //Compute the final vertex position in clip space.
   gl_Position = ftransform();

}
