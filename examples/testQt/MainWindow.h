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
#ifndef MainWindow_h__
#define MainWindow_h__

#include <QtGui/QMainWindow>
#include "ui_MainWindow.h"
#include <dtCore/refptr.h>

class QFileInfo;


class MainWindow : public QMainWindow
{
   Q_OBJECT
public:
	MainWindow(QMainWindow* parent = NULL);
	virtual ~MainWindow();

   void SetGraphicsWidget(QWidget* graphics);

 public slots:
   void OnFileLoaded(bool valid);

signals:
   void LoadFile(const QString& filename);

private slots:
   //auto-generated slots
   void on_actionOpen_triggered();
   void on_actionAboutQt_triggered();

private:
  Ui::MainWindow ui;

  QFileInfo* mLastMeshInfo;
};
#endif // MainWindow_h__
