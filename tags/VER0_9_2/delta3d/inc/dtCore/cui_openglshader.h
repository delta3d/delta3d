/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef DELTA_CUI_OPENGLSHADER
#define DELTA_CUI_OPENGLSHADER


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

#endif // DELTA_CUI_OPENGLSHADER
