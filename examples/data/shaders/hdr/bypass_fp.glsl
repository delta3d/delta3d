/*
 * Reimplement fixed function pipeline of OpenGL
 * So bypass all the data.
 */

// -------------------------------------------------------
// Texture units used for texturing
// -------------------------------------------------------
uniform sampler2D texUnit0;


/**
 * Reimplement fixed pipeline
 **/
void main(void)
{
	// get color from the texture
	vec4 texColor0 = texture2D(texUnit0, gl_TexCoord[0].st);
	
	// combine texture color with the vertex color
	gl_FragData[0] = texColor0;
}
