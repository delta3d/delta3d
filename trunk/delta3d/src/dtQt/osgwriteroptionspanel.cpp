/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_osgwriteroptionspanel.h"
#include <dtQt/osgwriteroptionspanel.h>
#include <QtCore/QString.h>
#include <QtGui/qtreewidget.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   enum TextureWriteOptionE
   {
      NONE,
      EMBED,
      EXTERNAL,
      EXTERNAL_WRITE
   };

   static const std::string TEXTURE_WRITE_OPTION("WriteImageHint");
   struct TextureWriteOption
   {
      std::string mUIText;
      std::string mValue;
      TextureWriteOptionE mEnumValue;
   };

   static const TextureWriteOption _TextureWriteOptions[] = {
      TextureWriteOption{ "Include Data", "IncludeData", EMBED },
      TextureWriteOption{ "Include File", "IncludeFile", EMBED },
      TextureWriteOption{ "External", "UseExternal", EXTERNAL },
      TextureWriteOption{ "Write Out", "WriteOut", EXTERNAL_WRITE },
      TextureWriteOption{ "No Textures", "", NONE }
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgWriterOptionsPanel::OsgWriterOptionsPanel(QWidget* parent)
      : BaseClass(parent)
      , mUI(new Ui::OsgWriterOptionsPanel)
      , mBinaryMode(false)
      , mOptions(new osgDB::ReaderWriter::Options())
   {
      mUI->setupUi(this);

      CreateConnections();

      CreateUIOptions();

      mUI->mTextureWrite->setCurrentIndex(0);

      UpdateUI();
   }

   OsgWriterOptionsPanel::~OsgWriterOptionsPanel()
   {
      delete mUI;
      mUI = nullptr;
   }

   void OsgWriterOptionsPanel::CreateConnections()
   {
      connect(mUI->mOptimize, SIGNAL(stateChanged(int)),
         this, SLOT(OnOptimizeChanged(int)));

      connect(mUI->mTextureWrite, SIGNAL(currentIndexChanged(int)),
         this, SLOT(OnTextureWriteChanged()));
   }

   void OsgWriterOptionsPanel::CreateUIOptions()
   {
      const TextureWriteOption* curOption = nullptr;

      size_t limit = sizeof(_TextureWriteOptions) / sizeof(TextureWriteOption);
      for (size_t i = 0; i < limit; ++i)
      {
         curOption = &_TextureWriteOptions[i];

         QString qstr(tr(curOption->mUIText.c_str()));
         mUI->mTextureWrite->addItem(qstr);
      }
   }

   void OsgWriterOptionsPanel::SetBinaryMode(bool binaryMode)
   {
      mBinaryMode = binaryMode;
   }

   bool OsgWriterOptionsPanel::IsBinaryMode() const
   {
      return mBinaryMode;
   }

   bool OsgWriterOptionsPanel::IsOptimizeChecked() const
   {
      return mUI->mOptimize->isChecked();
   }

   void OsgWriterOptionsPanel::SetOptimizeChecked(bool checked)
   {
      if (mUI->mOptimize->isChecked() != checked)
      {
         mUI->mOptimize->setChecked(checked);
      }
   }

   void OsgWriterOptionsPanel::OnOptimizeChanged(int checkedState)
   {
      UpdateData();

      emit SignalOptimizeChecked(checkedState == Qt::Checked);
   }

   void OsgWriterOptionsPanel::OnTextureWriteChanged()
   {
      UpdateData();
   }

   OsgOptionsPtr OsgWriterOptionsPanel::GetOptions() const
   {
      return mOptions;
   }

   void OsgWriterOptionsPanel::UpdateUI()
   {
      // Texture Write Option
      std::string value = mOptions->getPluginStringData(TEXTURE_WRITE_OPTION);
      int index = mUI->mTextureWrite->findText(value.c_str());
      mUI->mTextureWrite->setCurrentIndex(index);
   }

   void OsgWriterOptionsPanel::UpdateData()
   {
      // General plugin writer options
      const TextureWriteOption* textureWriteOption = &_TextureWriteOptions[mUI->mTextureWrite->currentIndex()];
      mOptions->setPluginStringData(TEXTURE_WRITE_OPTION, textureWriteOption->mValue);

      TextureWriteOptionE enumVal = textureWriteOption->mEnumValue;
      mBinaryMode = true;
      bool exportTextures = enumVal != NONE;
      bool externalTextures = enumVal != EMBED || ! mBinaryMode;
      bool embedTextures = ! externalTextures;
      bool compress = mUI->mCompressTextures->isChecked();
      bool writeFiles = enumVal == EXTERNAL_WRITE;
      bool originalRefs = mUI->mUseOriginalRefs->isChecked();
      std::string optionStr;

      if (exportTextures)
      {
         optionStr = "OutputTextureFiles";

         if (compress)
         {
            optionStr += " compressed";
            optionStr += " compressImageData";
         }

         if (externalTextures)
         {
            optionStr += " noTexturesInIVEFile"; // IVE
            optionStr += " inlineExternalReferencesInIVEFile"; // IVE

            if (writeFiles)
            {
               optionStr += "includeExternalReferences"; // OSG
               optionStr += "writeExternalReferenceFiles"; // OSG
            }

            if (originalRefs)
            {
               optionStr += "useOriginalExternalReferences";
            }
         }

         if (embedTextures)
         {
            optionStr += " includeImageFileInIVEFile";
         }
      }
      else
      {
         optionStr += " noTexturesInIVEFile";
         optionStr += " noWriteExternalReferenceFiles";
      }

      mOptions->setOptionString(optionStr);
   }

}
