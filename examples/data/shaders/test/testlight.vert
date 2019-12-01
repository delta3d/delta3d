varying vec3 vNormal;
varying vec3 vViewDir;
varying float distance;
varying vec3 vPos;

//This vertex shader is meant to perform the per vertex operations of per pixel lighting
//using a single directional light source.
void main()
{
   const float MAX_DISTANCE = 750.0;

   // Get the normal vector in view space
   vNormal = normalize(gl_NormalMatrix * gl_Normal);   

   //Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0;   

   //Calculate the distance from this vertex to the camera.
   vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
   vec3 ecPosition3 = vec3(ecPosition) / ecPosition.w;
   distance = length(ecPosition3);    

   // Get the view direction in view space
   vViewDir = -ecPosition.xyz;  

   //Compute the final vertex position in clip space.
   gl_Position = ftransform();  
}