// texturepreview.h: Declaration of the TexturePreview class.
//
//////////////////////////////////////////////////////////////////////

#include <FL/Fl_Widget.h>
#include <FL/fl_draw.h>

#include <osg/Image>

#include <osgDB/ReadFile>

/**
 * An FLTK widget that displays a preview of an OSG image.
 */
class TexturePreview : public Fl_Widget
{
   public:
      
      /**
       * Constructor.
       *
       * @param x the x coordinate of the widget
       * @param y the y coordinate of the widget
       * @param w the width of the widget
       * @param h the height of the widget
       * @param label the widget label
       */
      TexturePreview(int x, int y, int w, int h, const char *label = 0);
      
      /**
       * Sets the texture to display.
       *
       * @param texture the texture path
       */
      void SetTexture(std::string texture);
      
      /**
       * Retrieves the path of the texture being displayed.
       *
       * @return the path of the texture being displayed
       */
      std::string GetTexture();
      
      /**
       * Draws the widget.
       */
      virtual void draw();
      
      
   private:
      
      /**
       * Per-line image drawing callback.
       *
       * @param data the user data passed to the callback
       * @param x the x location at which to draw
       * @param y the y location at which to draw
       * @param w the width of the line
       * @param buf the buffer to fill
       */
      static void drawImageCallback(void* data, int x, int y, int w, uchar* buf);
      
      /**
       * The texture image.
       */
      osg::ref_ptr<osg::Image> mImage;   
};
