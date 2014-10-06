#version 120 
#extension GL_EXT_geometry_shader4 : enable 

varying in vec4 color[];
varying out vec4 colorOut;

uniform float osg_FrameTime;

// a passthrough geometry shader for color and position
void main()
{ 
  vec4 faceEdgeA = gl_PositionIn[1] - gl_PositionIn[0];
  vec4 faceEdgeB = gl_PositionIn[2] - gl_PositionIn[0];
  vec3 faceNormal = normalize(cross(faceEdgeB.xyz, faceEdgeA.xyz));

  for(int i = 0; i < gl_VerticesIn; ++i)
  {
    gl_Position = gl_PositionIn[i];
    gl_Position.xyz += faceNormal * 3.0 * abs(sin(2.0 * osg_FrameTime));

    colorOut = color[i];

    // done with the vertex 
    EmitVertex();
  }

  EndPrimitive();
}





