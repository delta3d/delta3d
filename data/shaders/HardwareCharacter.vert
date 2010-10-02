
/*
 *  This shader provides hardware skinning and is used in dtAnim
 *  Bradley Anderegg
 */

const int MAX_BONES = 82;
uniform vec4 boneTransforms[MAX_BONES * 3];

void main(void)
{
   //initialize our data
   vec4 transformedPosition = vec4(0.0, 0.0, 0.0, 1.0);
   vec4 transformedNormal = vec4(0.0, 0.0, 0.0, 1.0);
   float boneWeights[4];//float[](gl_MultiTexCoord2.x, gl_MultiTexCoord2.y, gl_MultiTexCoord2.z, gl_MultiTexCoord2.w);
   float boneIndices[4];//int[](gl_MultiTexCoord3.x, gl_MultiTexCoord3.y, gl_MultiTexCoord3.z, gl_MultiTexCoord3.w);
   
   boneWeights[0] = gl_MultiTexCoord2.x; boneWeights[1] = gl_MultiTexCoord2.y; boneWeights[2] = gl_MultiTexCoord2.z; boneWeights[3] = gl_MultiTexCoord2.w;
   boneIndices[0] = gl_MultiTexCoord3.x; boneIndices[1] = gl_MultiTexCoord3.y; boneIndices[2] = gl_MultiTexCoord3.z; boneIndices[3] = gl_MultiTexCoord3.w;


   //multiply each bone and weight to get the offset matrix
   for(int i = 0; i < 4; ++i)
   {
      int boneIndex = int(boneIndices[i] * 3.0);
      float boneWeight = boneWeights[i];

      transformedPosition.x += boneWeight * dot(gl_Vertex, boneTransforms[boneIndex]);
      transformedPosition.y += boneWeight * dot(gl_Vertex, boneTransforms[boneIndex + 1]);
      transformedPosition.z += boneWeight * dot(gl_Vertex, boneTransforms[boneIndex + 2]);

      transformedNormal.x += boneWeight * dot(gl_Normal.xyz, boneTransforms[boneIndex].xyz);
      transformedNormal.y += boneWeight * dot(gl_Normal.xyz, boneTransforms[boneIndex + 1].xyz);
      transformedNormal.z += boneWeight * dot(gl_Normal.xyz, boneTransforms[boneIndex + 2].xyz);
   }

   //set proper varyings
   gl_FrontColor = gl_Color;
   gl_TexCoord[0] = gl_MultiTexCoord0;
   gl_TexCoord[1] = gl_MultiTexCoord1;
   gl_TexCoord[2] = transformedNormal;
   
   gl_Position = gl_ModelViewProjectionMatrix * transformedPosition;
   
   vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
   vec3 ecPosition3 = vec3(ecPosition) / ecPosition.w;
   gl_FogFragCoord = length(ecPosition3);
}

