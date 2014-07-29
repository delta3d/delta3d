/*
 * Combine input texture with a given hdr texture by applying
 * tonemapping algorithm found in
 * http://www.mpi-inf.mpg.de/resources/hdr/peffects/krawczyk05sccg.pdf
 */

// -------------------------------------------------------
// Texture units used for texturing
// -------------------------------------------------------
// blurred texture of the brightpassed data
uniform sampler2D blurInput;

// hdr texture containing the scene
uniform sampler2D hdrInput;

// Luminance input 
uniform sampler2D lumInput;

// input texture containing the adpted luminance
uniform sampler2D texAdaptedLuminance;

// how much to use blurred 
uniform float fBlurFactor;

// this gives us middle gray value 
uniform float g_fMiddleGray;


/**
 * Scale luminance value according to the settings
 * @param lum Current luminance value 
 * @param avg Acerage luminance value
**/
float computeScaledLuminance(float avg, float lum)
{
    // compute scaled luminance
    float scaledLum = lum * (g_fMiddleGray / (avg + 0.001));
    
    // clamp to fp16 value 
    scaledLum = min(scaledLum, 65504.0);
    
    // compute new luminance for the color
    return scaledLum / (1.0 + scaledLum);    
}


/**
 **/
void main(void)
{
	vec2 inTex = gl_TexCoord[0].st;
	
	// get color from the texture blurred texture
	vec4 blurColor = texture2D(blurInput, inTex);
	
	// get color from the input texture 
	vec4 hdrColor = texture2D(hdrInput, inTex);

	// get adapted, normal and scaled luminance
    float fLuminance = texture2D(lumInput, inTex).r;
	float fAdaptedLum = texture2D(texAdaptedLuminance, vec2(0.5,0.5)).w;
    float fScaledLum = computeScaledLuminance(fAdaptedLum, fLuminance);

	// resulting color is the hdr color multiplied by the scaled luminance
	vec4 color = hdrColor * fScaledLum;

	// gamma correction
	gl_FragColor.rgb = blurColor.rgb * fBlurFactor + color.rgb;
    gl_FragColor.a = 1.0;   
}
