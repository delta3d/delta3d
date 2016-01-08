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
*
* Chris Rodgers
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_statesetpanel.h"
#include <dtQt/statesetpanel.h>
#include <dtQt/qtutils.h>
#include <osg/Texture>

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtCore/QObject>
#include <QtGui/QBitmap>
#include <QtGui/QDropEvent>
#include <QtWidgets/QOpenGLWidget>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   static const int COL_NAME = 0;
   static const int COL_TEX_UNIT = 1;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
	class TextureListItem : public QTreeWidgetItem
	{
	public:
      typedef QTreeWidgetItem BaseClass;

		TextureListItem(const QIcon& icon, const QString& str)
         : BaseClass()
         , mTexUnit(0)
		{
         setIcon(COL_NAME, icon);
         setText(COL_NAME, str);
      }

		dtCore::RefPtr<osg::Texture> mTexture;
      int mTexUnit;
		QPixmap mPixmap;
	};



	/////////////////////////////////////////////////////////////////////////////
	// CLASS CODE
	/////////////////////////////////////////////////////////////////////////////
	StateSetPanel::StateSetPanel(QWidget* parent)
		: BaseClass(parent)
      , mCurrentItem(nullptr)
		, mUI(new Ui::StateSetPanel)
	{
		mUI->setupUi(this);

      CreateConnections();
	}

	StateSetPanel::~StateSetPanel()
	{}

	void StateSetPanel::SetStateSet(osg::StateSet* stateset)
	{
		if (mStateSet != stateset)
		{
			mStateSet = stateset;

			UpdateUI();
		}
	}

	osg::StateSet* StateSetPanel::GetStateSet() const
	{
		return mStateSet.get();
	}

	void StateSetPanel::CreateConnections()
   {
      // Handle clicks in the list widget.
      connect(mUI->mListTextures, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
         this, SLOT(OnItemClicked(QTreeWidgetItem*, int)));

      connect(mUI->mTexUnit, SIGNAL(valueChanged(int)),
         this, SLOT(OnTextureUnitChanged(int)));
	}

	void StateSetPanel::UpdateUI()
	{
		if (mStateSet.valid())
		{
			mUI->mName->setText(mStateSet->getName().c_str());

			mUI->mListTextures->clear();

			int limit = mStateSet->getNumTextureAttributeLists();
			for (int i = 0; i < limit; ++i)
			{
				osg::StateAttribute* attr = mStateSet->getTextureAttribute(i, osg::StateAttribute::TEXTURE);
				osg::Texture* tex = attr->asTexture();
            osg::Image* img = tex->getImage(0);
            QString qstr = img->getFileName().c_str();
            mUI->mWidth->setText(QString::number(tex->getTextureWidth()));
            mUI->mHeight->setText(QString::number(tex->getTextureHeight()));

            QImage qimg;
            if (QtUtils::ConvertToQImage(*tex, 0, qimg))
            {
               QPixmap pix;
               pix.convertFromImage(qimg);
               QIcon icon = QIcon(pix);
               TextureListItem* item = new TextureListItem(icon, qstr);
               item->mTexture = tex;
               item->mPixmap = pix;
               item->mTexUnit = i;
               item->setText(COL_TEX_UNIT, QString::number(i));
               mUI->mListTextures->addTopLevelItem(item);
            }
         }
		}
		else
		{
			Clear();
		}
	}

	void StateSetPanel::UpdateData()
	{}

	int StateSetPanel::Clear()
	{
      blockSignals(true);

      mCurrentItem = nullptr;
		mUI->mName->setText("");
      mUI->mTexUnit->setValue(0);
		mUI->mListTextures->clear();

      blockSignals(false);

      return 0;
   }

   void StateSetPanel::dragEnterEvent(QDragEnterEvent* event)
   {}

   void StateSetPanel::dragMoveEvent(QDragMoveEvent* event)
   {}

   void StateSetPanel::dropEvent(QDropEvent* event)
   {
      qDebug() << event->mimeData()->formats().front();
      event->acceptProposedAction();
   }

	void StateSetPanel::OnUIChanged()
	{}

	void StateSetPanel::OnDataChanged()
	{}

	void StateSetPanel::OnItemClicked(QTreeWidgetItem* item, int column)
	{
		QList<QTreeWidgetItem*> items = mUI->mListTextures->selectedItems();
		if (items.empty())
		{
			QPixmap emptyPix;
			mUI->mPreview->setPixmap(emptyPix);
		}
		else
		{
			TextureListItem* texItem = (TextureListItem*)(items.front());
         osg::Texture* tex = texItem->mTexture.get();

         blockSignals(true);
         mCurrentItem = texItem;
         mUI->mTexUnit->setValue(texItem->mTexUnit);
         mUI->mPreview->setPixmap(texItem->mPixmap);
         mUI->mWidth->setText(QString::number(tex->getTextureWidth()));
         mUI->mHeight->setText(QString::number(tex->getTextureHeight()));
         blockSignals(false);
      }
	}

   void StateSetPanel::OnTextureUnitChanged(int unit)
   {
      // HACK:
      if (true)
         return;

      if (mCurrentItem != nullptr)
      {
         osg::Texture* currentTex = mCurrentItem->mTexture.get();

         osg::StateAttribute* attr = mStateSet->getTextureAttribute(unit, osg::StateAttribute::TEXTURE);
         osg::Texture* oldTex = attr->asTexture();

         if (oldTex != nullptr)
         {
            osg::StateAttribute::GLMode mode = mStateSet->getTextureMode(unit, GL_TEXTURE_2D);
            int newUnit = FindFreeTextureUnit();
            mStateSet->setTextureAttributeAndModes(newUnit, oldTex, mode);
         }

         mStateSet->setTextureAttributeAndModes(unit, currentTex, osg::StateAttribute::ON);
      }

      UpdateListLabels();
   }

   void StateSetPanel::UpdateListLabels()
   {
      if ( ! mStateSet.valid())
      {
         return;
      }

      int limit = mStateSet->getNumTextureAttributeLists();
      for (int i = 0; i < limit; ++i)
      {
         // Formulate item text.
         osg::StateAttribute* attr = mStateSet->getTextureAttribute(i, osg::StateAttribute::TEXTURE);
         if (attr != nullptr)
         {
            osg::Texture* tex = attr->asTexture();
            QString name = tex->getImage(0)->getFileName().c_str();

            // Find the matching item by texture.
            TextureListItem* texItem = nullptr;
            int numItems = mUI->mListTextures->topLevelItemCount();
            for (int j = 0; j < numItems; ++j)
            {
               TextureListItem* tmpItem = (TextureListItem*)mUI->mListTextures->itemAt(j, 0);
               if (tmpItem->mTexture.get() == tex)
               {
                  texItem = tmpItem;
                  break;
               }
            }

            // Set the item text and texture unit.
            if (texItem != nullptr)
            {
               texItem->setText(COL_NAME, name);
               texItem->setText(COL_TEX_UNIT, QString::number(i));
               texItem->mTexUnit = i;
            }
         }
      }
   }

   int StateSetPanel::FindFreeTextureUnit() const
   {
      int texUnit = 0;

      for (int i = 0; i < 32; ++i)
      {
         if (nullptr == mStateSet->getTextureAttribute(i, osg::StateAttribute::TEXTURE))
         {
            texUnit = i;
         }
      }

      return texUnit;
   }

}
