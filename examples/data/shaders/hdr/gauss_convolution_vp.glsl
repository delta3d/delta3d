
/*
 * Reimplement fixed function pipeline of OpenGL
 * So bypass all the data.
 * Precompute some constants which are needed for the convolution computation
 */

//! Sigma value for the gaussian kernel
uniform float sigma;

const float PI = 3.1415926535897;

// Varyings
varying float sigma2;
varying float c;

/**
 * Reimplement fixed pipeline
 **/
void main(void)
{
	// bypass the texture coordinate data
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	// compute position of the pixel 
	gl_Position = ftransform();
	
	// bypass color data
	gl_FrontColor = gl_Color;
	
	// precompute constants
	sigma2 = 2.0 * sigma * sigma;
	c = sqrt((1.0 / (sigma2 * PI)));
}

