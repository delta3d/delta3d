#version 120

varying vec4 vertexColor;
varying float vDistance;

float calculateDistance(mat4, vec4);

void main()
{
   gl_Position = ftransform();
   gl_TexCoord[0] = gl_MultiTexCoord0; 
   vertexColor = gl_Color;

   vDistance = calculateDistance(gl_ModelViewMatrix, gl_Vertex);
}
