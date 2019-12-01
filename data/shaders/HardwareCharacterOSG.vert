#version 120

/*  -*-c++-*- 
 *  Copyright (C) 2008 Cedric Pinson <cedric.pinson@plopbyte.net>
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

in vec4 boneWeight0;
in vec4 boneWeight1;
in vec4 boneWeight2;
in vec4 boneWeight3;

uniform vec4 boneTransforms[MAX_BONES * 3];
uniform scale = 1.0;

vec4 transformedPosition;
vec4 transformedNormal;

void main( void )
{
    transformedPosition = vec4(0.0,0.0,0.0,1.0);
	transformedNormal = vec4(0.0,0.0,0.0,1.0);
	float boneWeights[4];
	float boneIndices[4];

	boneIndices[0] = boneWeight0.x;
	boneIndices[1] = boneWeight0.z;
	boneIndices[2] = boneWeight1.x;
	boneIndices[3] = boneWeight1.z;

	boneWeights[0] = boneWeight0.y;
	boneWeights[1] = boneWeight0.w;
	boneWeights[2] = boneWeight1.y;
	boneWeights[3] = boneWeight1.w;
	
	vec4 vert;
	vert.xyz = gl_Vertex.xyz * scale;
	vert.w = 1.0;

    int boneIndex;
    float boneWeight;
    for (int i = 0; i < 4; ++i)
    {
        boneIndex =  int(boneIndices[i]*3.0);
        boneWeight = boneWeights[i];
		
		transformedPosition.x += boneWeight * dot(vert, boneTransforms[boneIndex]);
		transformedPosition.y += boneWeight * dot(vert, boneTransforms[boneIndex + 1]);
		transformedPosition.z += boneWeight * dot(vert, boneTransforms[boneIndex + 2]);

		transformedNormal.x += boneWeight * dot(gl_Normal.xyz, boneTransforms[boneIndex].xyz);
		transformedNormal.y += boneWeight * dot(gl_Normal.xyz, boneTransforms[boneIndex + 1].xyz);
		transformedNormal.z += boneWeight * dot(gl_Normal.xyz, boneTransforms[boneIndex + 2].xyz);
    }

    gl_FrontColor = vec4(1.0,0.0,0.0,1.0);//gl_Color;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_MultiTexCoord1;
    gl_TexCoord[2] = transformedNormal;

    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * transformedPosition;

    vec4 ecPosition = gl_ModelViewMatrix * vert;
    vec3 ecPosition3 = vec3(ecPosition) / ecPosition.w;
    gl_FogFragCoord = length(ecPosition3);
}
