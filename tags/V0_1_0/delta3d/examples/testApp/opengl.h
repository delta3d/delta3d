/*! \file opengl.h
	\brief header to include crossplatform opengl support

Original Author:
	Scott Franke
	sfranke@gljournal.org

Creation date:
	12 Jun 2000

*/

#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
#endif

#ifdef MACOS
 #include <OpenGL/Opengl.h>
#endif
