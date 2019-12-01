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
#include "ui_propertycontrolresource.h"
#include <dtQt/propertycontrolresource.h>
#include <QtGui/QFileDialog.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyControlResource::PropertyControlResource(QWidget* parent, dtCore::DataType& dataType)
      : BaseClass(parent, dataType)
      , mUI(new Ui::PropertyControlResource)
   {
      mUI->setupUi(this);
   }

   PropertyControlResource::~PropertyControlResource()
   {
      delete mUI;
      mUI = NULL;
   }

   QObject* PropertyControlResource::GetControlUI()
   {
      return mUI->mLayout;
   }

   QLabel* PropertyControlResource::GetLabel() const
   {
      return mUI->mLabel;
   }

   void PropertyControlResource::CreateConnections()
   {
      BaseClass::CreateConnections();

      connect(mUI->mButton, SIGNAL(clicked(bool)),
         this, SLOT(OnButtonClicked()));
   }

   void PropertyControlResource::SetResource(const dtCore::ResourceDescriptor& res)
   {
      dtCore::ResourceDescriptor oldRes = GetResource();

      if (oldRes.GetResourceIdentifier() != res.GetResourceIdentifier())
      {
         QString qtext(tr(res.GetResourceIdentifier().c_str()));
         mUI->mValue->setText(qtext);
         mUI->mValue->setToolTip(qtext);

         // Since UI was changed, trigger an update to data.
         emit SignalUIChanged();
      }
   }

   dtCore::ResourceDescriptor PropertyControlResource::GetResource() const
   {
      std::string value(mUI->mValue->text().toStdString());
      dtCore::ResourceDescriptor res(value);
      return res;
   }

   PropertyControlResource::ResourceProperty*
      PropertyControlResource::GetResourceProperty() const
   {
      return dynamic_cast<ResourceProperty*>(GetLinkedProperty());
   }

   void PropertyControlResource::UpdateUI(const PropertyType& prop)
   {
      const dtCore::ResourceActorProperty* rprop
         = dynamic_cast<const dtCore::ResourceActorProperty*>(&prop);

      if (rprop != NULL)
      {
         dtCore::ResourceDescriptor res = rprop->GetValue();

         QString qtext(tr(res.GetResourceIdentifier().c_str()));
         mUI->mValue->setText(qtext);
         mUI->mValue->setToolTip(qtext);
      }
   }

   void PropertyControlResource::UpdateData(PropertyType& propToUpdate)
   {
      dtCore::ResourceActorProperty* rprop
         = dynamic_cast<dtCore::ResourceActorProperty*>(&propToUpdate);

      if (rprop != NULL)
      {
         std::string value(mUI->mValue->text().toStdString());
         dtCore::ResourceDescriptor res(value);
         rprop->SetValue(res);
      }
   }

   void PropertyControlResource::OnButtonClicked()
   {
      QString file = QFileDialog::getOpenFileName(this,
         tr("Open Image"), ".", tr("Image Files (*.png *.jpg *.bmp *.tga *.jpg *.tiff)"));

      QFileInfo info(file);

      if (info.exists())
      {
         // TODO:
      }
   }

}
