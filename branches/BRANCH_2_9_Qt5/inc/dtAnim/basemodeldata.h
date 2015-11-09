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
 * Bradley Anderegg 07/17/2007
 */
#ifndef __DELTA_BASE_MODEL_DATA_H__
#define __DELTA_BASE_MODEL_DATA_H__

#include <dtAnim/export.h>
#include <dtCore/refptr.h>

#include <osg/Array>
#include <osg/Referenced>
#include <osg/observer_ptr>
#include <dtUtil/getsetmacros.h>
#include <dtCore/resourcedescriptor.h>

#include <vector>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class VertexBufferObject;
   class ElementBufferObject;
}
/// @endcond

namespace dtAnim
{
   class Animatable;

   // we will hold a vector of animatables for each CalCoreModel
   typedef std::vector<dtCore::RefPtr<dtAnim::Animatable> > AnimatableArray;
   typedef std::vector<std::string> StrArray;

   enum ModelResourceType
   {
      NO_FILE,
      SKEL_FILE, // Skeleton
      MAT_FILE,  // Material
      MESH_FILE, // Mesh
      ANIM_FILE, // Animation
      MORPH_FILE, // Morph Animation
      MIXED_FILE // Mixed
   };

   /**
    * A simple data class that stores the configuration options for level of detail.
    */
   class DT_ANIM_EXPORT LODOptions
   {
   public:
      LODOptions();

      ///@return the distance at which to start decreasing the level of detail.
      double GetStartDistance() const { return mStartDistance; }
      void SetStartDistance(double newDistance);

      ///@return the distance at which the level of detail should be the minimum.
      double GetEndDistance() const { return mEndDistance; }
      void SetEndDistance(double newDistance);

      ///@return the maximum distance that the model will be drawn at all.
      double GetMaxVisibleDistance() const { return mMaxVisibleDistance; }
      void SetMaxVisibleDistance(double newDistance);

   private:
      double mStartDistance, mEndDistance, mMaxVisibleDistance;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT BaseModelData : public osg::Referenced
   {
   public:
      BaseModelData(const std::string& modelName, const dtCore::ResourceDescriptor& rd,
         const std::string& characterSystemType);

      static std::string GetResourceTypeName(dtAnim::ModelResourceType resourceType);

      const std::string& GetCharacterSystemType() const;

      /// Add a higher level animatable such as a sequence or an animation channel.
      void Add(Animatable& animatable);

      void Remove(Animatable& animatable);

      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, Resource);

      DT_DECLARE_ACCESSOR(std::string, ModelName);

      DT_DECLARE_ACCESSOR(float, Scale);

      DT_DECLARE_ACCESSOR(std::string, ShaderGroupName);
      DT_DECLARE_ACCESSOR(std::string, ShaderName);
      DT_DECLARE_ACCESSOR(std::string, PoseMeshFilename);
      /// Sets the maximum number of bones the shader supports
      DT_DECLARE_ACCESSOR(unsigned, ShaderMaxBones);


      Animatable* GetAnimatableByName(const std::string& name);
      const Animatable* GetAnimatableByName(const std::string& name) const;

      AnimatableArray& GetAnimatables();
      const AnimatableArray& GetAnimatables() const;

      virtual float GetAnimationDuration(const std::string& name) const = 0;

      /**
       * @return the vertex array that stores the raw vertex data
       */
      osg::FloatArray* GetSourceVertexArray() { return mVertexArray; }

      /**
       * @return the vertex array that stores the raw index data
       */
      osg::IntArray* GetSourceIndexArray() { return mIndexArray; }

      /**
       * @return the number of elements between each successive source vertex row
       */
      int GetStride() { return mStride; }

      /// Allocate memory to be used for the source arrays
      void CreateSourceArrays(int numberOfVertices, int numberOfIndices, int stride);

      /// Deletes the memory held by the vertex and index source arrays
      void DestroySourceArrays();

      /**
       * @return the vbo being used with this character core model, or 0 for none.
       */
      osg::VertexBufferObject* GetVertexBufferObject();

      /// Sets the Vertex Buffer Object being used with this character core model
      void SetVertexBufferObject(osg::VertexBufferObject* vbo);

      /**
       * @return the ebo being used with this character core model, or 0 for none
       */
      osg::ElementBufferObject* GetElementBufferObject() const;

      /// Sets the Element Buffer Object being used with this character core model
      void SetElementBufferObject(osg::ElementBufferObject* ebo);

      /**
       * @return the DrawElements objects used with the ElementBufferObject or 0 for none
       */
      osg::DrawElements* GetDrawElements() const;

      /// Sets the DrawElements to use with the ElementBufferObject
      void SetDrawElements(osg::DrawElements* drawElements);


      LODOptions& GetLODOptions() { return mLODOptions; }
      const LODOptions& GetLODOptions() const { return mLODOptions; }

      virtual int LoadResource(dtAnim::ModelResourceType resourceType,
         const std::string& file, const std::string& objectName) = 0;

      virtual int UnloadResource(dtAnim::ModelResourceType resourceType, const std::string& objectName) = 0;

      /**
       * Convenience method for determining the file type from the specified file.
       */
      virtual ModelResourceType GetFileType(const std::string& file) const = 0;

      /**
       * Determine the number of files registered under the specified type.
       */
      unsigned GetFileCount(ModelResourceType fileType) const;

      /**
       * Method for obtaining all associated files of a specified type.
       * @param fileType The type of files to search for.
       * @param outFiles List to capture all files matching the specified file type.
       * @return Number of results.
       */
      unsigned GetFileListForFileType(ModelResourceType fileType, StrArray& outFiles) const;

      /**
       * Convenience method for obtaining all object names for the specified file type.
       * @param fileType The type of files to search for.
       * @param outNames List to capture all object names mapped to the specified file type.
       * @return Number of results.
       */
      unsigned GetObjectNameListForFileType(ModelResourceType fileType, StrArray& outNames) const;

      /**
       * Convenience method for returning object names in the order that they were registered on load.
       * This method is important for writing model data to file.
       * @param fileType The type of files the names relate to.
       * @param outNames List to capture all object names mapped to the specified file type.
       * @return Number of results.
       */
      unsigned GetObjectNameListForFileTypeSorted(ModelResourceType fileType, StrArray& outNames) const;

      /**
       * Method for changing an object name.
       * @param fileType The type of files the name relates to.
       * @param oldObjectName Name that may be mapped to a file.
       * @param newObjectName Name that is to replace the old name.
       * @return TRUE if the old object name was found and the new name set.
       */
      bool ReplaceObjectName(ModelResourceType fileType, const std::string& oldObjectName,
         const std::string& newObjectName);

      /**
       * Method for changing the file that an object name is mapped to.
       * @param fileType The type of files the name relates to.
       * @param objectName Name that may be mapped to a file.
       * @param file File to register for the specified object name.
       * @return TRUE if the object name was found and the file name set.
       */
      bool SetFileForObjectName(ModelResourceType fileType, const std::string& objectName,
         const std::string& file);

      /**
       * Method for determining the file that an object name is mapped to.
       * @param fileType The type of files the name relates to.
       * @param objectName Name that may be mapped to a file.
       * @return File under which the object name is mapped; empty string otherwise.
       */
      std::string GetFileForObjectName(ModelResourceType fileType, const std::string& objectName) const;

      /**
       * Method for obtaining all object names referencing the specified file.
       * @param file The file that may be mapped to multiple object names.
       * @param outObjectNames List to capture names found for the specified file.
       * @return Number of results.
       */
      unsigned GetObjectNameListForFile(const std::string& file, StrArray& outObjectNames) const;
   
      /**
       * Register a file a file with an object name. A file may be registered with multiple names.
       * @param file The file to be registered to an object name.
       * @param objectName Name of an object that may represent the file, such as an animation, mesh, etc.
       * @param fileType Type of resource the file contains.
       * @return TRUE if the file and object name were successfully registered.
       */
      bool RegisterFile(const std::string& file, const std::string& objectName, ModelResourceType fileType);

      /**
       * Register a file a file with an object name. A file may be registered with multiple names.
       * @param file The file to be registered to an object name.
       * @param objectName Name of an object that may represent the file, such as an animation, mesh, etc.
       * @return TRUE if the file and object name were successfully registered.
       */
      bool RegisterFile(const std::string& file, const std::string& objectName);

      /**
       * Method for removing file-to-object-name mappings by a specific file name.
       * @param file File mapping to be removed.
       * @param outObjectNames List to capture of all object names that were mapped to the specified file.
       * @return Number of matches that were removed.
       */
      unsigned UnregisterFile(const std::string& file, StrArray* outObjectNames);

      /**
       * Method for removing file-to-object-name mappings by a specific object name.
       * @param objectName The name to be removed from the mapping
       * @param file Name of the file to match (exact), file type extension (search within certain type),
       *        or empty string (search all).
       * @param outFileNames List to capture of all files that the object name was mapped to.
       * @return Number of matches that were removed.
       */
      unsigned UnregisterObjectName(const std::string& objectName, dtAnim::ModelResourceType fileType,
         StrArray* outFileNames = NULL);

      /**
       * Method for removing file-to-object-name mappings by a specific object name.
       * @param objectName The name to be removed from the mapping
       * @param file Name of the file to match (exact), file type extension (search within certain type),
       *        or empty string (search all).
       * @param outFileNames List to capture of all files that the object name was mapped to.
       * @return Number of matches that were removed.
       */
      unsigned UnregisterObjectName(const std::string& objectName, const std::string& file = "",
         StrArray* outFileNames = NULL);

      virtual int GetIndexForObjectName(ModelResourceType fileType, const std::string& objectName) const = 0;

   protected:
      virtual ~BaseModelData();

      BaseModelData(const BaseModelData&); //not implemented
      BaseModelData& operator=(const BaseModelData&); //not implemented

   private:
      std::string mCharacterSystemType;

      // Buffer data
      int mStride;
      osg::ref_ptr<osg::IntArray> mIndexArray;
      osg::ref_ptr<osg::FloatArray> mVertexArray;
      osg::observer_ptr<osg::VertexBufferObject> mVertexBufferObject;
      osg::observer_ptr<osg::ElementBufferObject> mElementBufferObject;
      osg::ref_ptr<osg::DrawElements> mElementBufferDrawElements;

      AnimatableArray mAnimatables;

      LODOptions mLODOptions;

      // File Mapping
      class ObjectNameAndFileType : public osg::Referenced
      {
      public:
         ObjectNameAndFileType(const std::string& objectName = "", ModelResourceType fileType = NO_FILE)
            : mType(fileType)
            , mName(objectName)
         {}

         ModelResourceType mType; // For quick reference to avoid several string compares.
         std::string mName;

      protected:
         virtual ~ObjectNameAndFileType() {}
      };

      // A file can be mapped to multiple named objects, such as multiple animation channels
      typedef std::multimap<std::string, dtCore::RefPtr<ObjectNameAndFileType> > FileToObjectMap;
      FileToObjectMap mFileObjectMap;
   };

} // namespace dtAnim

#endif /*DELTA_CAL3D_MODEL_DATA*/
