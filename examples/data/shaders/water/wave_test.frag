#version 120
//////////////////////////////////////////////
//A generic ocean water shader
//by Bradley Anderegg
//////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//The wave parameters are packed into two vec4's like so
// [Length, Speed, Amplitude, Frequency], [Q, reserved for later use, Direction.x, Direction.y]
//////////////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D waveTexture;

void main (void)
{   
   //test texture waves   
   vec2 waveCoords = vec2(gl_FragCoord.x / 1024.0, gl_FragCoord.y / 1024.0);
   vec3 waveNormal = texture2D(waveTexture, waveCoords.xy).rgb;
   gl_FragColor = vec4(waveNormal.rgb, 1.0);
   
}
