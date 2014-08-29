#version 120

/*
 *  This shader provides hardware skinning and is used in dtAnim
 *  Bradley Anderegg
 *  Modifications by Peter Amstutz
 */

const int MAX_BONES = 82;
uniform vec4 boneTransforms[MAX_BONES * 3];
uniform mat4 osg_ViewMatrixInverse = mat4(1.0);

varying vec3 vNormal;
varying vec3 vTangent;
varying vec3 vBitangent;
varying vec3 vPos;
varying vec3 vCamera;
varying vec3 vViewDir;
varying vec3 vLightDir;

attribute vec4 boneWeights;
attribute vec4 boneIndices;
attribute vec4 tangentSpace;

void main(void)
{
   mat3 inverseView3x3 = mat3(osg_ViewMatrixInverse[0].xyz, osg_ViewMatrixInverse[1].xyz, osg_ViewMatrixInverse[2].xyz);

   //initialize our data
   vec4 transformedPosition = vec4(0.0, 0.0, 0.0, 1.0);
   vec4 transformedNormal = vec4(0.0, 0.0, 0.0, 1.0);
   vec4 transformedTangent = vec4(0.0, 0.0, 0.0, 1.0);
   float boneWeightsA[4];
   float boneIndicesA[4];
   
   boneWeightsA[0] = boneWeights.x;
   boneWeightsA[1] = boneWeights.y;
   boneWeightsA[2] = boneWeights.z;
   boneWeightsA[3] = boneWeights.w;
   
   boneIndicesA[0] = boneIndices.x;
   boneIndicesA[1] = boneIndices.y;
   boneIndicesA[2] = boneIndices.z;
   boneIndicesA[3] = boneIndices.w;

   //multiply each bone and weight to get the offset matrix
   for(int i = 0; i < 4; ++i)
   {
      int boneIndex = int(boneIndices[i] * 3.0);
      float boneWeight = boneWeights[i];

      vec4 btx = boneTransforms[boneIndex];
      vec4 bty = boneTransforms[boneIndex + 1];
      vec4 btz = boneTransforms[boneIndex + 2];
      
      transformedPosition.x += boneWeight * dot(gl_Vertex, btx);
      transformedPosition.y += boneWeight * dot(gl_Vertex, bty);
      transformedPosition.z += boneWeight * dot(gl_Vertex, btz);

      transformedNormal.x += boneWeight * dot(gl_Normal.xyz, btx.xyz);
      transformedNormal.y += boneWeight * dot(gl_Normal.xyz, bty.xyz);
      transformedNormal.z += boneWeight * dot(gl_Normal.xyz, btz.xyz);
      
      transformedTangent.x += boneWeight * dot(tangentSpace.xyz, btx.xyz);
      transformedTangent.y += boneWeight * dot(tangentSpace.xyz, bty.xyz);
      transformedTangent.z += boneWeight * dot(tangentSpace.xyz, btz.xyz);
   }

   //set proper varyings
   gl_FrontColor = gl_Color;
   gl_TexCoord[0] = gl_MultiTexCoord0;
   gl_TexCoord[1] = gl_MultiTexCoord1;
   gl_TexCoord[2] = transformedNormal;
   
   vNormal = gl_NormalMatrix * normalize(transformedNormal.xyz);
   vTangent = gl_NormalMatrix * normalize(transformedTangent.xyz);
   vBitangent = gl_NormalMatrix * normalize(cross(transformedNormal.xyz, transformedTangent.xyz) * tangentSpace.w);
   
   gl_Position = gl_ModelViewProjectionMatrix * transformedPosition;
   vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
   vPos = ecPosition.xyz;
   vViewDir = vPos / ecPosition.w;
   
   vLightDir = normalize(inverseView3x3 * gl_LightSource[0].position.xyz);
   
   vCamera = osg_ViewMatrixInverse[3].xyz;
}

