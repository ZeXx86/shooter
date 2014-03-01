void main()
{
	// output the transformed vertex
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
 
