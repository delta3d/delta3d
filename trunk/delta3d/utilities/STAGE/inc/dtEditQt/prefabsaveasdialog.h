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
 * Jeffrey P. Houde
 */

#ifndef DELTA_PREFAB_SAVE_DIALOG
#define DELTA_PREFAB_SAVE_DIALOG

#include <QtGui/QDialog>

class QLineEdit;
class QTextEdit;
class QCloseEvent;
class QPushButton;
class QToolButton;

namespace dtEditQt
{
   class PrefabSaveDialog : public QDialog
   {
      Q_OBJECT

   public:
      /// Constructor
      PrefabSaveDialog(QWidget* parent = NULL);

      /// Destructor
      virtual ~PrefabSaveDialog() { }

      /// Retrieves the category of the prefab the user entered
      const std::string getPrefabCategory();

      /// Retrieves the name of the prefab the user entered
      const std::string getPrefabName();

      /// Retrieves the filename of the prefab
      const std::string getPrefabFileName();

      /// Retrieves the description of the prefab
      const std::string getPrefabDescription();

      /// Retrieves the icon file name of the prefab
      const std::string GetPrefabIconFileName();

      /// Sets the category of the prefab
      void setPrefabCategory(const std::string cat);

   public slots:
      /// slot for receiving the text changing signal
      void edited(const QString& newText);
      
      /// slot for changing the Prefab icon
      void IconChanged();

   private:
      QPushButton* okButton;
      QLineEdit*   nameEdit;
      QLineEdit*   mCategoryEdit;
      std::string  mIconFilePath;
      QToolButton* mIconButton;
      QIcon*       mIcon;
      QTextEdit*   descEdit;
      //QLineEdit*   fileEdit;

      bool EnsureIconFolderExists();
   };

} // namespace dtEditQt

#endif // DELTA_PREFAB_SAVE_DIALOG
