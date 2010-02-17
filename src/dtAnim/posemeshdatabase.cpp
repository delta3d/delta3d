/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 MOVES Institute
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
 * Michael Guerrero
 */

#include <dtAnim/posemeshdatabase.h>
#include <dtAnim/posemeshloader.h>
#include <dtAnim/posemesh.h>
#include <dtAnim/posemath.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>

#include <algorithm>
#include <cstddef>
#include <istream>

using namespace dtAnim;

// Functor to easily build a list of meshes
template<typename ContainerT>
struct PoseBuilderFunctor
{
   PoseBuilderFunctor(dtAnim::Cal3DModelWrapper* model, ContainerT* container)
      : mModel(model)
      , mContainer(container)
   {}

   void operator ()(const PoseMeshData& meshData)
   {
      PoseMesh* newMesh = new PoseMesh(mModel, meshData);
      mContainer->push_back(newMesh);
   }

private:
   PoseBuilderFunctor();

   dtAnim::Cal3DModelWrapper* mModel;
   ContainerT* mContainer;
};

struct PredicatePoseMeshName
{
   std::string mName;
   PredicatePoseMeshName(const std::string& name): mName(name) {}
   PredicatePoseMeshName(const PredicatePoseMeshName& same): mName(same.mName) {}

   bool operator ()(const PoseMesh* mesh) const
   {
      return(mesh->GetName() == mName);
   }

private:
   PredicatePoseMeshName();
};

template<typename PtrT>
struct DeletePointer
{
   void operator ()(PtrT ptr)
   {
      delete ptr;
   }
};

////////////////////////////////////////////////////////////////////////////////
PoseMeshDatabase::PoseMeshDatabase(dtAnim::Cal3DModelWrapper* model)
   : mMeshes()
   , mModel(model)
{
}

////////////////////////////////////////////////////////////////////////////////
PoseMeshDatabase::~PoseMeshDatabase()
{
   std::for_each(mMeshes.begin(),
                 mMeshes.end(),
                 DeletePointer<PoseMeshList::value_type>());

   mMeshes.clear();
}

////////////////////////////////////////////////////////////////////////////////
PoseMesh* PoseMeshDatabase::GetPoseMeshByName(const std::string& name)
{
   PoseMeshList::iterator iter = std::find_if(mMeshes.begin(),
                                              mMeshes.end(),
                                              PredicatePoseMeshName(name));
   if (iter == mMeshes.end())
   {
      return NULL;
   }

   return *iter;
}

////////////////////////////////////////////////////////////////////////////////
bool PoseMeshDatabase::LoadFromFile(const std::string& file)
{
   PoseMeshLoader meshLoader;
   std::vector<PoseMeshData> meshDataVector;
   bool result = meshLoader.Load(file, meshDataVector);
   assert(result);

   mMeshes.reserve(mMeshes.size() + meshDataVector.size());


   try
   {
      std::for_each(meshDataVector.begin(),
                    meshDataVector.end(),
                    PoseBuilderFunctor<PoseMeshList>(mModel.get(), &mMeshes));
   }
   catch (dtUtil::Exception& exception)
   {
      LOG_ERROR(exception.ToString());

      // Mesh is invalid, clear its data
      std::for_each(mMeshes.begin(),
                    mMeshes.end(),
                    DeletePointer<PoseMeshList::value_type>());

      mMeshes.clear();

      result = false;
   }

   return result;
}





