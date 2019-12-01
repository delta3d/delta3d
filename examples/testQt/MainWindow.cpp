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
#include "MainWindow.h"
#include <dtUtil/datapathutils.h>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

////////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QMainWindow* parent)
: QMainWindow(parent)
, mLastMeshInfo(NULL)
{
  ui.setupUi(this);

  mLastMeshInfo = new QFileInfo(QString::fromStdString(dtUtil::GetEnvironment("DELTA_ROOT") + "/examples/data/StaticMeshes/"));
}

////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
   delete mLastMeshInfo;
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::SetGraphicsWidget(QWidget* graphics)
{
   QWidget* w = centralWidget();
   QLayout* l = w->layout();
   l->layout()->addWidget(graphics); 
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionOpen_triggered()
{
   const QString filename = QFileDialog::getOpenFileName(this,
                               tr("Load mesh"),
                               mLastMeshInfo->absolutePath(),
                               "*.ive");
   if (filename.isEmpty())
   {
      //user canceled
      return;
   }

   mLastMeshInfo->setFile(filename);
   emit LoadFile(filename);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionAboutQt_triggered()
{
   QApplication::aboutQt();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::OnFileLoaded(bool valid)
{
   if (valid == false)
   {
      QMessageBox::critical(this, tr("Can't load mesh"), tr("Could not load file: ") + mLastMeshInfo->fileName());
   }
}
