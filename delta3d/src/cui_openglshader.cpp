/*! \file CUI_OpenGLShader.cpp
	\brief implementation of CUI_Shader

Original Author:
	Scott Franke
	 sfranke@gljournal.org

Creation date:
	29 Feb 2000

Purpose:
	Implementation
*/

#include <memory.h>

// dll
#include "UI/UI_types.h"
#include "UI/IUI_Renderer.h"

//local
#include "cui_openglshader.h"

#include <stdio.h>


using namespace dtCore;

/////////////////////////////////////////////////////////////
// Function: CUI_OpenGLShader::CUI_OpenGLShader
//           ( void )
//
// Description:
//   Default constructor.  Default shader is white with
//    default texture.
//
/////////////////////////////////////////////////////////////

CUI_OpenGLShader::CUI_OpenGLShader( void )
{

}

/////////////////////////////////////////////////////////////
// Function: CUI_OpenGLShader::CUI_OpenGLShader
//           ( int texid )
//
// Description:
//   Use a pre-loaded texture.
//
/////////////////////////////////////////////////////////////

CUI_OpenGLShader::CUI_OpenGLShader( int texid ) : CUI_SimpleShader( texid )
{

}

/////////////////////////////////////////////////////////////
// Function: CUI_OpenGLShader::CUI_OpenGLShader
//           ( int texid,
//             v4_f color )
//
// Description:
//   Use a pre-loaded texture and a given color.
//
/////////////////////////////////////////////////////////////

CUI_OpenGLShader::CUI_OpenGLShader( int texid, v4_f color ) : CUI_SimpleShader( texid, color )
{

}

/////////////////////////////////////////////////////////////
// Function: CUI_OpenGLShader::CUI_OpenGLShader
//           ( v4_f color )
//
// Description:
//   Use a non-textured (different than default texture)
//    shader of a certain color.
//
/////////////////////////////////////////////////////////////

CUI_OpenGLShader::CUI_OpenGLShader( v4_f color ) : CUI_SimpleShader( color )
{

}

/////////////////////////////////////////////////////////////
// Function: CUI_OpenGLShader::~CUI_OpenGLShader
//           ( void )
//
// Description:
//   Destructor.
//
/////////////////////////////////////////////////////////////

CUI_OpenGLShader::~CUI_OpenGLShader( void )
{

}

/////////////////////////////////////////////////////////////
// Function: CUI_OpenGLShader::Render
//           ( double time, IUI_Renderer *renderer )
//
// Description:
//   Tell renderer to bind texture
//
/////////////////////////////////////////////////////////////
void CUI_OpenGLShader::Render ( double time, IUI_Renderer *renderer )
{
	renderer->SetShader( this );
}
