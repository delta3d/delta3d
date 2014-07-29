/*
 * Compute logarithmic luminance for next mipmap level.
 * see http://msdn2.microsoft.com/en-us/library/bb173484(VS.85).aspx
 */

// -------------------------------------------------------
// Texture units used for texturing
// -------------------------------------------------------
// just input texture from the previous stage
uniform sampler2D texUnit0;

// width of the input texture 
uniform float osgppu_ViewportWidth;

// height of the input texture 
uniform float osgppu_ViewportHeight;

// current mipmap level where we render the output
uniform float osgppu_MipmapLevel;

// number of mipmap levels available (needed for Shader Model 3.0 hardware)
uniform float osgppu_MipmapLevelNum;


/**
 * Do compute current luminance value. If we are in the last mipmap level
 * then do compute the exponent and adapted luminance value.
 **/
void main(void)
{
    // just some variables
    const float epsilon = 0.001;
    float res = 0.0;
    float c[4];
    
    // get texture sizes of the previous level
    vec2 size = vec2(osgppu_ViewportWidth, osgppu_ViewportHeight) * 2.0;

    // this is our starting sampling coordinate 
    vec2 iCoord = gl_TexCoord[0].st;

    // this represent the step size to sample the pixels from previous mipmap level
    vec2 texel = vec2(1.0, 1.0) / (size);
    vec2 halftexel = vec2(0.5, 0.5) / size;

    // create offset for the texel sampling (TODO check why -1 seems to be correct)    
    vec2 st[4];
    st[0] = iCoord - halftexel + vec2(0,0);
    st[1] = iCoord - halftexel + vec2(texel.x,0);
    st[2] = iCoord - halftexel + vec2(0,texel.y);
    st[3] = iCoord - halftexel + vec2(texel.x,texel.y);
    
    // retrieve 4 texels from the previous mipmap level
    for (int i=0; i < 4; i++)
    {
        // map texels coordinates, such that they do stay in defined space
        st[i] = clamp(st[i], vec2(0,0), vec2(1,1));
        
        // get texel from the previous mipmap level
        //c[i] = texelFetch2D(texUnit0, ivec2(size * st[i]), (int)osgppu_MipmapLevel - 1).r;
        c[i] = texture2D(texUnit0, st[i], osgppu_MipmapLevel - 1.0).r;
    }

    // if we compute the first mipmap level, then just compute the sum
    // of the log values
    if (abs(osgppu_MipmapLevel - 1.0) < 0.00001)
    {
        res += log(epsilon + c[0]);
        res += log(epsilon + c[1]);
        res += log(epsilon + c[2]);
        res += log(epsilon + c[3]);

    // for the rest we just compute the sum of underlying values
    }else
    {
        res += c[0];
        res += c[1];
        res += c[2];
        res += c[3];
    }

    // normalize result
    res *= 0.25;

    // if we are in the last mipmap level
    if (osgppu_MipmapLevelNum - osgppu_MipmapLevel < 2.0)
    {
        // exponentiate
        res = exp(res);
    }

    // result
    gl_FragData[0].rgba = vec4( min(res, 65504.0) );
    //gl_FragData[0].a = 1.0;
}
