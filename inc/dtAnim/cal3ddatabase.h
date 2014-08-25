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
 * David Guthrie and Bradley Anderegg 07/17/2007
 */

#ifndef DELTA_CAL3D_DATABASE
#define DELTA_CAL3D_DATABASE

#include <dtAnim/export.h>
#include <dtAnim/cal3dloader.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtCore/refptr.h>

#include <osg/Referenced>

#include <string>
#include <vector>

class CalCoreModel;

namespace dtAnim
{
   class Cal3DModelData;
   class PoseMeshDatabase;

   class DT_ANIM_EXPORT Cal3DDatabase: public osg::Referenced
   {
      public:
         typedef std::vector<dtCore::RefPtr<Cal3DModelData> >  ModelDataArray;

      public:

         static Cal3DDatabase& GetInstance();
         static void Destroy();

         /// Try to parse a file as a character file
         bool IsFileValid(const std::string& filename);
         
         ///Load an animated entity definition file and return the Cal3DModelWrapper
         dtCore::RefPtr<Cal3DModelWrapper> Load(const std::string& filename);

         void LoadAsynchronously(const std::string& filename);

         ///Save an animated entity definition file.
         bool Save(const std::string& filename, const Cal3DModelWrapper& wrapper);

         ///Get the model data associated with this model wrapper
         const Cal3DModelData* GetModelData(const Cal3DModelWrapper& wrapper) const;

         ///Get the model data associated with this model wrapper
         Cal3DModelData* GetModelData(const Cal3DModelWrapper& wrapper);

         ///Get the model data associated with this filename (used for asynch loading)
         Cal3DModelData* GetModelData(const std::string& filename);
         
         dtAnim::PoseMeshDatabase* GetPoseMeshDatabase(Cal3DModelWrapper& wrapper);
         
         void PurgeLoaderCaches();
         void TruncateDatabase();

         /// @return the node builder for this database.
         AnimNodeBuilder& GetNodeBuilder();

		 void ReleaseGLObjects();
         
      protected:
         Cal3DDatabase();
         virtual ~Cal3DDatabase();

         bool InternalLoad(const std::string& filename, dtCore::RefPtr<Cal3DModelData>& data_in);

         Cal3DModelData* Find(const std::string& filename);
         Cal3DModelData* Find(const CalCoreModel* coreModel);

         const Cal3DModelData* Find(const std::string& filename) const;
         const Cal3DModelData* Find(const CalCoreModel* coreModel) const;

         ModelDataArray mModelData;

         dtCore::RefPtr<Cal3DLoader> mFileLoader;
         dtCore::RefPtr<AnimNodeBuilder> mNodeBuilder;

         typedef dtCore::RefPtr<dtAnim::PoseMeshDatabase> PoseDatabase;
         typedef std::map<CalCoreModel*, PoseDatabase> PoseDatabaseMap;
         PoseDatabaseMap mPoseMeshMap;

         mutable OpenThreads::Mutex mAsynchronousLoadLock;
         mutable OpenThreads::Mutex mLoadingLock;

         static dtCore::RefPtr<Cal3DDatabase> mInstance;

         friend class LoadTask;
   };

}

#endif /*DELTA_CAL3D_DATABASE*/
