/*
 * Create a post process fog effect using the main scene color and depth buffer.
 */

// Texture containing the scene (color map)
uniform sampler2D texColorMap;

// Depth map containing depth values of the image map
uniform sampler2D texDepthMap;

// zNear and zFar values 
uniform float zNear;
uniform float zFar;

uniform vec4 fogColor;
uniform float fogDensity;

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

	const float LOG2 = 1.442695;

    float fogFactor = exp2( -fogDensity * 
                       fogDensity * 
                       dist * 
                       dist * 
                       LOG2 );

    fogFactor = clamp(fogFactor, 0.0, 1.0);
      
    vec4 result = mix(colorValue, fogColor, 1.0 - fogFactor);
    gl_FragColor = result;
    
}
