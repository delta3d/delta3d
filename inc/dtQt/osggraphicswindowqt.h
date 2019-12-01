/* -*-c++-*-
 * Delta3D
 * Copyright (C) 2007-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#ifndef OSGGRAPHICSWINDOWQT_H_
#define OSGGRAPHICSWINDOWQT_H_

#include <dtQt/export.h>
#include <QtCore/Qt>
#include <QtCore/QObject>
#include <osg/Version>
#include <osgViewer/GraphicsWindow>

class QGLWidget;

namespace dtQt
{
   class OSGAdapterWidget;
   class GLWidgetFactory;

   class DT_QT_EXPORT OSGGraphicsWindowQt : public osgViewer::GraphicsWindow
   {
   public:
      typedef osgViewer::GraphicsWindow BaseClass;

      OSGGraphicsWindowQt(osg::GraphicsContext::Traits* traits,
                          dtQt::GLWidgetFactory* factory = NULL,
                          dtQt::OSGAdapterWidget* adapter = NULL);
      virtual ~OSGGraphicsWindowQt();

      void SetQGLWidget(QGLWidget* qwidget);
      QGLWidget* GetQGLWidget();
      const QGLWidget* GetQGLWidget() const;

      virtual bool isSameKindAs(const Object* object) const;
      virtual const char* libraryName() const;
      virtual const char* className() const;

      virtual bool valid() const;

      /** Realise the GraphicsContext.*/
      virtual bool realizeImplementation();

      /** Return true if the graphics context has been realised and is ready to use.*/
      virtual bool isRealizedImplementation() const;

      /** Close the graphics context.*/
      virtual void closeImplementation();

      /** Make this graphics context current.*/
      virtual bool makeCurrentImplementation();

      /** Release the graphics context.*/
      virtual bool releaseContextImplementation();

      /** Swap the front and back buffers.*/
      virtual void swapBuffersImplementation();

      /** Check to see if any events have been generated.*/
#if OSG_VERSION_LESS_THAN(3,2,0)
      virtual void checkEvents();
#else
      virtual bool checkEvents();
#endif
      /** Get the window's position and size.*/
      virtual void getWindowRectangle(int& x, int& y, int& width, int& height);

      /** Set the window's position and size.*/
      virtual bool setWindowRectangleImplementation(int x, int y, int width, int height);

      /** Set Window decoration.*/
      virtual bool setWindowDecorationImplementation(bool flag);

      /** Get focus.*/
      virtual void grabFocus();

      /** Get focus on if the pointer is in this window.*/
      virtual void grabFocusIfPointerInWindow();

      void requestClose();
      virtual void resizedImplementation(int x, int y, int width, int height);

      virtual void setWindowName (const std::string & name);
      virtual void useCursor(bool cursorOn);
      virtual void setCursor(osgViewer::GraphicsWindow::MouseCursor mouseCursor);

      virtual void requestWarpPointer(float x, float y);

   private:
      bool mValid;
      bool mRealized;
      bool mCloseRequested;
      QGLWidget* mQWidget;
      Qt::CursorShape mCursorShape;
   };

} // namespace dtQt

#endif // OSGGRAPHICSWINDOWQT_H_
