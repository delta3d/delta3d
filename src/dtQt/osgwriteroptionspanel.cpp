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
      TextureWriteOption(std::string text, std::string value, TextureWriteOptionE e )
         : mUIText(text)
         , mValue(value)
         , mEnumValue(e)
      {

      }

      std::string mUIText;
      std::string mValue;
      TextureWriteOptionE mEnumValue;
   };

   static const TextureWriteOption _TextureWriteOptions[] = {
      TextureWriteOption( "Include Data", "IncludeData", EMBED ),
      TextureWriteOption( "Include File", "IncludeFile", EMBED ),
      TextureWriteOption( "External", "UseExternal", EXTERNAL ),
      TextureWriteOption( "Write Out", "WriteOut", EXTERNAL_WRITE ),
      TextureWriteOption( "No Textures", "", NONE )
   };



   struct TextureCompressionOption
   {
      TextureCompressionOption(std::string text, osg::Texture::InternalFormatMode e)
         : mUIText(text)
         , mEnumValue(e)
      {

      }


      std::string mUIText;
      osg::Texture::InternalFormatMode mEnumValue;
   };

   static const TextureCompressionOption _TextureCompressionOptions[] = {
      TextureCompressionOption( "None", osg::Texture::USE_IMAGE_DATA_FORMAT ),
      TextureCompressionOption( "Auto", osg::Texture::USE_USER_DEFINED_FORMAT ),
      TextureCompressionOption(  "ARB", osg::Texture::USE_ARB_COMPRESSION ),
      TextureCompressionOption( "DXT1", osg::Texture::USE_S3TC_DXT1_COMPRESSION ),
      TextureCompressionOption( "DXT3", osg::Texture::USE_S3TC_DXT3_COMPRESSION ),
      TextureCompressionOption( "DXT5", osg::Texture::USE_S3TC_DXT5_COMPRESSION )
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class OsgWriterOptionsPreset : public osg::Referenced
   {
   public:
      bool mOptimize;
      bool mCompress;
      bool mOriginalReferences;
      std::string mTextureWriteOption;

      OsgWriterOptionsPreset()
         : mOptimize(false)
         , mCompress(false)
         , mOriginalReferences(false)
      {}

   protected:
      virtual ~OsgWriterOptionsPreset()
      {}
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgWriterOptionsPanel::OsgWriterOptionsPanel(QWidget* parent)
      : BaseClass(parent)
      , mUI(new Ui::OsgWriterOptionsPanel)
      , mBinaryMode(false)
      , mOptions(new osgDB::ReaderWriter::Options())
      , mTextureCompression(_TextureCompressionOptions[0].mEnumValue)
   {
      mUI->setupUi(this);

      CreateConnections();

      CreateUIOptions();

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

      connect(mUI->mTextureCompression, SIGNAL(currentIndexChanged(int)),
         this, SLOT(OnTextureCompressionChanged()));
   }

   void OsgWriterOptionsPanel::CreateUIOptions()
   {
      // Texture Write Options
      const TextureWriteOption* curOption = nullptr;
      size_t limit = sizeof(_TextureWriteOptions) / sizeof(TextureWriteOption);
      for (size_t i = 0; i < limit; ++i)
      {
         curOption = &_TextureWriteOptions[i];

         QString qstr(tr(curOption->mUIText.c_str()));
         mUI->mTextureWrite->addItem(qstr);
      }

      // Texture Compression Options
      const TextureCompressionOption* curCompressOption = nullptr;
      limit = sizeof(_TextureCompressionOptions) / sizeof(TextureCompressionOption);
      for (size_t i = 0; i < limit; ++i)
      {
         curCompressOption = &_TextureCompressionOptions[i];

         QString qstr(tr(curCompressOption->mUIText.c_str()));
         mUI->mTextureCompression->addItem(qstr);
      }
   }

   void OsgWriterOptionsPanel::SetBinaryMode(bool binaryMode)
   {
      mBinaryMode = binaryMode;

      UpdateData();
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

   void OsgWriterOptionsPanel::OnTextureCompressionChanged()
   {
      UpdateData();
   }

   OsgOptionsPtr OsgWriterOptionsPanel::GetOptions() const
   {
      return mOptions;
   }

   void OsgWriterOptionsPanel::SetTextureCompressionOption(osg::Texture::InternalFormatMode compression)
   {
      mTextureCompression = compression;
   }

   osg::Texture::InternalFormatMode OsgWriterOptionsPanel::GetTextureCompressionOption() const
   {
      return mTextureCompression;
   }

   void OsgWriterOptionsPanel::UpdateUI()
   {
      // Texture Write Option
      std::string value = mOptions->getPluginStringData(TEXTURE_WRITE_OPTION);
      int index = mUI->mTextureWrite->findText(value.c_str());
      mUI->mTextureWrite->setCurrentIndex(index < 0 ? 0 : index);

      // Texture Compression Option
      index = 0;
      const TextureCompressionOption* curCompressOption = nullptr;
      size_t limit = sizeof(_TextureCompressionOptions) / sizeof(TextureCompressionOption);
      for (size_t i = 0; i < limit; ++i)
      {
         curCompressOption = &_TextureCompressionOptions[i];

         if (curCompressOption->mEnumValue == mTextureCompression)
         {
            index = i;
            break;
         }
      }

      mUI->mTextureCompression->setCurrentIndex(index);
   }

   void OsgWriterOptionsPanel::UpdateData()
   {
      // General plugin writer options
      const TextureWriteOption* textureWriteOption = &_TextureWriteOptions[mUI->mTextureWrite->currentIndex()];
      TextureWriteOptionE enumVal = textureWriteOption->mEnumValue;

      const TextureCompressionOption* textureCompressionOption = &_TextureCompressionOptions[mUI->mTextureCompression->currentIndex()];
      mTextureCompression = textureCompressionOption->mEnumValue;

      bool exportTextures = enumVal != NONE;
      bool externalTextures = enumVal != EMBED || ! mBinaryMode;
      bool embedTextures = ! externalTextures;
      bool compress = mTextureCompression != osg::Texture::USE_IMAGE_DATA_FORMAT;
      bool writeFiles = enumVal == EXTERNAL_WRITE;
      bool originalRefs = mUI->mUseOriginalRefs->isChecked();
      std::string optionStr;

      if (exportTextures)
      {
         // Set the hint to write images.
         mOptions->setPluginStringData(TEXTURE_WRITE_OPTION, textureWriteOption->mValue);

         optionStr = "OutputTextureFiles";

         if (compress)
         {
            optionStr += " compressed";
            optionStr += " compressImageData";
         }

         if (externalTextures)
         {
            optionStr += " noTexturesInIVEFile"; // IVE
            //optionStr += " inlineExternalReferencesInIVEFile"; // IVE

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
            // OSG will default to "IncludeData" if not specified otherwise.

            if (textureWriteOption->mValue == "IncludeFile")
            {
               optionStr += " includeImageFileInIVEFile";
               //optionStr += " noWriteExternalReferenceFiles"; // IVE
            }
         }

         mOptions->setOptionString(optionStr);
      }
      else
      {
         mOptions->removePluginStringData(TEXTURE_WRITE_OPTION);
         optionStr += " noTexturesInIVEFile"; // IVE
         optionStr += " noWriteExternalReferenceFiles"; // IVE
         mOptions->setOptionString(optionStr);
      }
   }

}
