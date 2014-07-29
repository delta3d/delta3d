/*
 * Compute adapted luminance based on the data from the previous frames.
 * see http://msdn2.microsoft.com/en-us/library/bb173484(VS.85).aspx
 */

// -------------------------------------------------------
// Texture units used for texturing
// -------------------------------------------------------
// input texture containing the average luminance
uniform sampler2D texLuminance;

// input texture containing the current adapted luminance
uniform sampler2D texAdaptedLuminance;

// max and min possible luminance
uniform float maxLuminance;
uniform float minLuminance;

// time interval between two frames
uniform float invFrameTime;

// scaling factor which decides how fast to adapt for new luminance
uniform float adaptScaleFactor;

const float TauCone = 0.01;
const float TauRod = 0.04;

/**
 * Compute adapted luminance value.
 * @param current Is the current luminance value 
 * @param old Adapted luminance value from the previous frame
 **/
void main(void)
{
    // get current luminance, this one is stored in the last mipmap level
    float current = texture2D(texLuminance, vec2(0.5,0.5), 100.0).x;
    
    // get old adapted luminance value
    float old = texture2D(texAdaptedLuminance, vec2(0.5,0.5)).w;

    //determin if rods or cones are active
    //Perceptual Effects in Real-time Tone Mapping: Equ(7)    
    float sigma = clamp(0.4/(0.04+current),0.0,1.0);

    //interpolate tau from taurod and taucone depending on lum
    //Perceptual Effects in Real-time Tone Mapping: Equ(12)
    float Tau = mix(TauCone,TauRod,sigma) / adaptScaleFactor;

    // compute new adapted value
    //float lum = old + (current - old) * (1.0 - pow(0.98, adaptScaleFactor * invFrameTime));

    // clamp and return back
    //gl_FragData[0].xyzw = lum;//clamp(lum, minLuminance, maxLuminance);
    //gl_FragData[0].a = 1.0;





    //calculate adaption
    //Perceptual Effects in Real-time Tone Mapping: Equ(5)
    float lum  = old + (current - old) * (1.0 - exp(-(invFrameTime)/Tau));
    //gl_FragData[0].x = current;
    //gl_FragData[0].y = old;
    //gl_FragData[0].z = (1.0 - exp(-(invFrameTime)/Tau));
    gl_FragData[0].xyzw = vec4( clamp(lum, minLuminance, maxLuminance) );
}
