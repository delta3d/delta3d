#ifndef DTHUD_CEGUITEXTURE_H
#define DTHUD_CEGUITEXTURE_H

#include <dtGUI/export.h>
#include <osg/Texture2D>
#include <CEGUI/CEGUITexture.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
    class GraphicsContext;
}
/// @endcond

namespace dtGUI
{

/**
 * @brief
 *     implementation of the CEGUI::Texture for OpenSceneGraph
 */
class DT_GUI_EXPORT CEGUITexture : public CEGUI::Texture
{
public:
   /********************************************************************************
                              construction/destruction
   ********************************************************************************/

   CEGUITexture(CEGUI::Renderer*);

   ~CEGUITexture();



   /********************************************************************************
                              DELTA-Interface
   ********************************************************************************/

   ///set the osg-texture that'll be used
   inline void SetOSGTexture(osg::Texture2D* pTexture) { if (pTexture) { m_pTexture = pTexture; } }

   ///get current osg-texture
   inline osg::Texture2D* GetOSGTexture() { return m_pTexture.get(); }

   ///get current osg-texture
   inline const osg::Texture2D* GetOSGTexture() const { return m_pTexture.get(); }

   ///get osg-texture-handle for a osg::GraphicsContext
   unsigned int GetTextureID(osg::GraphicsContext*);

   ///if set to true the CEGUIREnderer will flip y-coordinates
   inline void SetFlipHorizontal(bool bX) { m_bFlipHorizontal = bX; }

   ///if true the CEGUIREnderer will flip y-coordinates
   inline bool IsFlippedHorizontal() const { return m_bFlipHorizontal; }



   /********************************************************************************
                              CEGUI-Inteface
   ********************************************************************************/

   ///get the original with of the texture (cegui-textures are automatically resized to pot)
   virtual CEGUI::ushort getOriginalWidth(void) const;

   ///get the original height of the texture (cegui-textures are automatically resized to pot)
   virtual CEGUI::ushort getOriginalHeight(void) const;

   virtual float getXScale(void) const { return m_fScaleX; }

   virtual float getYScale(void) const { return m_fScaleY; }

   virtual   CEGUI::ushort getWidth() const;

   virtual   CEGUI::ushort getHeight() const;

   virtual void loadFromFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup);

   virtual void loadFromMemory(const void* buffPtr, CEGUI::uint buffWidth, CEGUI::uint buffHeight, CEGUI::Texture::PixelFormat pixelFormat);

   void ResizeToMinPOT(unsigned short w, unsigned short h);

private:

   void updateCachedScaleValues();

   osg::ref_ptr<osg::Texture2D> m_pTexture;

   float                        m_fScaleX;
   float                        m_fScaleY;

   unsigned short               m_usOriginalWidth;
   unsigned short               m_usOriginalHeight;
   bool                         m_bFlipHorizontal;
};

} // namespace dtGUI

#endif // DTHUD_CEGUITEXTURE_H
