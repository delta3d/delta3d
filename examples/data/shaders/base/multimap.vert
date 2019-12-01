#version 120

varying vec3 vLightDir;
varying vec3 vLightDir2;
varying vec3 vNormal;
varying vec3 vTangent;
varying vec3 vPos;
varying vec3 vCamera;
varying vec4 vViewPos;

uniform mat4 osg_ViewMatrixInverse = mat4(1.0);

void sphereMap(vec3, vec3, out vec2);
float calculateDistance(mat4, vec4);
void GenerateShadowTexCoords( in vec4 ecPosition );

void main()
{   
   // Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0;
   
   gl_FrontColor = gl_Color;

   vec4  ecPos  = gl_ModelViewMatrix * gl_Vertex;
   GenerateShadowTexCoords(ecPos);

   // Moves the position, normal, and light direction into world space   
   vPos = (osg_ViewMatrixInverse * gl_ModelViewMatrix * gl_Vertex).xyz;
   mat3 inverseView3x3 = mat3(osg_ViewMatrixInverse[0].xyz, osg_ViewMatrixInverse[1].xyz, osg_ViewMatrixInverse[2].xyz);

   vCamera = osg_ViewMatrixInverse[3].xyz;
   
   vNormal = normalize(inverseView3x3 * gl_NormalMatrix * gl_Normal);
   vTangent = normalize(inverseView3x3 * gl_NormalMatrix * gl_MultiTexCoord1.xyz);
   
   vTangent = normalize(vTangent - dot(vTangent, vNormal) * vNormal);

   vLightDir = normalize(inverseView3x3 * gl_LightSource[0].position.xyz);
   vLightDir2 = normalize(inverseView3x3 * gl_LightSource[1].position.xyz);
   
   vViewPos = gl_ModelViewMatrix * gl_Vertex;
   
   // Compute the final vertex position in clip space.
   gl_Position = ftransform();

}
