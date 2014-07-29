/*
 * Pass only values which are over a bright treshold.
 * see http://msdn2.microsoft.com/en-us/library/bb173484(VS.85).aspx
 */

// -------------------------------------------------------
// Texture units used for texturing
// -------------------------------------------------------

// Original hdr input 
uniform sampler2D hdrInput;

// Luminance input containing luminance values (mipmapped)
uniform sampler2D lumInput;

// input texture containing the adpted luminance
uniform sampler2D texAdaptedLuminance;

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
 * Perform passing of bright values. The bright values
 * will then be scaled according to the adapted luminance.
 **/
void main(void)
{
	const float BRIGHT_PASS_THRESHOLD = 0.9;
	const float BRIGHT_PASS_OFFSET = 1.0;
		
	// get luminance and average (adapted) luminance value 
	float fLuminance = texture2D(lumInput, gl_TexCoord[0].st).r;
	float fAdaptedLum = texture2D(texAdaptedLuminance, vec2(0.5,0.5)).w;
    float fScaledLum = computeScaledLuminance(fAdaptedLum, fLuminance);

    // get color of the pixel 
    vec3 vSample = texture2D(hdrInput, gl_TexCoord[0].st).rgb;

	// Determine what the pixel's value will be after tone mapping occurs
    vSample *= fScaledLum;
    //vSample.rgb *= g_fMiddleGray/(fScaledLum + 0.001);
	
	// Subtract out dark pixels
	vSample -= BRIGHT_PASS_THRESHOLD;
	
	// Clamp to 0
	vSample = max(vSample, vec3(0.0, 0.0, 0.0));
	
	// Map the resulting value into the 0 to 1 range. Higher values for
	// BRIGHT_PASS_OFFSET will isolate lights from illuminated scene 
	// objects.
	vSample /= (BRIGHT_PASS_OFFSET + vSample);

	// resulting color
	gl_FragColor.rgb = vSample;
    gl_FragColor.a = fAdaptedLum;
}
