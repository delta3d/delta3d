varying vec3 vNormal;
varying vec3 vHalfVec;
varying vec3 vLightDir;

/**
 * This is the vertex shader to a simple set of shaders that calculates
 * per pixel lighting assuming one directional light source.
 */
void main()
{
   vNormal = gl_NormalMatrix * gl_Normal;
   vLightDir = normalize(vec3(gl_LightSource[0].position));
   vHalfVec = normalize(vec3(gl_LightSource[0].halfVector));

   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
   gl_Position = ftransform();
}
