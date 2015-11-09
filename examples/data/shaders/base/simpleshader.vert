#version 120

varying vec3 vLightDir;
varying vec3 vLightDir2;
varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vCamera;

uniform mat4 osg_ViewMatrixInverse;

void GenerateShadowTexCoords( in vec4 ecPosition );

void main(void)
{
   gl_TexCoord[0] = gl_MultiTexCoord0;
   
   vec4  ecPos  = gl_ModelViewMatrix * gl_Vertex;
   GenerateShadowTexCoords(ecPos);

   vPos = (osg_ViewMatrixInverse * gl_ModelViewMatrix * gl_Vertex).xyz;
   mat3 inverseView3x3 = mat3(osg_ViewMatrixInverse[0].xyz, osg_ViewMatrixInverse[1].xyz, osg_ViewMatrixInverse[2].xyz);

   vCamera = osg_ViewMatrixInverse[3].xyz;
   vNormal = inverseView3x3 * gl_NormalMatrix * gl_Normal;
   
   vLightDir = normalize(inverseView3x3 * gl_LightSource[0].position.xyz);
   vLightDir2 = normalize(inverseView3x3 * gl_LightSource[1].position.xyz);

   gl_Position = ftransform();
}

