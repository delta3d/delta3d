// texturepreview.h: Declaration of the TexturePreview class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DELTA_PSEDITOR_TEXTURE_PREVIEW
#define DELTA_PSEDITOR_TEXTURE_PREVIEW

#include <QtGui/QLabel>
#include <QtGui/QPaintEvent>

#include <osg/Image>

#include <osgDB/ReadFile>

////////////////////////////////////////////////////////////////////////////////

/**
 * An widget that displays a preview of an OSG image.
 */
class TexturePreview : public QLabel
{
   Q_OBJECT

public:
   TexturePreview(QWidget* parent = NULL);

   void SetTexture(const QString& texture);
   std::string GetTexture();

protected:
   void paintEvent(QPaintEvent* event);

private:
   void DrawLoadedImage(QImage& image);

   osg::ref_ptr<osg::Image> mImage;
};

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_PSEDITOR_TEXTURE_PREVIEW

