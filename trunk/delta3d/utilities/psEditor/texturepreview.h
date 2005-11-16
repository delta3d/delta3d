// texturepreview.h: Declaration of the TexturePreview class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4244) // for warning C4244: '=' : conversion from 'short' to 'uchar', possible loss of data
#	pragma warning(disable : 4311) // for warning C4311: 'type cast' : pointer truncation from 'void *const ' to 'long'
#	pragma warning(disable : 4312) // for warning C4312: 'type cast' : conversion from 'long' to 'void *' of greater size
#endif

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>

#ifdef _MSC_VER
#	pragma warning(pop)
#endif

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
