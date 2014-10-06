/*
 * Just render the scene in an ususal way. Output linear z-depth values
 * and normal in the second texture coordiantes.
 * Precompute simple lighting model.
 */


/**
 **/
void main(void)
{
   // compute position of the pixel
   gl_Position = ftransform();

   // bypass color data
   gl_FrontColor = gl_Color;

   // compute normal and depth
   gl_TexCoord[0].xyz = normalize(gl_NormalMatrix * gl_Normal);

   // compute linear depth value
   // this is just a simple matrix multiplication
   // the difference is that this value is not divided by the consecutive
   // pipeline as for gl_Position, hence giving us what we have looked for
   gl_TexCoord[0].w = (gl_ModelViewProjectionMatrix * gl_Vertex).z;

   // store here the eye position of the vertex
   gl_TexCoord[1] = gl_ModelViewMatrix * vec4(gl_Vertex.xyz,1);   

   // Transform the global lights to eye space
   gl_TexCoord[2] = gl_ModelViewMatrix * vec4(0.0, 1.0, 0.0, 1.0);
}
