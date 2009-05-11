uniform float TimeDilation;
uniform float MoveXDilation;
uniform float MoveYDilation;
uniform float MoveZDilation;

varying vec3 vNormal;
varying vec3 vHalfVec;
varying vec3 vLightDir;

void main()
{
   vNormal = gl_NormalMatrix * gl_Normal;
   vLightDir = normalize(vec3(gl_LightSource[0].position));
   vHalfVec = normalize(vec3(gl_LightSource[0].halfVector));

   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

   gl_Vertex.x += 1.5 * sin(3.14159 * MoveXDilation);
   gl_Vertex.y += 1.5 * sin(3.14159 * MoveYDilation);
   gl_Vertex.z += 2.0 * sin(3.14159 * MoveZDilation);

   gl_Position = ftransform();
}
