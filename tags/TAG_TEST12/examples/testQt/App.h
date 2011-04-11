/* -*-c++-*-
 * testQt  - Using 'The MIT License'
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
 */
#ifndef DELTA_CollisionViewer
#define DELTA_CollisionViewer

#include <dtABC/application.h>
#include "MainWindow.h"
#include <QtCore/QObject>

namespace dtCore
{
   class Object;
   class OrbitMotionModel;
}
class App : public QObject, public dtABC::Application
{
   Q_OBJECT
   public:
      App(const std::string& configFilename = "config.xml");
 
      virtual void Config();

   signals:
      void FileLoaded(bool valid);

   protected:
     virtual ~App();

   private slots:
     void OnLoadFile(const QString& filename);

   private:
    MainWindow mMainWindow; ///<QMainWindow
    dtCore::RefPtr<dtCore::Object> mObject1;
    dtCore::RefPtr<dtCore::OrbitMotionModel> mCameraMotion;
};

#endif // DELTA_CollisionViewer
