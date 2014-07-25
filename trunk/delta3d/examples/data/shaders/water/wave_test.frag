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
   vec2 waveCoords = gl_FragCoord.xy / 1024;
   vec3 waveNormal = texture2D(waveTexture, waveCoords);
   gl_FragColor = vec4(vec3(waveNormal), 1.0);
   
}
