//uniform float TimeDilation;
uniform float MoveXDilation;
uniform float MoveYDilation;
uniform float MoveZDilation;

// Vertex - Simple blimp shader for 'Hover' and 'Highlight' 
// Lighting was removed for simplicity
void main()
{
   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

   gl_Vertex.x += 1.5 * sin(3.14159 * MoveXDilation);
   gl_Vertex.y += 1.5 * sin(3.14159 * MoveYDilation);
   gl_Vertex.z += 2.0 * sin(3.14159 * MoveZDilation);

   gl_Position = ftransform();
}
