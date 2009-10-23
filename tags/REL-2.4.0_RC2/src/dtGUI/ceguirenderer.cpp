#include <dtGUI/ceguirenderer.h>
#include <dtGUI/ceguitexture.h>
#include <dtUtil/macros.h>

//includes gl without needed the windows headers.
#include <osg/GL>
#include <osg/GLU>

#if defined( __APPLE__ )
//needed for something in CEGUI. weird.
#include <CoreFoundation/CoreFoundation.h>
#endif /* __APPLE__ **/

#include <iostream>

#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUIEventArgs.h>
#include <CEGUI/CEGUIImageCodec.h>
#include <CEGUI/CEGUIDynamicModule.h>
#include <CEGUI/CEGUIBase.h>
#include <CEGUI/CEGUITexture.h>
#include <CEGUI/CEGUILogger.h>

#define S_(X) #X
#define STRINGIZE(X) S_(X)

using namespace dtGUI;

/*************************************************************************
   Constants definitions
 **************************************************************************/
const int CEGUIRenderer::VERTEX_PER_QUAD           = 6;
const int CEGUIRenderer::VERTEX_PER_TRIANGLE       = 3;
const int CEGUIRenderer::VERTEXBUFFER_CAPACITY     = OGLRENDERER_VBUFF_CAPACITY;


/*************************************************************************
   Constructor
 **************************************************************************/
CEGUIRenderer::CEGUIRenderer(CEGUI::uint max_quads, CEGUI::ImageCodec*  codec)
   : m_queueing(true)
   , m_currTexture(0)
   , m_bufferPos(0)
   , m_imageCodec(codec)
   , m_imageCodecModule(0)
   {
      GLint vp[4];

      // initialise renderer size
      glGetIntegerv(GL_VIEWPORT, vp);
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
      m_display_area.d_left   = 0;
      m_display_area.d_top    = 0;
      m_display_area.d_right  = (float)vp[2];
      m_display_area.d_bottom = (float)vp[3];

      if (!m_imageCodec) { setupImageCodec(""); }

      setModuleIdentifierString();
   }


CEGUIRenderer::CEGUIRenderer(CEGUI::uint max_quads,int width, int height, CEGUI::ImageCodec* codec)
   : m_queueing(true)
   , m_currTexture(0)
   , m_bufferPos(0)
   , m_imageCodec(codec)
   , m_imageCodecModule(0)
   {
      GLint vp[4];

      // initialise renderer size
      glGetIntegerv(GL_VIEWPORT, vp);
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
      m_display_area.d_left   = 0;
      m_display_area.d_top    = 0;
      m_display_area.d_right  = static_cast<float>(width);
      m_display_area.d_bottom = static_cast<float>(height);
      if (!m_imageCodec)
      {
         setupImageCodec("");
      }
      setModuleIdentifierString();
   }


/*************************************************************************
   Destructor
 **************************************************************************/
CEGUIRenderer::~CEGUIRenderer(void)
{
   destroyAllTextures();
   cleanupImageCodec();
}


/*************************************************************************
   adds a quad to the list to be rendered
 **************************************************************************/
void CEGUIRenderer::addQuad(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex, const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colours,  CEGUI::QuadSplitMode quam_split_mode)
{
   if (m_pGraphicsContext == NULL)
   {
      CEGUI::Logger::getSingleton().logEvent("dtGUI::CEGUIRenderer doesn't have a valid graphics context.", CEGUI::Errors);
      return;
   }

   // if not queuing, render directly (as in, right now!)
   if (!m_queueing)
   {
      renderQuadDirect(dest_rect, z, tex, texture_rect, colours, quam_split_mode);
   }
   else
   {
      QuadInfo quad;
      quad.position          = dest_rect;
      quad.position.d_bottom = m_display_area.d_bottom - dest_rect.d_bottom;
      quad.position.d_top    = m_display_area.d_bottom - dest_rect.d_top;
      quad.z                 = z;
      quad.texid             = ((CEGUITexture*)tex)->GetTextureID(m_pGraphicsContext.get());

      if (((CEGUITexture*)tex)->IsFlippedHorizontal())
      {
         quad.texPosition.d_left   = texture_rect.d_left;
         quad.texPosition.d_right  = texture_rect.d_right;
         quad.texPosition.d_top    = texture_rect.d_bottom;
         quad.texPosition.d_bottom = texture_rect.d_top;
      }
      else
      {
         quad.texPosition = texture_rect;
      }

      quad.topLeftCol     = colourToOGL(colours.d_top_left);
      quad.topRightCol    = colourToOGL(colours.d_top_right);
      quad.bottomLeftCol  = colourToOGL(colours.d_bottom_left);
      quad.bottomRightCol = colourToOGL(colours.d_bottom_right);

      // set quad split mode
      quad.splitMode = quam_split_mode;

      m_quadlist.insert(quad);
   }

}



/*************************************************************************
   perform final rendering for all queued renderable quads.
 **************************************************************************/
void CEGUIRenderer::doRender(void)
{
   m_currTexture = 0;

   initPerFrameStates();
   //glInterleavedArrays(GL_T2F_C4UB_V3F , 0, myBuff);

   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   glEnableClientState(GL_VERTEX_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(MyQuad), &myBuff[0].tex);
   glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(MyQuad), &myBuff[0].color);
   glVertexPointer(3, GL_FLOAT, sizeof(MyQuad), &myBuff[0].vertex);


   // iterate over each quad in the list
   for (QuadList::iterator i = m_quadlist.begin(); i != m_quadlist.end(); ++i)
   {
      const QuadInfo& quad = (*i);

      if (m_currTexture != quad.texid)
      {
         renderVBuffer();
         glBindTexture(GL_TEXTURE_2D, quad.texid);
         m_currTexture = quad.texid;
      }

      // vert0
      myBuff[m_bufferPos].vertex[0] = quad.position.d_left;
      myBuff[m_bufferPos].vertex[1] = quad.position.d_top;
      myBuff[m_bufferPos].vertex[2] = quad.z;
      myBuff[m_bufferPos].color     = quad.topLeftCol;
      myBuff[m_bufferPos].tex[0]    = quad.texPosition.d_left;
      myBuff[m_bufferPos].tex[1]    = quad.texPosition.d_top;
      ++m_bufferPos;

      // vert1
      myBuff[m_bufferPos].vertex[0] = quad.position.d_left;
      myBuff[m_bufferPos].vertex[1] = quad.position.d_bottom;
      myBuff[m_bufferPos].vertex[2] = quad.z;
      myBuff[m_bufferPos].color     = quad.bottomLeftCol;
      myBuff[m_bufferPos].tex[0]    = quad.texPosition.d_left;
      myBuff[m_bufferPos].tex[1]    = quad.texPosition.d_bottom;
      ++m_bufferPos;

      // vert2

      // top-left to bottom-right diagonal
      if (quad.splitMode == CEGUI::TopLeftToBottomRight)
      {
         myBuff[m_bufferPos].vertex[0] = quad.position.d_right;
         myBuff[m_bufferPos].vertex[1] = quad.position.d_bottom;
         myBuff[m_bufferPos].vertex[2] = quad.z;
         myBuff[m_bufferPos].color     = quad.bottomRightCol;
         myBuff[m_bufferPos].tex[0]    = quad.texPosition.d_right;
         myBuff[m_bufferPos].tex[1]    = quad.texPosition.d_bottom;
      }
      // bottom-left to top-right diagonal
      else
      {
         myBuff[m_bufferPos].vertex[0] = quad.position.d_right;
         myBuff[m_bufferPos].vertex[1] = quad.position.d_top;
         myBuff[m_bufferPos].vertex[2] = quad.z;
         myBuff[m_bufferPos].color     = quad.topRightCol;
         myBuff[m_bufferPos].tex[0]    = quad.texPosition.d_right;
         myBuff[m_bufferPos].tex[1]    = quad.texPosition.d_top;
      }
      ++m_bufferPos;

      // vert3
      myBuff[m_bufferPos].vertex[0] = quad.position.d_right;
      myBuff[m_bufferPos].vertex[1] = quad.position.d_top;
      myBuff[m_bufferPos].vertex[2] = quad.z;
      myBuff[m_bufferPos].color     = quad.topRightCol;
      myBuff[m_bufferPos].tex[0]    = quad.texPosition.d_right;
      myBuff[m_bufferPos].tex[1]    = quad.texPosition.d_top;
      ++m_bufferPos;

      // vert4

      // top-left to bottom-right diagonal
      if (quad.splitMode == CEGUI::TopLeftToBottomRight)
      {
         myBuff[m_bufferPos].vertex[0] = quad.position.d_left;
         myBuff[m_bufferPos].vertex[1] = quad.position.d_top;
         myBuff[m_bufferPos].vertex[2] = quad.z;
         myBuff[m_bufferPos].color     = quad.topLeftCol;
         myBuff[m_bufferPos].tex[0]    = quad.texPosition.d_left;
         myBuff[m_bufferPos].tex[1]    = quad.texPosition.d_top;
      }
      // bottom-left to top-right diagonal
      else
      {
         myBuff[m_bufferPos].vertex[0] = quad.position.d_left;
         myBuff[m_bufferPos].vertex[1] = quad.position.d_bottom;
         myBuff[m_bufferPos].vertex[2] = quad.z;
         myBuff[m_bufferPos].color     = quad.bottomLeftCol;
         myBuff[m_bufferPos].tex[0]    = quad.texPosition.d_left;
         myBuff[m_bufferPos].tex[1]    = quad.texPosition.d_bottom;
      }
      ++m_bufferPos;

      // vert 5
      myBuff[m_bufferPos].vertex[0] = quad.position.d_right;
      myBuff[m_bufferPos].vertex[1] = quad.position.d_bottom;
      myBuff[m_bufferPos].vertex[2] = quad.z;
      myBuff[m_bufferPos].color     = quad.bottomRightCol;
      myBuff[m_bufferPos].tex[0]    = quad.texPosition.d_right;
      myBuff[m_bufferPos].tex[1]    = quad.texPosition.d_bottom;
      ++m_bufferPos;

      if (m_bufferPos > (VERTEXBUFFER_CAPACITY - VERTEX_PER_QUAD))
      {
         renderVBuffer();
      }

   }

   // Render
   renderVBuffer();

   exitPerFrameStates();
}


/*************************************************************************
   clear the queue
 **************************************************************************/
void CEGUIRenderer::clearRenderList(void)
{
   m_quadlist.clear();
}


/*************************************************************************
   create an empty texture
 **************************************************************************/
CEGUI::Texture* CEGUIRenderer::createTexture(void)
{
   CEGUITexture* tex = new CEGUITexture(this);
   m_texturelist.push_back(tex);
   return tex;
}

CEGUI::Texture* CEGUIRenderer::createTexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup)
{
   CEGUITexture* tex = new CEGUITexture(this);
   try
   {
      tex->loadFromFile(filename, resourceGroup);
   }
   catch (CEGUI::RendererException&)
   {
      delete tex;
      throw;
   }
   m_texturelist.push_back(tex);
   return tex;
}


CEGUI::Texture* CEGUIRenderer::createTexture(float size)
{
   CEGUITexture* tex = new CEGUITexture(this);
   try
   {
      tex->ResizeToMinPOT(size, size);
   }
   catch (CEGUI::RendererException&)
   {
      delete tex;
      throw;
   }
   m_texturelist.push_back(tex);
   return tex;
}


/*************************************************************************
   Destroy a texture
 **************************************************************************/
void CEGUIRenderer::destroyTexture(CEGUI::Texture* texture)
{
   if (texture)
   {
      CEGUITexture* tex = (CEGUITexture*)texture;
      m_texturelist.remove(tex);
      delete tex;
   }

}


/*************************************************************************
   destroy all textures still active
 **************************************************************************/
void CEGUIRenderer::destroyAllTextures(void)
{
   while (!m_texturelist.empty())
   {
      destroyTexture(*(m_texturelist.begin()));
   }
}


/*************************************************************************
   setup states etc
 **************************************************************************/
void CEGUIRenderer::initPerFrameStates(void)
{
   //save current attributes
   glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
   glPushAttrib(GL_ALL_ATTRIB_BITS);

   glPolygonMode(GL_FRONT, GL_FILL);
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0.0, m_display_area.d_right, 0.0, m_display_area.d_bottom);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   glDisable(GL_LIGHTING);
   glDisable(GL_DEPTH_TEST);
   glDisable(GL_FOG);
   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);
   glDisable(GL_TEXTURE_GEN_R);

   glFrontFace(GL_CCW);
   glCullFace(GL_BACK);
   glEnable(GL_CULL_FACE);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glEnable(GL_TEXTURE_2D);
}


void CEGUIRenderer::exitPerFrameStates(void)
{
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

   //restore former attributes
   glPopAttrib();
   glPopClientAttrib();
}


/*************************************************************************
   renders whatever is in the vertex buffer
 **************************************************************************/
void CEGUIRenderer::renderVBuffer(void)
{
   // if bufferPos is 0 there is no data in the buffer and nothing to render
   if (m_bufferPos == 0)
   {
      return;
   }

   // render the sprites
   glDrawArrays(GL_TRIANGLES, 0, m_bufferPos);

   // reset buffer position to 0...
   m_bufferPos = 0;
}


/*************************************************************************
   sort quads list according to texture
 **************************************************************************/
void CEGUIRenderer::sortQuads(void)
{
   // no need to do anything here.
}


/*************************************************************************
   render a quad directly to the display
 **************************************************************************/
void CEGUIRenderer::renderQuadDirect(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex, const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colours,  CEGUI::QuadSplitMode quam_split_mode)
{
   if (m_pGraphicsContext == NULL)
   {
      CEGUI::Logger::getSingleton().logEvent("dtGUI::CEGUIRenderer doesn't have a valid graphics context.", CEGUI::Errors);
      return;
   }

   QuadInfo quad;
   quad.position.d_left   = dest_rect.d_left;
   quad.position.d_right   = dest_rect.d_right;
   quad.position.d_bottom   = m_display_area.d_bottom - dest_rect.d_bottom;
   quad.position.d_top      = m_display_area.d_bottom - dest_rect.d_top;
   quad.texPosition      = texture_rect;

   quad.topLeftCol      = colourToOGL(colours.d_top_left);
   quad.topRightCol   = colourToOGL(colours.d_top_right);
   quad.bottomLeftCol   = colourToOGL(colours.d_bottom_left);
   quad.bottomRightCol   = colourToOGL(colours.d_bottom_right);

   MyQuad myquad[VERTEX_PER_QUAD];

   initPerFrameStates();
   glInterleavedArrays(GL_T2F_C4UB_V3F , 0, myquad);
   glBindTexture(GL_TEXTURE_2D, ((CEGUITexture*)tex)->GetTextureID(m_pGraphicsContext.get()));

   //vert0
   myquad[0].vertex[0] = quad.position.d_left;
   myquad[0].vertex[1] = quad.position.d_top;
   myquad[0].vertex[2] = z;
   myquad[0].color     = quad.topLeftCol;
   myquad[0].tex[0]    = quad.texPosition.d_left;
   myquad[0].tex[1]    = quad.texPosition.d_top;

   //vert1
   myquad[1].vertex[0] = quad.position.d_left;
   myquad[1].vertex[1] = quad.position.d_bottom;
   myquad[1].vertex[2] = z;
   myquad[1].color     = quad.bottomLeftCol;
   myquad[1].tex[0]    = quad.texPosition.d_left;
   myquad[1].tex[1]    = quad.texPosition.d_bottom;

   //vert2

   // top-left to bottom-right diagonal
   if (quam_split_mode == CEGUI::TopLeftToBottomRight)
   {
      myquad[2].vertex[0] = quad.position.d_right;
      myquad[2].vertex[1] = quad.position.d_bottom;
      myquad[2].vertex[2] = z;
      myquad[2].color     = quad.bottomRightCol;
      myquad[2].tex[0]    = quad.texPosition.d_right;
      myquad[2].tex[1]    = quad.texPosition.d_bottom;
   }
   // bottom-left to top-right diagonal
   else
   {
      myquad[2].vertex[0] = quad.position.d_right;
      myquad[2].vertex[1] = quad.position.d_top;
      myquad[2].vertex[2] = z;
      myquad[2].color     = quad.topRightCol;
      myquad[2].tex[0]    = quad.texPosition.d_right;
      myquad[2].tex[1]    = quad.texPosition.d_top;
   }

   //vert3
   myquad[3].vertex[0] = quad.position.d_right;
   myquad[3].vertex[1] = quad.position.d_top;
   myquad[3].vertex[2] = z;
   myquad[3].color     = quad.topRightCol;
   myquad[3].tex[0]    = quad.texPosition.d_right;
   myquad[3].tex[1]    = quad.texPosition.d_top;

   //vert4

   // top-left to bottom-right diagonal
   if (quam_split_mode == CEGUI::TopLeftToBottomRight)
   {
      myquad[4].vertex[0] = quad.position.d_left;
      myquad[4].vertex[1] = quad.position.d_top;
      myquad[4].vertex[2] = z;
      myquad[4].color     = quad.topLeftCol;
      myquad[4].tex[0]    = quad.texPosition.d_left;
      myquad[4].tex[1]    = quad.texPosition.d_top;
   }
   // bottom-left to top-right diagonal
   else
   {
      myquad[4].vertex[0] = quad.position.d_left;
      myquad[4].vertex[1] = quad.position.d_bottom;
      myquad[4].vertex[2] = z;
      myquad[4].color     = quad.bottomLeftCol;
      myquad[4].tex[0]    = quad.texPosition.d_left;
      myquad[4].tex[1]    = quad.texPosition.d_bottom;
   }

   //vert5
   myquad[5].vertex[0] = quad.position.d_right;
   myquad[5].vertex[1] = quad.position.d_bottom;
   myquad[5].vertex[2] = z;
   myquad[5].color     = quad.bottomRightCol;
   myquad[5].tex[0]    = quad.texPosition.d_right;
   myquad[5].tex[1]    = quad.texPosition.d_bottom;

   glDrawArrays(GL_TRIANGLES, 0, 6);

   exitPerFrameStates();
}


/*************************************************************************
   convert colour value to whatever the OpenGL system is expecting.
 **************************************************************************/
CEGUI::uint32 CEGUIRenderer::colourToOGL(const CEGUI::colour& col) const
{
   const CEGUI::argb_t c = col.getARGB();

   // OpenGL wants RGBA

#ifdef __BIG_ENDIAN__
   CEGUI::uint32 cval = (c << 8) | (c >> 24);
#else
   CEGUI::uint32 cval = ((c&0xFF0000)>>16) | (c&0xFF00) | ((c&0xFF)<<16) | (c&0xFF000000);
#endif
   return cval;
}


void CEGUIRenderer::setDisplaySize(const CEGUI::Size& sz)
{
   if (m_display_area.getSize() != sz)
   {
      m_display_area.setSize(sz);

      CEGUI::EventArgs args;
      fireEvent(EventDisplaySizeChanged, args, EventNamespace);
   }
}

void CEGUIRenderer::setModuleIdentifierString()
{
   // set ID string
   d_identifierString = "CEGUI::Renderer - Official OpenGL based renderer module for CEGUI";
}


/************************************************************************
    Grabs all loaded textures to local buffers and frees them
 *************************************************************************/
//void CEGUIRenderer::grabTextures()
//{
//    typedef std::list<Texture*> texlist;
//    texlist::iterator i = m_texturelist.begin();
//    while (i!=m_texturelist.end())
//    {
//        (*i)->grabTexture();
//        i++;
//    }
//}


/************************************************************************
    Restores all textures from the previous call to 'grabTextures'
 *************************************************************************/
//void CEGUIRenderer::restoreTextures()
//{
//    typedef std::list<Texture*> texlist;
//    texlist::iterator i = m_texturelist.begin();
//    while (i!=m_texturelist.end())
//    {
//        (*i)->restoreTexture();
//        i++;
//    }
//}
/***********************************************************************
    Get the current ImageCodec object used
 *************************************************************************/
CEGUI::ImageCodec& CEGUIRenderer::getImageCodec()
{
   return *m_imageCodec;
}
/***********************************************************************
    Set the current ImageCodec object used
 *************************************************************************/
void CEGUIRenderer::setImageCodec(const CEGUI::String& codecName)
{
   setupImageCodec(codecName);
}
/***********************************************************************
    Set the current ImageCodec object used
 *************************************************************************/
void CEGUIRenderer::setImageCodec(CEGUI::ImageCodec* codec)
{
   if (codec)
   {
      cleanupImageCodec();
      m_imageCodec = codec;
      m_imageCodecModule = 0;
   }
}
/***********************************************************************
    setup the ImageCodec object used
 *************************************************************************/
void CEGUIRenderer::setupImageCodec(const CEGUI::String& codecName)
{

   // Cleanup the old image codec
   if (m_imageCodec)
   {
      cleanupImageCodec();
   }
   // Test whether we should use the default codec or not
   if (codecName.empty())
   {
      m_imageCodecModule = new CEGUI::DynamicModule(CEGUI::String("CEGUI") + m_defaultImageCodecName);
   }
   else
   {
      m_imageCodecModule = new CEGUI::DynamicModule(CEGUI::String("CEGUI") + codecName);
   }

   // Create the codec object itself
   CEGUI::ImageCodec* (*createFunc)(void) =
      (CEGUI::ImageCodec* (*)(void))m_imageCodecModule->getSymbolAddress("createImageCodec");
   m_imageCodec = createFunc();
}
/***********************************************************************
    cleanup the ImageCodec object used
 *************************************************************************/
void CEGUIRenderer::cleanupImageCodec()
{
   if (m_imageCodec && m_imageCodecModule)
   {
      void(*deleteFunc)(CEGUI::ImageCodec*) =
         (void(*)(CEGUI::ImageCodec*))m_imageCodecModule->getSymbolAddress("destroyImageCodec");
      deleteFunc(m_imageCodec);
      m_imageCodec = 0;
      delete m_imageCodecModule;
      m_imageCodecModule = 0;
   }

}
/***********************************************************************
    set the default ImageCodec name
 *************************************************************************/
void CEGUIRenderer::setDefaultImageCodecName(const CEGUI::String& codecName)
{
   m_defaultImageCodecName = codecName;
}

/***********************************************************************
    get the default ImageCodec name to be used
 *************************************************************************/
const CEGUI::String& CEGUIRenderer::getDefaultImageCodecName()
{
   return m_defaultImageCodecName;

}

void CEGUIRenderer::SetGraphicsContext(osg::GraphicsContext *gc)
{
   m_pGraphicsContext = gc;
}

CEGUI::String CEGUIRenderer::m_defaultImageCodecName("TGAImageCodec");


