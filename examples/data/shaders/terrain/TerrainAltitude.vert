
attribute vec4 tangentAttrib;

varying vec3 vLightDir;
varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vWorldNormal;
varying vec3 vCamera;

uniform mat4 inverseViewMatrix;

float calculateDistance(mat4, vec4);

void main()
{   
   //Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0;
   gl_FogFragCoord = gl_FogCoord;

   //moves the position, normal, and light direction into world space   
   vPos = (inverseViewMatrix * gl_ModelViewMatrix * gl_Vertex).xyz;
   mat3 inverseView3x3 = mat3(inverseViewMatrix[0].xyz, inverseViewMatrix[1].xyz, inverseViewMatrix[2].xyz);

   vCamera = inverseViewMatrix[3].xyz;
   vNormal = inverseView3x3 * gl_NormalMatrix * gl_Normal;

   vLightDir = normalize(inverseView3x3 * gl_LightSource[0].position.xyz);

   vWorldNormal = gl_Normal;
   
   vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

   //Compute the final vertex position in clip space.
   gl_Position = ftransform();

}
