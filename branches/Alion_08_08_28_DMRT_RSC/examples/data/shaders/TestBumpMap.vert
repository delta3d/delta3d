
attribute vec4 TangentAttrib;

varying vec3 lightDir;
varying vec3 eyeDir;

void main()
{   
   //Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0;     
   
   vec3 lightPosition = normalize(gl_LightSource[0].position.xyz);
   
   vec3 normal    = normalize(gl_NormalMatrix * gl_Normal);
   vec3 tangent   = normalize(gl_NormalMatrix * TangentAttrib.xyz);
   vec3 bitangent = cross(normal, tangent);   
 
   // gram-schmidt tangent orthonormalization
   float tangentDOTnormal = dot(tangent, normal);
   vec3 tempNormal = normal * tangentDOTnormal;
   tangent -= tempNormal;   
 
   float bitangentDOTnormal = dot(bitangent, normal);
   tempNormal = normal * bitangentDOTnormal;
   float bitangentDOTtangent = dot(bitangent, tangent);
   vec3 tempTan = tangent * bitangentDOTtangent; 
   vec3 tempBi = tempNormal - tempTan;
   bitangent -= tempBi;
   
   vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
   
   vec3 temp;
   temp.x = dot(lightPosition, tangent);
   temp.y = dot(lightPosition, bitangent);
   temp.z = dot(lightPosition, normal);
   lightDir = normalize(temp);
   
   eyeDir = vec3(gl_ModelViewMatrix * gl_Vertex);   
   
   temp.x = dot(eyeDir, tangent);
   temp.y = dot(eyeDir, bitangent);
   temp.z = dot(eyeDir, normal);
   eyeDir = normalize(temp);   

   //Compute the final vertex position in clip space.
   gl_Position = ftransform();   
}