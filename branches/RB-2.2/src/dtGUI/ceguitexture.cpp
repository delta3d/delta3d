#include <dtGUI/ceguitexture.h>
#include <dtGUI/ceguirenderer.h>

#include <osg/GraphicsContext>
#include <osg/Image>
#include <osg/Texture2D>

#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUIImageCodec.h>

#include <iostream>

using namespace dtGUI;

CEGUITexture::CEGUITexture(CEGUI::Renderer *pRenderer)
   : CEGUI::Texture(pRenderer)
   , m_bFlipHorizontal(false)
{
    m_pTexture = new osg::Texture2D();
    m_pTexture->setInternalFormat(GL_RGBA);
    m_pTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
    m_pTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
}

CEGUITexture::~CEGUITexture()
{
}

void CEGUITexture::loadFromFile(const CEGUI::String& sFileName, const CEGUI::String& sResourceGroupName)
{
   CEGUIRenderer* pRenderer =  static_cast<CEGUIRenderer*>(getRenderer());
   // load file to memory via resource provider
   CEGUI::RawDataContainer texFile;
   CEGUI::System::getSingleton().getResourceProvider()->loadRawDataContainer(sFileName, texFile, sResourceGroupName);
   CEGUI::Texture* pTexture = pRenderer->getImageCodec().load(texFile, this);
   // unload file data buffer
   CEGUI::System::getSingleton().getResourceProvider()->unloadRawDataContainer(texFile);
   if (pTexture == 0)
   {
      throw CEGUI::RendererException("dtHUD::loadFromFile - " +
         pRenderer->getImageCodec().getIdentifierString() +
         " failed to load image '" + sFileName + "'.");
   }
}

void CEGUITexture::loadFromMemory(const void* pBuffer, CEGUI::uint iBufferWidth, CEGUI::uint iBufferHeight, CEGUI::Texture::PixelFormat pixFormat)
{
   GLint comps;
   GLenum format;
   switch (pixFormat)
   {
   case PF_RGB:
       comps = 3;
       format = GL_RGB;
       break;
   case PF_RGBA:
       comps = 4;
       format = GL_RGBA;
       break;
   };


   m_pTexture->setInternalFormat(format);
   ResizeToMinPOT(iBufferWidth, iBufferHeight);

   updateCachedScaleValues();

   unsigned char *pImageBuffer = new unsigned char[iBufferWidth*iBufferHeight*comps];
   for (unsigned int i=0; i<iBufferWidth*iBufferHeight*comps; i++)
   {
      pImageBuffer[i] = reinterpret_cast<const unsigned char*>(pBuffer)[i];
   }

   osg::Image *img = new osg::Image();
   img->setImage(iBufferWidth, iBufferHeight, 0,
                 comps,
                 format,
                 GL_UNSIGNED_BYTE,
                 pImageBuffer,
                 osg::Image::USE_NEW_DELETE);
   m_pTexture->setImage(img);
}

CEGUI::ushort CEGUITexture::getWidth() const
{
   return m_pTexture->getTextureWidth();
}

CEGUI::ushort CEGUITexture::getHeight() const
{
   return m_pTexture->getTextureHeight();
}

CEGUI::ushort CEGUITexture::getOriginalWidth() const
{
   return m_usOriginalWidth;
}

CEGUI::ushort CEGUITexture::getOriginalHeight() const
{
   return m_usOriginalHeight;
}

unsigned int CEGUITexture::GetTextureID(osg::GraphicsContext *pGraphicsContext)
{
   if (! m_pTexture->getTextureObject(pGraphicsContext->getState()->getContextID()))
   {
      m_pTexture->apply(*pGraphicsContext->getState());
   }
   if (! m_pTexture->getTextureObject(pGraphicsContext->getState()->getContextID()))
   {
      throw CEGUI::RendererException("dtHUD::GetTextureID 'cannot create valid object'");
   }
   return m_pTexture->getTextureObject(pGraphicsContext->getState()->getContextID())->_id;
}

void CEGUITexture::updateCachedScaleValues()
{
   CEGUI::ushort usOriginalWidth = getOriginalWidth();
   CEGUI::ushort usScaledWidth = getWidth();
   m_fScaleX = 1.0f / ((usOriginalWidth == usScaledWidth) ?
      static_cast<float>(usOriginalWidth) :
      static_cast<float>(usScaledWidth));

   CEGUI::ushort usOriginalHeight = getOriginalHeight();
   CEGUI::ushort usScaledHeight = getHeight();
   m_fScaleY = 1.0f / ((usOriginalHeight == usScaledHeight) ?
      static_cast<float>(usOriginalHeight) :
      static_cast<float>(usScaledHeight));
}

void CEGUITexture::ResizeToMinPOT(unsigned short usWidth, unsigned short usHeight)
{
   m_usOriginalWidth = usWidth;
   m_usOriginalHeight = usHeight;

//   unsigned int uiMax;
//   (usWidth>usHeight) ? uiMax=usWidth : uiMax=usHeight;
//   unsigned int uiPot = 1;
//   while (uiPot<uiMax) uiPot<<=1;

  //unsigned char* pBuffer = new unsigned char[uiPot * uiPot * 4];
//   m_pTexture->setTextureSize(uiPot,uiPot);
  //delete[] pBuffer;

   m_pTexture->setTextureSize(m_usOriginalWidth, m_usOriginalHeight);
   updateCachedScaleValues();
}


