#version 120
//////////////////////////////////////////////
//This shader is used to color the far plane with 
// the underwater color to give the effect of an ocean
//by Bradley Anderegg
//////////////////////////////////////////////

uniform mat4 modelViewProjectionInverse;

varying vec4 worldSpacePos;

const float FarPlane = 100.0;

void main(void)
{    
    //transform our vector into screen space
   gl_Position = ftransform();   

   vec4 screenPos = gl_Position;   
   screenPos.z = 1.0;
   worldSpacePos = (modelViewProjectionInverse * screenPos);

}




