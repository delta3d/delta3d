/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 * William E. Johnson II
 */

#ifndef DELTA_MAP_DIALOG
#define DELTA_MAP_DIALOG

#include <QtGui/QDialog>

class QLineEdit;
class QCloseEvent;
class QPushButton;
class QTextEdit;

namespace dtCore
{
   class Map;
}

namespace dtEditQt
{
   class MapDialog : public QDialog
   {
      Q_OBJECT

   public:
      /// Constructor
      MapDialog(QWidget* parent = NULL);

      /// Destructor
      virtual ~MapDialog() { }

      inline dtCore::Map* getFinalizedMap() { return mMap; }
      inline const dtCore::Map* getFinalizedMap() const { return mMap; }

   public slots:
      /// slot for receiving the text changing signal
      void nameEdited(const QString& newText);
      void categoryEdited(const QString& newText);
      void updateFileName();

      /// slot for applying the changes made
      void applyChanges();

   private:
      QPushButton* mOkButton;
      QLineEdit*   mNameEdit;
      QTextEdit*   mDescEdit;
      QLineEdit*   mFileEdit;
      QLineEdit*   mCategoryEdit;
      dtCore::Map*  mMap;
   };

} // namespace dtEditQt

#endif // DELTA_MAP_DIALOG
