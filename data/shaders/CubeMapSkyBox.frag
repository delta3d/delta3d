/* 
*  This shader just does a simple lookup into a cubemap from a 3D vertex
*
*  Bradley Anderegg
*/


uniform samplerCube cubeMap;

varying vec3 texCoord;


void main(void)
{   	
   vec4 color = textureCube(cubeMap, texCoord);
	
   gl_FragColor = color;
}

