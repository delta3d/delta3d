
/* 
*  This shader provides hardware skinning and is used in dtAnim
*  Bradley Anderegg
*/

const int MAX_BONES = 72;
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
		vec4 offset;
		offset.x = boneWeights[i] * dot(gl_Vertex, boneTransforms[int(boneIndices[i]) * 3 + 0]);
		offset.y = boneWeights[i] * dot(gl_Vertex, boneTransforms[int(boneIndices[i]) * 3 + 1]);
		offset.z = boneWeights[i] * dot(gl_Vertex, boneTransforms[int(boneIndices[i]) * 3 + 2]);
		offset.w = 0.0; 
		
		transformedPosition += offset;
		transformedNormal += offset;	  	
	}

	//calculate lighting			
	
	//set proper varyings
	gl_FrontColor = gl_Color;	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;
	gl_TexCoord[2] = transformedNormal;
	
	gl_Position = gl_ModelViewProjectionMatrix * transformedPosition;
}

