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

#ifndef DELTA_OSG_WRITER_OPTIONS_PANEL_H
#define DELTA_OSG_WRITER_OPTIONS_PANEL_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>
#include <QtGui/qwidget.h>
#include <osg/Node>
#include <osg/Texture>
#include <osgDB/Options>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class OsgWriterOptionsPanel;
}



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   /////////////////////////////////////////////////////////////////////////////
   typedef dtCore::RefPtr<osgDB::Options> OsgOptionsPtr;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT OsgWriterOptionsPanel : public QWidget
   {
      Q_OBJECT
   public:
      typedef QWidget BaseClass;

      OsgWriterOptionsPanel(QWidget* parent = NULL);

      virtual ~OsgWriterOptionsPanel();

      void SetBinaryMode(bool binaryMode);
      bool IsBinaryMode() const;

      OsgOptionsPtr GetOptions() const;

      void SetTextureCompressionOption(osg::Texture::InternalFormatMode compression);
      osg::Texture::InternalFormatMode GetTextureCompressionOption() const;

      virtual void UpdateUI();

      virtual void UpdateData();

      bool IsOptimizeChecked() const;

   public slots:
      void SetOptimizeChecked(bool checked);

   signals:
      void SignalOptimizeChecked(bool checked);

   protected slots:
      void OnOptimizeChanged(int checkedState);

      void OnTextureWriteChanged();
      void OnTextureCompressionChanged();

   protected:
      virtual void CreateConnections();

      void CreateUIOptions();

      Ui::OsgWriterOptionsPanel* mUI;

      bool mBinaryMode;
      OsgOptionsPtr mOptions;
      osg::Texture::InternalFormatMode mTextureCompression;
   };
}

#endif
