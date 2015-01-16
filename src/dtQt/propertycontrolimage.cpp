/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_propertycontrolimage.h"
#include <dtQt/propertycontrolimage.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/resourcedescriptor.h>
#include <dtCore/project.h>
#include <QtGui/QFileDialog.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // HELPER FUNCTIONS
   /////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor ConvertToResource(const QPixmap& pixmap)
   {
      dtCore::ResourceDescriptor res;

      // TODO:
      //dtCore::Project::GetInstance().

      return res;
   }

   bool ConvertToQImage(const std::string filepath, QPixmap& outQimage)
   {
      bool success = false;

      if (dtUtil::FileUtils::GetInstance().FileExists(filepath))
      {
         QString qstr(filepath.c_str());
         success = outQimage.load(qstr);
      }
      else
      {
         LOG_WARNING("Cannot access file: " + filepath);
      }
      
      return success;
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyControlImage::PropertyControlImage(QWidget* parent)
      : BaseClass(parent, dtCore::DataType::TEXTURE)
      , mUI(new Ui::PropertyControlImage)
   {
      mUI->setupUi(this);
   }

   PropertyControlImage::~PropertyControlImage()
   {
      delete mUI;
      mUI = NULL;
   }

   void PropertyControlImage::Init()
   {
      BaseClass::Init();

      mUI->mValue->installEventFilter(this);
   }

   QObject* PropertyControlImage::GetControlUI()
   {
      return mUI->mLayout;
   }

   QLabel* PropertyControlImage::GetLabel() const
   {
      return mUI->mLabel;
   }

   void PropertyControlImage::UpdateUI(const PropertyType& prop)
   {
      const dtCore::ResourceActorProperty* iprop
         = dynamic_cast<const dtCore::ResourceActorProperty*>(&prop);

      if (iprop != NULL)
      {
         QPixmap qimage;

         dtCore::ResourceDescriptor res = iprop->GetValue();
         std::string filepath = iprop->GetResourcePath(res);

         // HACK:
         /*if (filepath.empty())
         {
            dtCore::ShaderParamTexture2DProperty* shaderProp = dynamic_cast<dtCore::ShaderParamTexture2DProperty*>(prop);
            if (shaderProp != NULL)
            {
               filepath = shaderProp->GetShaderParam()->GetTexture();
            }
         }*/

         if (ConvertToQImage(filepath, qimage))
         {
            mUI->mValue->setPixmap(qimage);
         }
      }
   }

   void PropertyControlImage::UpdateData(PropertyType& propToUpdate)
   {
      const QPixmap* qimage = GetUIImage();

      dtCore::ResourceActorProperty* prop
         = dynamic_cast<dtCore::ResourceActorProperty*>(&propToUpdate);
      if (prop != NULL && qimage != NULL)
      {
         dtCore::ResourceDescriptor res
            = ConvertToResource(*qimage);

         prop->SetValue(res);
      }
   }

   const QPixmap* PropertyControlImage::GetUIImage() const
   {
      return mUI->mValue->pixmap();
   }

   void PropertyControlImage::SetUIImage(QPixmap qimage)
   {
      mUI->mValue->setPixmap(qimage);

      // Signal the change. Property will be set in UpdateData.
      SignalUIChanged();
   }
   
   void PropertyControlImage::GetImagePick()
   {
      const QPixmap* oldFile = GetUIImage();

      QString file = QFileDialog::getOpenFileName(this,
         tr("Open Image"), ".", tr("Image Files (*.png *.jpg *.bmp *.tga *.jpg *.tiff)"));;

      QFileInfo info(file);

      if (info.exists())
      {
         QPixmap qimage;
         if (qimage.load(file))
         {
            SetUIImage(qimage);
         }
      }
   }

   bool PropertyControlImage::eventFilter(QObject *obj, QEvent *qevent)
   {
      if (qevent->type() == QEvent::MouseButtonPress)
      {
         GetImagePick();
         return true;
      }

      // standard event processing
      return QObject::eventFilter(obj, qevent);
   }

}
