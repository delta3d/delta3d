#ifndef DTHUD_CEGUIRENDERER_H
#define DTHUD_CEGUIRENDERER_H

#include <list>
#include <set>

#include <dtGUI/export.h>
#include <osg/GraphicsContext>

#include <CEGUI/CEGUIRenderer.h>


/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace CEGUI
{
   class Base;
   class Texture;
   class ImageCodec;
}
/// @endcond

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4251)
#endif

#define OGLRENDERER_VBUFF_CAPACITY   4096

namespace dtGUI
{

   class CEGUITexture;
   class DynamicModule;

   class DT_GUI_EXPORT CEGUIRenderer : public CEGUI::Renderer
   {

      public:

         CEGUIRenderer(CEGUI::uint max_quads, CEGUI::ImageCodec* codec = 0);

         CEGUIRenderer(CEGUI::uint max_quads, int width, int height, CEGUI::ImageCodec* codec = 0);

         virtual ~CEGUIRenderer(void);

         virtual   void addQuad(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex, const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colours, CEGUI::QuadSplitMode quam_split_mode);

         virtual   void doRender(void);

         virtual   void clearRenderList(void);

         virtual void setQueueingEnabled(bool setting) {m_queueing = setting;}

         virtual   CEGUI::Texture* createTexture(void);

         virtual   CEGUI::Texture* createTexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup);

         virtual   CEGUI::Texture* createTexture(float size);

         virtual   void destroyTexture(CEGUI::Texture* texture);

         virtual void destroyAllTextures(void);

         virtual bool isQueueingEnabled(void) const{return m_queueing;}

         virtual float getWidth(void) const      {return m_display_area.getWidth();}

         virtual float getHeight(void) const      {return m_display_area.getHeight();}

         virtual CEGUI::Size   getSize(void) const         {return m_display_area.getSize();}

         virtual CEGUI::Rect   getRect(void) const         {return m_display_area;}

         virtual   CEGUI::uint   getMaxTextureSize(void) const      {return m_maxTextureSize;}

         virtual   CEGUI::uint   getHorzScreenDPI(void) const   {return 96;}

         virtual   CEGUI::uint   getVertScreenDPI(void) const   {return 96;}

         void setDisplaySize(const CEGUI::Size& sz);

         CEGUI::ImageCodec& getImageCodec(void);

         void setImageCodec(const CEGUI::String& codecName);

         void setImageCodec(CEGUI::ImageCodec* codec);

         static void setDefaultImageCodecName(const CEGUI::String& codecName);

         static const CEGUI::String& getDefaultImageCodecName();

         void SetGraphicsContext(osg::GraphicsContext *);

      private:

         /************************************************************************
          Implementation Constants
          *************************************************************************/
         static const int         VERTEX_PER_QUAD;                     //!< number of vertices per quad
         static const int         VERTEX_PER_TRIANGLE;                  //!< number of vertices for a triangle
         static const int         VERTEXBUFFER_CAPACITY;                  //!< capacity of the allocated vertex buffer

         /*************************************************************************
          Implementation Structs & classes
          **************************************************************************/
         struct MyQuad
         {
            float tex[2];
            CEGUI::uint32 color;
            float vertex[3];
         };

         struct QuadInfo
         {
            GLuint      texid;
            CEGUI::Rect position;
            float      z;
            CEGUI::Rect      texPosition;
            CEGUI::uint32      topLeftCol;
            CEGUI::uint32      topRightCol;
            CEGUI::uint32      bottomLeftCol;
            CEGUI::uint32      bottomRightCol;

            CEGUI::QuadSplitMode   splitMode;

            bool operator<(const QuadInfo& other) const
            {
               return z > other.z;
            }

         };


         /*************************************************************************
          Implementation Methods
          **************************************************************************/
         // setup states etc
         void   initPerFrameStates(void);

         // restore states
         void  exitPerFrameStates(void);

         // render whatever is in the vertex buffer
         void   renderVBuffer(void);

         // sort quads list according to texture
         void   sortQuads(void);

         // render a quad directly to the display
         void   renderQuadDirect(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex, const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colours, CEGUI::QuadSplitMode quam_split_mode);

         // convert colour value to whatever the OpenGL system is expecting.
         CEGUI::uint32   colourToOGL(const CEGUI::colour& col) const;

         // set the module ID string
         void setModuleIdentifierString();

         // setup image codec
         void setupImageCodec(const CEGUI::String& codecName);

         // cleanup image codec
         void cleanupImageCodec();

         /*************************************************************************
           Implementation Data
          **************************************************************************/
         typedef std::multiset<QuadInfo>      QuadList;
         QuadList m_quadlist;

         CEGUI::Rect      m_display_area;

         MyQuad      myBuff[OGLRENDERER_VBUFF_CAPACITY];

         bool      m_queueing;         //!< setting for queuing control.
         CEGUI::uint m_currTexture;      //!< Currently bound texture.
         int         m_bufferPos;      //!< index into buffer where next vertex should be put.
         bool      m_sorted;         //!< true when data in quad list is sorted.

         std::list<CEGUITexture*>   m_texturelist;      //!< List used to track textures.
         GLint       m_maxTextureSize;      //!< Holds maximum supported texture size (in pixels).

         CEGUI::ImageCodec* m_imageCodec;           //!< Holds a pointer to the image codec to use.
         CEGUI::DynamicModule* m_imageCodecModule; //!< Holds a pointer to the image codec module. If m_imageCodecModule is 0 we are not owner of the image codec object

         static CEGUI::String m_defaultImageCodecName; //!< Holds the name of the default codec to use

         osg::ref_ptr<osg::GraphicsContext> m_pGraphicsContext;

   };

} //namespace dtGUI

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif //DTHUD_CEGUIRENDERER_H
