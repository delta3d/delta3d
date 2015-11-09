/*
 * Create Depth-Of-Filed effect by combining the depth map within the image map.
 */

// -------------------------------------------------------
// Texture units used for texturing
// -------------------------------------------------------
// Texture containing the scene (color map)
uniform sampler2D texColorMap;

// Colormap of the scene blurred
uniform sampler2D texBlurredColorMap;

// Colormap of the scene blurred
uniform sampler2D texStrongBlurredColorMap;

// Depth map containing depth values of the image map
uniform sampler2D texDepthMap;

// Focal length of the camear
uniform float focalLength;

// Focal range of the camera 
uniform float focalRange;

// zNear and zFar values 
uniform float zNear;
uniform float zFar;

float convertZ( in float near, in float far, in float depthBufferValue )
{
        float clipZ = ( depthBufferValue - 0.5 ) * 2.0;
        return -(2.0 * far * near) / ( clipZ * ( far - near ) - ( far + near ));
}

/**
 **/
void main(void)
{
    vec2 inTex = gl_TexCoord[0].st;
    
    // compute distance to the viewer
    float a = zFar / ( zFar - zNear );
    float b = zFar * zNear / ( zNear - zFar );
    
    float depth = texture2D( texDepthMap, inTex ).x;
    float dist = b / ( depth - a );
    
    // get color map and blurred color map values 
    vec4 colorValue = texture2D (texColorMap, inTex).rgba;
    vec4 blurredValue1 = texture2D ( texBlurredColorMap, inTex).rgba;
    vec4 blurredValue2 = texture2D ( texStrongBlurredColorMap, inTex).rgba;
    
    // now compute the bluriness value 
    float blur = clamp(abs(dist - focalLength) / focalRange, 0.0, 1.0);
    float factor1 = 1.0;
    float factor2 = 0.0;
    
    // compute blend factors 
    if (blur > 0.5)
        factor2 = (blur - 0.5) * 2.0;
    else
        factor1 = blur * 2.0;
        
    
    // the resulting color value is the combination of blurred and non-blurred map 
    vec4 result = mix(colorValue, blurredValue1, factor1);
    gl_FragColor = mix(result, blurredValue2, factor2);
    
}
