//About the most innovative shader ever
//but it shows what is the bare bone minimum for a vertex shader


//declaring color as a varying means it's value can be set here 
//and then referenced in the fragment shader
varying vec4 color;


void main(void)
{
	//we set the value of color for the vertex
	//this will effectively treat the world coordinate 
	//for this vertex as a color in the fragment shader
	color = gl_Vertex;
	
	//setting gl_Position is the only requirement of the vertex shader
	//multiplying by gl_ModelViewProjectionMatrix moves the world or local coordinate
	//of this vertex into screen space
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

