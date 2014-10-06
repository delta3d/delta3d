//////////////////////////////////////////////
//An under water shader
//by Bradley Anderegg
//////////////////////////////////////////////

uniform float WaterHeight;
uniform mat4 modelViewProjectionInverse;
uniform mat4 inverseViewMatrix;

varying vec4 worldSpacePos;
varying vec3 lightVector;
varying vec4 camPos;

const float FarPlane = 30000.0;

void main(void)
{    
    //transform our vector into screen space
   gl_Position = ftransform();   

   vec4 screenPos = gl_Position;
   screenPos.z = 0.9999;
   worldSpacePos = (modelViewProjectionInverse * screenPos);

   
   mat3 inverseView3x3 = mat3(inverseViewMatrix[0].xyz, 
       inverseViewMatrix[1].xyz, inverseViewMatrix[2].xyz);
   
   camPos = inverseViewMatrix[3];
   
   //very far off in worldspace
   lightVector = (inverseView3x3 * gl_LightSource[0].position.xyz);
}




