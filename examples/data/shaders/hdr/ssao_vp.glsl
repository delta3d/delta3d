/*
 * SSAO - Technique.
 * This shader is applied on the screen sized quad with scene rendered before as input.
 */


/**
 **/
void main(void)
{
   // bypass the texture coordinate data
   gl_TexCoord[0] = gl_MultiTexCoord0;

   // compute position of the pixel
   gl_Position = ftransform();

   // this is the eyeview vector (just as if the scene were raytraced ;)
   // in eye coordinates
   gl_TexCoord[1] = gl_ModelViewMatrix * gl_Vertex;
}
