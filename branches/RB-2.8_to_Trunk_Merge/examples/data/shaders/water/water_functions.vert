//////////////////////////////////////////////////////////////////////////////////////////////////
//The wave parameters are packed into two vec4's like so
// [Length, Speed, Amplitude, Frequency], [Q, reserved for later use, Direction.x, Direction.y]
//////////////////////////////////////////////////////////////////////////////////////////////////

const int WAVE_OFFSET = 0;
const int NUM_WAVES = 16;
const int MAX_WAVES = 32;


uniform vec4 waveArray[2 * MAX_WAVES];
uniform float elapsedTime;
uniform float WaterHeight;
uniform mat4 inverseViewMatrix;
uniform vec3 cameraRecenter;


float GetHeightOnWaterSuface(vec2 point)
{

   vec4 camPos = inverseViewMatrix[3];
   float distance = length(camPos.xy - point);
   
   float height = WaterHeight;

   vec2 offsetPos = point.xy - cameraRecenter.xy;
   // There are 2 vec4's of data per wave, so the loop is MAX_WAVES * 2 but increments by 2's
   
   int offset = WAVE_OFFSET;   
   for(int i = 2 * offset; i < (offset + NUM_WAVES) * 2; i+=2)
   {           
      float waveLen = waveArray[i].x;
      float speed = waveArray[i].y;
      float freq = waveArray[i].w;
      float amp = waveArray[i].z;
      vec2 waveDir = waveArray[i + 1].zw;
      float k = max(1.5 * waveArray[i+1].x, 4.00001);
      
      //using approximation here because the waves scale out with distance to avoid aliasing with the grid
      float distBetweenVertsScalar = 2.5 + clamp(distance / 15.0, 0.0, 1000.0);
      amp *= 1.0 - clamp((distBetweenVertsScalar) / (waveLen), 0.0, 0.999);
      
      float mPlusPhi =  (freq * (speed * elapsedTime + offsetPos.x * waveDir.x + waveDir.y * offsetPos.y)); 
      float sinDir = pow((sin(mPlusPhi) + 1.0) / 2.0, k);
      
      height += amp * sinDir;
   }
   
   return height;
}
