#version 120


void main()
{   
   // Pass the texture coordinate on through.
   gl_TexCoord[0] = gl_MultiTexCoord0;
      
   // Compute the final vertex position in clip space.
   gl_Position = ftransform();

}
