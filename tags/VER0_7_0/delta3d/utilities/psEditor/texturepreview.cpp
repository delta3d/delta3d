// texturepreview.cpp: Implementation of the TexturePreview class.
//
//////////////////////////////////////////////////////////////////////

#include "texturepreview.h"


/**
 * Constructor.
 *
 * @param x the x coordinate of the widget
 * @param y the y coordinate of the widget
 * @param w the width of the widget
 * @param h the height of the widget
 * @param label the widget label
 */
TexturePreview::TexturePreview(int x, int y, int w, int h, const char *label)
   : Fl_Widget(x, y, w, h, label)
{}

/**
 * Sets the texture to display.
 *
 * @param texture the texture path
 */
void TexturePreview::SetTexture(std::string texture)
{
   mImage = osgDB::readImageFile(texture, osgDB::Registry::CACHE_IMAGES);
         
   redraw();
}

/**
 * Retrieves the path of the texture being displayed.
 *
 * @return the path of the texture being displayed
 */
std::string TexturePreview::GetTexture()
{
   if(mImage.valid())
   {
      return mImage->getFileName();
   }
   else
   {
      return "";
   }
}

/**
 * Draws the widget.
 */
void TexturePreview::draw()
{
   if(mImage.valid())
   {
      fl_draw_image(drawImageCallback, this, x(), y(), w(), h());
   }
   else
   {
      fl_color(255, 255, 255);
      
      fl_rectf(x(), y(), w(), h());
   }
}

/**
 * Per-line image drawing callback.
 *
 * @param data the user data passed to the callback
 * @param x the x location at which to draw
 * @param y the y location at which to draw
 * @param w the width of the line
 * @param buf the buffer to fill
 */
void TexturePreview::drawImageCallback(void* data, int x, int y, int w, uchar* buf)
{
   TexturePreview* self = (TexturePreview*)data;
   
   float sStep = (float)self->mImage->s()/self->w(),
         tStep = (float)self->mImage->t()/self->h(),
         s = x*sStep;
   
   int t = (self->mImage->t()-1)-(int)(y*tStep), 
       components = osg::Image::computeNumComponents(self->mImage->getPixelFormat()),
       ptr = 0;
   
   for(int i=0;i<w;i++)
   {
      uchar* data = self->mImage->data((int)s, t);
      
      switch(components)
      {
         case 1:
            buf[ptr++] = data[0];
            buf[ptr++] = data[0];
            buf[ptr++] = data[0];
            break;
            
         case 3:
            buf[ptr++] = data[0];
            buf[ptr++] = data[1];
            buf[ptr++] = data[2];
            break;
            
         case 4:
            float alpha = data[3]/255.0f;
            buf[ptr++] = (uchar)(data[0]*alpha);
            buf[ptr++] = (uchar)(data[1]*alpha);
            buf[ptr++] = (uchar)(data[2]*alpha);
            break;
      }
      
      s += sStep;
   }
}
