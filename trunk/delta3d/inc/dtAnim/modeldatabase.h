/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * David Guthrie and Bradley Anderegg
 */

#ifndef __DELTA_MODEL_DATABASE_H__
#define __DELTA_MODEL_DATABASE_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/export.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/basemodeldata.h>
#include <dtAnim/basemodelloader.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/characterfileloader.h>
#include <dtAnim/posemeshdatabase.h>
#include <dtUtil/threadpool.h>
#include <dtCore/resourcedescriptor.h>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT ModelDatabase : public osg::Referenced
   {
   public:
      typedef std::vector<dtCore::RefPtr<dtAnim::BaseModelData> >  ModelDataArray;

      static void Destroy();

      static bool IsAvailable();

      static ModelDatabase& GetInstance();

      bool IsFileValid(const dtCore::ResourceDescriptor& filename);

      void SetHardwareMode(bool hardwareMode);
      bool IsHardwareMode() const;

      bool IsHardwareSupported() const;

      /**
       * Loads a model internally
       * @return true if the model loaded successfully.
       */
      bool Load(const dtCore::ResourceDescriptor& resource);

      void TruncateDatabase();

      bool RegisterModelData(dtAnim::BaseModelData& modelData);
      bool UnregisterModelData(dtAnim::BaseModelData& modelData);

      dtAnim::BaseModelData* GetModelData(const dtCore::ResourceDescriptor& resource);

      dtAnim::BaseModelData* Find(const dtCore::ResourceDescriptor& resource);

      const dtAnim::BaseModelData* Find(const dtCore::ResourceDescriptor& resource) const;

      dtCore::RefPtr<dtAnim::BaseModelWrapper> CreateModelWrapper(dtAnim::BaseModelData& data);

      dtAnim::BaseModelLoader* GetModelLoader(const std::string& characterSystem) const;

      bool AddModelLoader(dtAnim::BaseModelLoader& modelLoader);

      bool RemoveModelLoader(dtAnim::BaseModelLoader& modelLoader);

      dtAnim::AnimNodeBuilder* GetNodeBuilder() const;

      // Convenience method for wrapping a node builder, if one exists.
      dtCore::RefPtr<osg::Node> CreateNode(dtAnim::BaseModelWrapper& model, bool immediate = true);

      dtAnim::PoseMeshDatabase* GetPoseMeshDatabase(dtAnim::BaseModelWrapper& wrapper);

   protected:
      ModelDatabase();

      virtual ~ModelDatabase();

      bool InternalLoad(const dtCore::ResourceDescriptor& resource, dtCore::RefPtr<dtAnim::BaseModelData>& outModelData);

      static dtCore::RefPtr<ModelDatabase> mInstance;
      
      bool mHardwareMode;
      
      dtCore::RefPtr<dtAnim::AnimNodeBuilder> mNodeBuilder;
      dtCore::RefPtr<dtAnim::CharacterFileLoader> mFileLoader;

      typedef std::map<std::string, dtCore::RefPtr<dtAnim::BaseModelLoader> > ModelLoaderMap;
      ModelLoaderMap mModelLoaders;

      typedef dtCore::RefPtr<dtAnim::PoseMeshDatabase> PoseDatabase;
      typedef std::map<std::string, PoseDatabase> PoseDatabaseMap;
      PoseDatabaseMap mPoseMeshMap;
      
      mutable OpenThreads::Mutex mAsynchronousLoadLock;
      mutable OpenThreads::Mutex mLoadingLock;
      
      ModelDataArray mModelData;
      
      friend class LoadTask;
   };

} // namespace dtAnim

#endif
