#version 120


uniform mat4 osg_ViewMatrixInverse;


void main(void)
{
   gl_TexCoord[0] = gl_MultiTexCoord0;
   
   vPos = (osg_ViewMatrixInverse * gl_ModelViewMatrix * gl_Vertex).xyz;
   mat3 inverseView3x3 = mat3(osg_ViewMatrixInverse[0].xyz, osg_ViewMatrixInverse[1].xyz, osg_ViewMatrixInverse[2].xyz);
   
   gl_Position = ftransform();
}

