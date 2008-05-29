/*
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2008 MOVES Institute 
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
*/

#ifndef DELTA_OBJECT_VIEWER
#define DELTA_OBJECT_VIEWER

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/object.h>
#include <dtABC/application.h>

#include <dtAnim/posemeshdatabase.h>
#include <dtAnim/posemeshutility.h>
#include <dtAnim/posemesh.h>
#include <dtAnim/attachmentcontroller.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////

class QColor;

namespace dtCore
{
   class OrbitMotionModel;
}

namespace osg
{
   class Group;
}

namespace dtAnim
{
   class CharDrawable;
   class Cal3DDatabase;
   class PoseMeshDatabase;
}

///////////////////////////////////////////////////////////////////////////////

class ObjectViewer: public QObject, public dtABC::Application
{
   Q_OBJECT

public:
	ObjectViewer();
	~ObjectViewer();

   virtual void Config();

public slots:
  
   void OnLoadShaderFile(const QString &filename);
   void OnLoadGeometryFile(const std::string &filename);
   void OnUnloadGeometryFile();
   void OnApplyShader(const std::string &groupName, const std::string &programName);
   void OnRemoveShader();
   void OnSetShaded();
   void OnSetWireframe();
   void OnSetShadedWireframe();
   void OnToggleGrid(bool shouldDisplay);    

signals:

   void AnimationLoaded(unsigned int, const QString &, unsigned int trackCount,
                        unsigned int keyframes, float duration);

   void ShaderLoaded(const std::string &shaderGroup, const std::string &shaderName);   

   void MaterialLoaded(int materialID, const QString &name, 
                       const QColor &diffuse, const QColor &ambient, const QColor &specular,
                       float shininess);

   void ErrorOccured( const QString &msg );

   void BlendUpdate(const std::vector<float> &weightList);

protected:
   virtual void PostFrame( const double deltaFrameTime );  

private:  
 
   dtCore::RefPtr<dtCore::Object> mObject;
   dtCore::RefPtr<dtCore::OrbitMotionModel> mMotion;

   dtCore::RefPtr<osg::Group> mShadedScene;
   dtCore::RefPtr<osg::Group> mUnShadedScene;
   dtCore::RefPtr<osg::Group> mWireDecorator;
   dtCore::RefPtr<osg::Group> mShadeDecorator;
   dtCore::RefPtr<osg::Geode> mGridGeode;

   void InitShadeDecorator();
   void InitWireDecorator();
   void InitGridPlanes();
};

#endif // DELTA_OBJECT_VIEWER
