/*! \file CUI_OpenGLRenderer.cpp
	\brief Implementation of CUI_OpenGLRenderer

Original Author:
	Scott Franke
	sfranke@gljournal.org

Creation date:
	12 Jun 2000
	Jun 2002 - modified for glut sample

Purpose:
	Implementation of CUI_OpenGLRenderer.
	a simple interface to OpenGL
*/

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#include <windows.h>
#include <GL/gl.h>
#endif

// local
#include "UI/CUI_SimpleShader.h"
#include "dtCore/cui_openglrenderer.h"
//#include "glpng.h"

#include <osgDB/ReadFile>
#include "dtCore/notify.h"

using namespace dtCore;

const unsigned int CUI_OpenGLRenderer::TranslatePrimitive[] = {
		GL_POLYGON, GL_QUADS, GL_TRIANGLES, GL_LINE_LOOP
};

// CUI_OpenGLRenderer()
// constructor
// initialize list of texture objects
CUI_OpenGLRenderer::CUI_OpenGLRenderer()
{

	m_maxTexture = 50;
	m_textures = new unsigned int[m_maxTexture];
	glGenTextures( m_maxTexture, (GLuint*)m_textures );
	m_numTextures = 0;

	m_pushCount = 0;
	m_scissorCount = 0;

	// make sure the glpng lib goes according to spec :)
	//pngSetStandardOrientation( true );
	
	m_currentShader = NULL;
	m_fade = 1.0;

}

// ~CUI_OpenGLRenderer
// destructor
// delete list of texture objects
CUI_OpenGLRenderer::~CUI_OpenGLRenderer()
{
	glDeleteTextures( m_numTextures, (GLuint*)m_textures );
	delete []m_textures;
}

// BeginRendering
// called once per frame
// disable lighting
bool CUI_OpenGLRenderer::BeginRendering( double time )
{
	v4_i viewport;
	glGetIntegerv(GL_VIEWPORT, (GLint*)viewport );

	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );

	return false;
}

// LoadTexture
// make a call to the glPNG library
int CUI_OpenGLRenderer::LoadTexture( char *filename )
{
        //pngInfo info;

	// need to expand texture list
	if( m_numTextures >= m_maxTexture )
	{
		unsigned int *temp;
		temp = m_textures;
		m_textures = new unsigned int[m_maxTexture*2];
		memcpy( m_textures, temp, sizeof(unsigned int) * m_maxTexture );
		glGenTextures( m_maxTexture, &(((GLuint*)m_textures)[m_maxTexture]) );
		delete []temp;
		m_maxTexture *= 2;
	}

	int texid = m_textures[m_numTextures];

	glBindTexture(GL_TEXTURE_2D, texid);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F); //0x812F value of GL_CLAMP_TO_EDGE in glext.h
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);

/*
	if (!pngLoad(filename, PNG_NOMIPMAP, PNG_ALPHA, &info)) {
		// error opening file.- goto default shader
		char *buf = new char[50+strlen(filename)];
		buf[0] = '\0';
		strcat( buf, "Unable to load texture : " );
		strcat( buf, filename );
		printf( buf );
//		MessageBox( NULL, buf, "Failure", MB_OK );
		delete []buf;
		return -1;
	}
*/

   osg::Image* img = osgDB::readImageFile(filename);
   if(!img)
   {
      dtCore::Notify(dtCore::WARN, "Can't load texture file '%s'", filename);
      return -1;
   }
   img->ensureValidSizeForTexturing(1024);

   glTexImage2D(GL_TEXTURE_2D, 0, 
               img->getInternalTextureFormat(),
               img->s(), img->t(), 0,
                (GLenum)img->getPixelFormat(),
                (GLenum)img->getDataType(), img->data());

	m_numTextures++;

	return texid;

}

// SetShader
// set the pointer so render calls will work correctly
bool CUI_OpenGLRenderer::SetShader( IUI_Shader *shader )
{
	v4_f color;
	int texid;

	m_currentShader = (CUI_OpenGLShader *) shader;
	// more complex shaders will require this functionality in the render* functions
	if( m_currentShader != NULL )
	{

		m_currentShader->GetColorv( color );

		texid = m_currentShader->GetTexid();
		if( texid < 0 )
		{
			glDisable( GL_TEXTURE_2D );
		}
		else
		{
			glBindTexture( GL_TEXTURE_2D, texid );
			glEnable( GL_TEXTURE_2D );
		}

		glColor4f( color[0], color[1], color[2], m_fade * color[3] );
	}
	else
	{
		// no shader - transparent
		glColor4f( 0.0, 0.0, 0.0, 0.0 );
		glDisable( GL_TEXTURE_2D );
	}

	return false;
}

// RenderToArea
// rescale render volume to a specific area.
bool CUI_OpenGLRenderer::RenderToArea( float minx, float miny, float maxx, float maxy, bool restrict )
{
	int* newarea = new v4_i;

	v4_i oldarea;

	if( m_pushCount == 0 )
	{
		glGetIntegerv(GL_VIEWPORT, (GLint*)oldarea );
	}
	else
	{
		memcpy( oldarea, m_scissorAreaStack.top(), sizeof(v4_i) );
	}

	FloatToInt( &(newarea[0]), oldarea[0] + oldarea[2] * minx );
	FloatToInt( &(newarea[1]), oldarea[1] + oldarea[3] * miny );
	FloatToInt( &(newarea[2]), oldarea[2] * (maxx - minx) );
	FloatToInt( &(newarea[3]), oldarea[3] * (maxy - miny) );

	m_scissorAreaStack.push( newarea );

	// if restrict rendering to this area, use stencil
	if( restrict == true )
	{
		glScissor( newarea[0], newarea[1], newarea[2], newarea[3] );
		if( m_scissorCount == 0 )
		{
		  glEnable( GL_SCISSOR_TEST );
		}
		m_scissorCount++;
	}

	glPushMatrix();

	glTranslatef( minx, miny, 0 );
	glScalef( (maxx-minx), (maxy-miny), 1 );

	m_pushCount++;

	return false;
}

bool CUI_OpenGLRenderer::RenderToPrevArea( bool restrict )
{
	if( m_pushCount>0 )
	{
		int *stackpop;

		glPopMatrix();

		stackpop = m_scissorAreaStack.top();
		m_scissorAreaStack.pop();
		delete []stackpop;

		if( restrict )
		{
			if( m_scissorCount > 1 )
			{
				v4_i oldarea;
				memcpy( oldarea, m_scissorAreaStack.top(), sizeof(v4_i) );
				glScissor( oldarea[0], oldarea[1], oldarea[2], oldarea[3] );
			}
			else
			{
			  glDisable( GL_SCISSOR_TEST );
			}
			m_scissorCount--;
		}

		m_pushCount--;
		return false;
	}

	// too many pops!
	return true;
}

// RenderUnitQuad
// no paramters, fast & easy
bool CUI_OpenGLRenderer::RenderUnitQuad( )
{
// bind shader texture

	glBegin( GL_QUADS );
	glTexCoord2f( 0.0, 0.0 );
	glVertex2f( 0.0, 0.0 );
	glTexCoord2f( 1.0, 0.0 );
	glVertex2f( 1.0, 0.0 );
	glTexCoord2f( 1.0, 1.0 );
	glVertex2f( 1.0, 1.0 );
	glTexCoord2f( 0.0, 1.0 );
	glVertex2f( 0.0, 1.0 );
	glEnd();

	return false;
}

// RenderPolygon
// simple immediate mode
bool CUI_OpenGLRenderer::RenderPrimitive( unsigned short type, v2_f *vlist, v2_f *texcoords, int numvert )
{

	glBegin( TranslatePrimitive[type] );
		for( int i=0;i<numvert;i++ )
		{
			glTexCoord2fv( texcoords[i] );
			glVertex2fv( vlist[i] );
		}
	glEnd();

	return false;
}

// SetFade
//  set the current fade value
bool CUI_OpenGLRenderer::SetFade( float fade )
{
	v4_f color;
	
	// set local fade value
	m_fade = fade;
	
	// update current color
	if( m_currentShader != NULL )
	{
		m_currentShader->GetColorv( color );
		glColor4f( color[0], color[1], color[2], fade * color[3] );
	}
	return false;
}

// GetFade
//  return the current fade value
float CUI_OpenGLRenderer::GetFade()
{
	return m_fade;
}

// IsExtensionSupported
// check for an extension
bool CUI_OpenGLRenderer::IsExtensionSupported( const char *extstring )
{
// implement this at some point
	return false;

}
