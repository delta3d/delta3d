/*! \file CUI_OpenGLRenderer.h
\brief A simple interface to opengl

Original Author:
Scott Franke
sfranke@gljournal.org

Creation date:
12 Jun 2000

Purpose:
A simple interface to opengl
*/

#ifndef CUI_OPENGLRENDERER_H
#define CUI_OPENGLRENDERER_H

#include <stack>

#include "UI/UI_types.h"
#include "UI/UI_Lib.h"
#include "UI/UI_types.h"
#include "UI/IUI_Renderer.h"

#include "CUI_OpenGLShader.h"

namespace dtCore
{
   //! Implementation of IUI_Renderer
   /*! A generic renderer using OpenGL\n\n
   Not a part of the core UI classes.  An add-on for sample purposes
   which can, of course, be used if you don't want to interface to
   an existing engine.
   TODO: display lists?
   */
   class CUI_OpenGLRenderer : public IUI_Renderer
   {
   public:
      //! constructor
      CUI_OpenGLRenderer();

      //! destructor
      /*! delete textures (and display lists)
      */
      virtual ~CUI_OpenGLRenderer();

      //! begin rendering
      /*! Called once per frame.
      Disable lighting\n\n
      */
      virtual bool BeginRendering( double time );

      //! load a texture from a file into memory
      /*! \param file the filename
      \return an integer handle
      */
      virtual int LoadTexture( char *file );

      //! set the shader to use when rendering
      /*! bind texture (set up for multitex)
      */
      virtual bool SetShader( IUI_Shader *shader );

      //! rescale render volume to a specific area
      virtual bool RenderToArea( float minx, float miny, float maxx, float maxy, bool restrict = false );

      //! return to previous render area
      virtual bool RenderToPrevArea( bool restrict = false );

      //! render a [0,1] unit quad
      /*! This is probably the most frequent thing to render,
      so it can be sped up as necessary.\n\n
      TODO: Display list worth it?
      */
      virtual bool RenderUnitQuad( void );

      //! render a primitive
      /*! \param type the type of primitive
      \param vlist list of vertices
      \param texcoords list of texture coordinates
      \param numvert number of vertices in the list
      \see UI_POLYGON
      \see UI_TRIANGLES
      \see UI_QUADS
      All done in immediate mode
      */
      virtual bool RenderPrimitive( unsigned short type, v2_f *vlist, v2_f *texcoords, int numvert );

      //! Fade all rendering by this amount
      virtual bool SetFade( float fade );

      //! Get the current fade value
      virtual float GetFade( );

      // New functionality

      //! check to see if a given extension is supported
      virtual bool IsExtensionSupported( const char *extstring );

   protected:

      // procedural texture methods - don't use any of them in this sample

      //! bool SetTexture
      virtual bool SetTexture(int textureID) {return false;}

      //! bool SetColor
      virtual bool SetColor(v4_f color) {return false;}

      //! CreateTexture will be called from Shaders
      //! returns a texture object that holds information about dimensions, filtering filename and id
      virtual IUI_Texture* CreateTexture(int width, int height, int filtering, bool renderable) {return NULL;}

      //! UpdateTexture will be called from Shaders
      //! copies data to texture memory, returns TRUE on success
      virtual bool UpdateTexture(int texid, int xoffset, int yoffset, int width, int height, void *data) {return false;}

      // ! Load a texture and return texture object
      virtual IUI_Texture* LoadTextureNew(char* file) {return NULL;}

      // ! Clear current buffers
      virtual void ClearBuffer(bool clearDepth, bool clearColor, float depth) {return;}

      //! end rendering
      /*!	Called once per frame.
      */
      virtual void EndRendering( ) {return;}

      //! allocated texture ids
      unsigned int *m_textures;

      //! current maximum allowable number of textures
      unsigned int m_maxTexture;
      //! number of textures in use
      unsigned int m_numTextures;

      //! transformation stack depth
      unsigned int m_pushCount;
      //! scissor stack depth
      unsigned int m_scissorCount;

      //! scissor stack type
      typedef std::stack<int *> SCISSORAREASTACK;

      //! scissor stack
      SCISSORAREASTACK m_scissorAreaStack;

      //! current shader
      CUI_OpenGLShader* m_currentShader;

      //! primitive enumeration
      static const unsigned int TranslatePrimitive[UI_NUM_PRIMITIVES];

      //! current fade amount
      float m_fade;

   };
}
#endif
