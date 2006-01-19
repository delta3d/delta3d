//
// Cloud Vertex Shader
//
// Author: George Tarantilis
//
// Inspired by the cloud shader of OGL Orange Book

varying vec3  ModelPosition;

uniform float Scale;

void main(void)
{
    ModelPosition = vec3 (gl_Vertex) * Scale;
    
    gl_FrontColor = gl_Color;
    
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
   	gl_Position = ftransform();	
}
