/*
 * Just render the scene in an ususal way. Output linear z-depth values
 * in the second texture coordiantes.
 */

// zNear and zFar values 
uniform float zNear;
uniform float zFar;

/**
 **/
void main(void)
{
   // compute simple diffuse lighting, so that the scene get more pleasant
   vec3 eyeToLight = normalize(-gl_TexCoord[2]);
   vec4 diffuse = gl_Color * max(dot(eyeToLight, normalize(gl_TexCoord[0].xyz)), 0.0) ;
   gl_FragData[0].rgb = diffuse;


   // Use face normals instead of interpolated normals to make sure that the tangent planes
   // associated with the normals are actually tangent to the surface
   gl_FragData[1].xyz = normalize(cross(ddx(gl_TexCoord[1].xyz), ddy(gl_TexCoord[1].xyz)));

   // compute linear z-depth value
   gl_FragData[1].w = gl_TexCoord[0].w / zFar;
}
