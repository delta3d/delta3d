/*! \file CUI_Shader.h
\brief Class definition for CUI_Shader

Original Author:
Scott Franke
sfranke@gljournal.org

Creation date:
29 Feb 2000

Purpose:
Class definition for CUI_Shader
*/

#ifndef CUI_SHADER_H
#define CUI_SHADER_H

#include "UI/UI_types.h"
#include "UI/CUI_SimpleShader.h"

namespace dtCore
{
   //! A general purpose surface shader
   /*!	Currently a single texture and a color. \n\n
   Future implementations should include procedural effects,
   multiple textures, other blend modes, etc\n\n
   Not a part of the core UI classes.  An add-on for sample purposes
   which can, of course, be used if you don't want to interface to
   an existing engine.
   */
   class CUI_OpenGLShader : public CUI_SimpleShader
   {
   public:
      //! Default constructor
      /*! Default shader is white with default texture */
      CUI_OpenGLShader( );

      //! Use a preloaded texture
      /*! \param texid texture identifier */
      CUI_OpenGLShader( int texid );

      //! Use a preloaded texture and a specific color
      /*! \param texid texture identifier
      \param color RGBA color
      */
      CUI_OpenGLShader( int texid, v4_f color );

      //! Create a solid color shader
      /*! \param color RGBA color */
      CUI_OpenGLShader( v4_f color );

      //! Destructor
      virtual ~CUI_OpenGLShader();

      //! Update the shaders motion (if any)
      //! called on SetShader, time is constant per frame
      //! setting texture and color has to be done by the shader via the supplied renderer
      virtual void Render ( double time, IUI_Renderer *renderer );

   protected:

   };
}
#endif

