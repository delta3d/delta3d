#version 120 
#extension GL_EXT_geometry_shader4 : enable 

varying in vec4 color[];
varying out vec4 colorOut;

// a passthrough geometry shader for color and position
void main()
{ 
  for(int i = 0; i < gl_VerticesIn; ++i)
  {
    // copy position
    gl_Position = gl_PositionIn[i];

    colorOut = color[i];

    // done with the vertex 
    EmitVertex();
  }

  EndPrimitive();
}





