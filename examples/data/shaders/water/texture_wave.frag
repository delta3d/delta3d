//////////////////////////////////////////////
//A generic ocean water shader
//by Bradley Anderegg
//////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//The wave parameters are packed into two vec4's like so
// [Length, Speed, Steepness, Frequency]
//////////////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D reflectionMap;
uniform float elapsedTime;
uniform mat4 inverseViewMatrix;
uniform float textureWaveChopModifier;
uniform float WaterHeight;
uniform float waveDirection;

const int CURWAVE=0;
const int numWaves = 32;//16;
const float twoPI = 6.283185;


float kArray[32];
float waveLengthArray[32];
float waveSpeedArray[32];


void main (void)
{  
   kArray[0] = 1.330;
   kArray[1] = 1.760;
   kArray[2] = 3.000;
   kArray[3] = 2.246;
   kArray[4] = 1.000;
   kArray[5] = 3.710;
   kArray[6] = 1.000;
   kArray[7] = 1.750;
   kArray[8] = 1.500;
   kArray[9] = 1.000;
   kArray[10] = 1.000;
   kArray[11] = 2.000;
   kArray[12] = 2.200;
   kArray[13] = 2.000;
   kArray[14] = 1.113;
   kArray[15] = 1.000;
   kArray[16] = 1.330;
   kArray[17] = 1.760;
   kArray[18] = 3.000;
   kArray[19] = 2.246;
   kArray[20] = 1.000;
   kArray[21] = 3.710;
   kArray[22] = 1.000;
   kArray[23] = 1.750;
   kArray[24] = 1.500;
   kArray[25] = 1.000;
   kArray[26] = 1.000;
   kArray[27] = 2.000;
   kArray[28] = 2.200;
   kArray[29] = 2.000;
   kArray[30] = 1.113;
   kArray[31] = 1.000;

   waveLengthArray[0] = 0.178800;
   waveLengthArray[1] = 0.053500;
   waveLengthArray[2] = 0.121860;
   waveLengthArray[3] = 0.240000;
   waveLengthArray[4] = 0.140000;
   waveLengthArray[5] = 0.116844;
   waveLengthArray[6] = 0.974370;
   waveLengthArray[7] = 0.080500;
   waveLengthArray[8] = 0.067000;
   waveLengthArray[9] = 0.356500;
   waveLengthArray[10] = 0.671350;
   waveLengthArray[11] = 0.191000;
   waveLengthArray[12] = 0.155000;
   waveLengthArray[13] = 0.139170;
   waveLengthArray[14] = 0.275000;
   waveLengthArray[15] = 0.448000;
   waveLengthArray[16] = 0.178800;
   waveLengthArray[17] = 0.053500;
   waveLengthArray[18] = 0.121860;
   waveLengthArray[19] = 0.240000;
   waveLengthArray[20] = 0.140000;
   waveLengthArray[21] = 0.116844;
   waveLengthArray[22] = 0.974370;
   waveLengthArray[23] = 0.080500;
   waveLengthArray[24] = 0.067000;
   waveLengthArray[25] = 0.356500;
   waveLengthArray[26] = 0.671350;
   waveLengthArray[27] = 0.191000;
   waveLengthArray[28] = 0.155000;
   waveLengthArray[29] = 0.139170;
   waveLengthArray[30] = 0.275000;
   waveLengthArray[31] = 0.448000;

   waveSpeedArray[0] = 0.095300;
   waveSpeedArray[1] = 0.038390;
   waveSpeedArray[2] = 0.031100;
   waveSpeedArray[3] = 0.042210;
   waveSpeedArray[4] = 0.114970;
   waveSpeedArray[5] = 0.143213;
   waveSpeedArray[6] = 0.145710;
   waveSpeedArray[7] = 0.051181;
   waveSpeedArray[8] = 0.014730;
   waveSpeedArray[9] = 0.153100;
   waveSpeedArray[10] = 0.213100;
   waveSpeedArray[11] = 0.022100;
   waveSpeedArray[12] = 0.121497;
   waveSpeedArray[13] = 0.121300;
   waveSpeedArray[14] = 0.145710;
   waveSpeedArray[15] = 0.118100;
   waveSpeedArray[16] = 0.095300;
   waveSpeedArray[17] = 0.038390;
   waveSpeedArray[18] = 0.031100;
   waveSpeedArray[19] = 0.042210;
   waveSpeedArray[20] = 0.114970;
   waveSpeedArray[21] = 0.143213;
   waveSpeedArray[22] = 0.145710;
   waveSpeedArray[23] = 0.051181;
   waveSpeedArray[24] = 0.014730;
   waveSpeedArray[25] = 0.153100;
   waveSpeedArray[26] = 0.213100;
   waveSpeedArray[27] = 0.022100;
   waveSpeedArray[28] = 0.121497;
   waveSpeedArray[29] = 0.121300;
   waveSpeedArray[30] = 0.145710;
   waveSpeedArray[31] = 0.118100;

   vec3 camPos = inverseViewMatrix[3].xyz;
                                    
   
   float resolutionScalar = 1.0 + clamp(floor(sqrt(camPos.z - WaterHeight) / 2.75), 0.0, 3.0); 
   float ampOverLength = 1.0 / (512.0 * resolutionScalar);

   vec3 textureNormal = vec3(0.0, 0.0, 0.0);  
   for(int i = 0; i < numWaves; ++i)
   {   
      float waveLength = waveLengthArray[i];
   
      float dir = pow(-1.0, float(i)) * float(i) * (textureWaveChopModifier / float(numWaves));
      float dirAsRad = radians(dir);//radians(waveDirArray[i]);
      //float dirAsRad = radians(waveDirArray[i]);
      float dirCos = cos(radians(waveDirection) + dirAsRad);
      float dirSin = sin(radians(waveDirection) + dirAsRad);
      vec2 waveDir = vec2(dirSin, dirCos);
      
      float freq = twoPI / waveLength;
      float amp = waveLength * ampOverLength;
      float steepness = 4.0;
      float speed = waveSpeedArray[i];      
   
      //speed * freq * time   
      float phi = 0.5 * speed * freq * elapsedTime;
      
      vec2 fragCoord = gl_FragCoord.xy;

      vec2 resolution = fragCoord / (128.0 * resolutionScalar);
      //float twoLSqrd = pow(2.0 * waveLength, 2.0);
      //vec2 tilingSize = sqrt(twoLSqrd / dot(waveDir, waveDir));
      //tilingSize *= (1.0 + int(1.0 / waveLength));
           
      //freq * waveDir DOT vertex
      //float m = dot( freq * waveDir, resolution * tilingSize);      
      float m = dot( freq * waveDir, resolution);                

      float k = 1.0;//1.1 * steepness;
      float vertexDerivativeScalar = freq * amp * pow((sin(m + phi) + 1.0) * 0.5, k - 1.0) * cos(m + phi);

      textureNormal.x += k * waveDir.x * vertexDerivativeScalar;
      textureNormal.y += k * waveDir.y * vertexDerivativeScalar;

   }

   textureNormal.x = -textureNormal.x;
   textureNormal.y = -textureNormal.y;
   textureNormal.z = 1.0;
   
   textureNormal = normalize(textureNormal); 

   //scale it into color space
   textureNormal += 1.0;
   textureNormal /= 2.0;
    
   gl_FragColor = vec4(textureNormal, 0.0);
   //gl_FragColor = vec4(waveDirArray[i].x, waveDirArray[i].y, 0.0, 1.0);
}
