// texturepreview.cpp: Implementation of the TexturePreview class.
//
////////////////////////////////////////////////////////////////////////////////

#include <texturepreview.h>
#include <QtGui/QImage>
#include <QtGui/QPainter>

////////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param x the x coordinate of the widget
 * @param y the y coordinate of the widget
 * @param w the width of the widget
 * @param h the height of the widget
 * @param label the widget label
 */
TexturePreview::TexturePreview(QWidget* parent /*= NULL*/)
: QLabel(parent)
, mImage(NULL)
{
}

/**
 * Sets the texture to display.
 *
 * @param texture the texture path
 */
void TexturePreview::SetTexture(const QString& texture)
{
   osg::ref_ptr <osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;
   options.get()->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_IMAGES);

   mImage = osgDB::readImageFile(texture.toStdString(), options.get());
   
   repaint();
}

/**
 * Retrieves the path of the texture being displayed.
 *
 * @return the path of the texture being displayed
 */
std::string TexturePreview::GetTexture()
{
   if (mImage.valid())
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
void TexturePreview::paintEvent(QPaintEvent* event)
{
   QImage qImage = QImage(width(), height(), QImage::Format_ARGB32);
   qImage.fill(qRgb(255, 255, 255));
   QPainter painter(this);

   if(mImage.valid())
   {
      int w = std::max(mImage->s(), width());
      int h = std::max(mImage->t(), height());
      qImage = QImage(w, h, QImage::Format_ARGB32);
      DrawLoadedImage(qImage);
   }

   painter.drawImage(QPoint(0, 0), qImage);
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
void TexturePreview::DrawLoadedImage(QImage& image)
{
   float sStep = (float)mImage->s() / width();
   float tStep = (float)mImage->t() / height();
   float s = 0.0f;
   float t = mImage->t() - tStep;

   int components = osg::Image::computeNumComponents(mImage->getPixelFormat());

   for(int x = 0; x < image.width() && s < mImage->s(); ++x)
   {
      t = mImage->t() - tStep;
      for(int y = 0; y < image.height() && t >= 0; ++y)
      {
         uchar* data = mImage->data((int)s, (int)t);
         QColor pixelColor;
         switch(components)
         {
         case 1:
            pixelColor.setRgb((int)data[0], (int)data[0], (int)data[0]);
            break;

         case 3:
            pixelColor.setRgb((int)data[0], (int)data[1], (int)data[2]);
            break;

         case 4:
            float alpha = data[3] / 255.0f;
            pixelColor.setRgb((int)data[0] * alpha, (int)data[1] * alpha, (int)data[2] * alpha);
            break;
         }
         image.setPixel(x, y, pixelColor.rgba());
         t -= tStep;
      }
      s += sStep;
   }
}

////////////////////////////////////////////////////////////////////////////////

