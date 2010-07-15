
// Pass-through geometry shader for testing purposes
void main()
{
		int i;
		for(i=0; i< gl_VerticesIn; i++)
		{
			gl_Position = gl_PositionIn[i];
			EmitVertex();
		}
		EndPrimitive();
}