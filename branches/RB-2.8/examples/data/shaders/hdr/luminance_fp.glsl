/*
 * Compute luminance values of the input texture.
 * So result will contain only luminance values per pixel.
 */

// -------------------------------------------------------
// Texture units used for texturing
// -------------------------------------------------------
uniform sampler2D texUnit0;


/**
 **/
void main(void)
{
	// get color from the texture
	vec4 texColor0 = texture2D(texUnit0, gl_TexCoord[0].st);

	// compute luminance and output
	gl_FragColor.xyz = vec3( texColor0.r * 0.2125 + texColor0.g * 0.7154 + texColor0.b * 0.0721 );
	gl_FragColor.a = texColor0.a;
}
