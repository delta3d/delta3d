#version 120
varying vec3 vNormal;
varying vec3 vViewDir;

varying vec4 vColorAmbientEmissive;

//This vertex shader is meant to perform the per vertex operations of per pixel lighting
//using a single directional light source.
void main()
{
   // Get the normal vector in view space
   vNormal = gl_NormalMatrix * gl_Normal;

   //Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0; 

   // Get the view direction in view space
   vViewDir = (gl_ModelViewMatrix * gl_Vertex).xyz; 

   vColorAmbientEmissive = gl_FrontLightModelProduct.sceneColor + 
                           gl_LightSource[0].ambient * gl_FrontMaterial.ambient;

   //Compute the final vertex position in clip space.
   gl_Position = ftransform(); 
}