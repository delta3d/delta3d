#version 120
attribute vec4 tangentAttrib;
//attribute vec4 gl_Color;

varying vec3 vLightDir;
varying vec3 vLightDir2;
varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vWorldNormal;
varying vec3 vCamera;
varying vec4 vViewPos;

uniform mat4 osg_ViewMatrixInverse;
uniform mat4 inverseViewMatrix;

float calculateDistance(mat4, vec4);
void GenerateShadowTexCoords( in vec4 ecPosition );

void main()
{   
   //Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0;  
   gl_FrontColor = gl_Color;

   vec4  ecPos  = gl_ModelViewMatrix * gl_Vertex;
   GenerateShadowTexCoords(ecPos);

   //moves the position, normal, and light direction into world space   
   vPos = (osg_ViewMatrixInverse * gl_ModelViewMatrix * gl_Vertex).xyz;
   mat3 inverseView3x3 = mat3(osg_ViewMatrixInverse[0].xyz, osg_ViewMatrixInverse[1].xyz, osg_ViewMatrixInverse[2].xyz);

   vCamera = inverseViewMatrix[3].xyz;
   vNormal = inverseView3x3 * gl_NormalMatrix * gl_Normal;

   vLightDir = normalize(inverseView3x3 * gl_LightSource[0].position.xyz);
   vLightDir2 = normalize(inverseView3x3 * gl_LightSource[1].position.xyz);

   vWorldNormal = gl_Normal;
   vViewPos = gl_ModelViewMatrix * gl_Vertex;   

   //Compute the final vertex position in clip space.
   gl_Position = ftransform();

}
