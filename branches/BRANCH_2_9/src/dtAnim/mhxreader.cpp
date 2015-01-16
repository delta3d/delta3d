
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/mhxreader.h>
#include <dtAnim/osgloader.h>
#include <dtCore/refptr.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <osgDB/FileNameUtils>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   static const std::string ATTR_BONE_HEAD("head");
   static const std::string ATTR_BONE_ROLL("roll");
   static const std::string ATTR_BONE_TAIL("tail");
   static const std::string ATTR_IMAGE_FILENAME("filename");
   static const std::string ATTR_MATERIAL_ALPHA("alpha");
   static const std::string ATTR_MATERIAL_DIFFUSE("diffuse_color");
   static const std::string ATTR_MATERIAL_SPECULAR("specular_color");
   static const std::string ATTR_MATERIAL_USE_TRANSPARENCY("use_transparency");
   static const std::string ATTR_MTEX_TEXTURE("texture");
   static const std::string ATTR_MTEX_USE_MAP_COLOR("use_map_color_diffuse");
   static const std::string ATTR_MTEX_USE_MAP_ALPHA("use_map_alpha");
   static const std::string ATTR_MTEX_USE_MAP_NORMAL("use_map_normal");
   static const std::string ATTR_MTEX_DIFFUSE_FACTOR("diffuse_color_factor");
   static const std::string ATTR_MTEX_ALPHA_FACTOR("alpha_factor");
   static const std::string ATTR_MTEX_NORMAL_FACTOR("normal_factor");
   static const std::string ATTR_TEXTURE_IMAGE_NAME("image");
   static const std::string ATTR_TEXTURE_USE_NORMAL_MAP("use_normal_map");

   static const std::string DELIMITER_END("End");

   static const std::string ELEMENT_ANIMATION("AnimationData");
   static const std::string ELEMENT_BONE("Bone");
   static const std::string ELEMENT_EDGES("Edges");
   static const std::string ELEMENT_FACES("Faces");
   static const std::string ELEMENT_IMAGE("Image");
   static const std::string ELEMENT_MATERIAL("Material");
   static const std::string ELEMENT_MATERIAL_TEXTURE("MTex");
   static const std::string ELEMENT_MESH("Mesh");
   static const std::string ELEMENT_MODIFIER("Modifier");
   static const std::string ELEMENT_OBJECT("Object");
   static const std::string ELEMENT_POSE("Pose");
   static const std::string ELEMENT_POSE_BONE("Posebone");
   static const std::string ELEMENT_SKELETON("Armature");
   static const std::string ELEMENT_TEXTURE("Armature");
   static const std::string ELEMENT_TEXTURE_LAYER("MeshTextureFaceLayer");
   static const std::string ELEMENT_VERTS("Verts");
   static const std::string ELEMENT_VERTEX_GROUP("VertexGroup");


   
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   typedef unsigned int VertIndex;

   /////////////////////////////////////////////////////////////////////////////
   struct EdgeInfo
   {
      EdgeInfo(VertIndex v1 = 0, VertIndex v2 = 0)
         : mV1(v1)
         , mV2(v2)
      {}

      VertIndex mV1;
      VertIndex mV2;
   };

   /////////////////////////////////////////////////////////////////////////////
   struct FaceInfo
   {
      FaceInfo(VertIndex v1 = 0, VertIndex v2 = 0, VertIndex v3 = 0, VertIndex v4 = 0)
         : mV1(v1)
         , mV2(v2)
         , mV3(v3)
         , mV4(v4)
      {}

      VertIndex mV1;
      VertIndex mV2;
      VertIndex mV3;
      VertIndex mV4;
   };

   /////////////////////////////////////////////////////////////////////////////
   struct MeshTextureFaceLayer
   {
      // TODO:
   };

   /////////////////////////////////////////////////////////////////////////////
   typedef std::vector<EdgeInfo> EdgeArray;
   typedef std::vector<FaceInfo> FaceArray;
   typedef std::vector<osg::Vec3> VertArray;

   /////////////////////////////////////////////////////////////////////////////
   struct BoneInfo
   {
      BoneInfo()
         : mUseConnect(false)
         , mLayers(0)
         , mRoll(0.0f)
      {}

      std::string mName;
      osg::Vec3 mHead;
      osg::Vec3 mTail;
      std::string mParentName;
      bool mUseConnect;
      int mLayers;
      float mRoll;
      /*
    head  -4.64803 -2.56735 8.74281  ;
    tail -4.68364 -2.70327 8.55378  ;
    parent Refer Bone f_middle.02.R ; 
    use_connect True ; 
    roll 1.56071 ; 
    use_deform False ; 
    show_wire False ; 
    use_inherit_scale False ; 
    layers Array 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0  ; 
    use_local_location True ; 
    lock False ; 
    use_envelope_multiply False ; 
    hide_select False ;*/
   };
   
   typedef std::map<std::string, BoneInfo> BoneMap;

   /////////////////////////////////////////////////////////////////////////////
   struct MaterialTextureInfo
   {
      MaterialTextureInfo()
         : mTextureUnit(0)
         , mUseMapColorDiffuse(true)
         , mUseMapAlpha(false)
         , mUseMapNormal(false)
         , mDiffuseFactor(1.0f)
         , mAlphaFactor(1.0f)
         , mNormalFactor(1.0f)
      {}

      std::string mName;
      std::string mType; // Ex: COLOR or NORMAL
      std::string mTextureName;
      int mTextureUnit;
      bool mUseMapColorDiffuse;
      bool mUseMapAlpha;
      bool mUseMapNormal;
      float mDiffuseFactor;
      float mAlphaFactor;
      float mNormalFactor;
   };

   typedef std::map<std::string, MaterialTextureInfo> MaterialTextureInfoMap;

   /////////////////////////////////////////////////////////////////////////////
   struct MaterialInfo
   {
      MaterialInfo()
         : mAlpha(1.0f)
         , mUseTransparency(false)
      {}

      std::string mName;
      osg::Vec3 mDiffuseColor;
      osg::Vec3 mSpecularColor;
      std::string mDiffuseShader;
      std::string mSpecularShader;
      MaterialTextureInfoMap mMatTexInfoMap;
      float mAlpha;
      bool mUseTransparency;
   };

   /////////////////////////////////////////////////////////////////////////////
   struct TextureInfo
   {
      TextureInfo()
         : mUseNormalMap(false)
      {}

      std::string mName;
      std::string mImageName;
      bool mUseNormalMap;
   };

   /////////////////////////////////////////////////////////////////////////////
   struct ImageInfo
   {
      ImageInfo()
         : mUsePremultiply(false)
      {}

      std::string mName;
      std::string mFileName;
      bool mUsePremultiply;
   };

   /////////////////////////////////////////////////////////////////////////////
   struct ModifierInfo
   {
      std::string mName;
      std::string mType; // Ex: MASK or ARMATURE
      std::string mMode; // Ex: VERTEX_GROUP
      std::string mVertexGroupName;
   };

   typedef std::map<std::string, ModifierInfo> ModifierMap;

   /////////////////////////////////////////////////////////////////////////////
   struct ObjectInfo
   {
      std::string mName;
      std::string mType; // Ex: MESH
      ModifierMap mModifierMap;
      osg::Vec3 mMhxOffset;
   };

   /////////////////////////////////////////////////////////////////////////////
   struct VertexGroupInfo
   {
      std::string mName;
   };

   typedef std::map<std::string, VertexGroupInfo> VertexGroupMap;

   /////////////////////////////////////////////////////////////////////////////
   struct MeshInfo
   {
      void AddVert(const osg::Vec3& vert)
      {
         mVerts.push_back(vert);
      }

      void AddEdge(VertIndex v1, VertIndex v2)
      {
         EdgeInfo edgeInfo(v1, v2);
         mEdges.push_back(edgeInfo);
      }

      std::string mName;
      VertArray mVerts;
      EdgeArray mEdges;
      FaceArray mFaces;
      VertexGroupMap mVertexGroupsMap;
   };
   
   /////////////////////////////////////////////////////////////////////////////
   struct SkeletonInfo
   {
      std::string mName;
      BoneMap mBoneMap;
   };
   
   
   
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   typedef std::list<std::string> StrList;
   typedef std::vector<std::string> StrArray;

   class Tokenizer
   {
   public:

      void SetData(std::istream& fin)
      {
         while (fin.good())
         {
            std::string str;
            std::getline(fin, str);
            mLines.push_back(str);
         }
      }

      bool IsEmpty() const
      {
         return mLines.empty() && mCurLineTokens.empty();
      }

      void SkipLine()
      {
         if ( ! mCurLineTokens.empty())
         {
            mCurLineTokens.clear();
         }
      }

      std::string GetToken()
      {
         std::string token;
         
         if (mCurLineTokens.empty() && ! mLines.empty())
         {
            StrArray tokens;
            GetLineTokens(tokens);

            if (tokens.empty())
            {
               mCurLineTokens.insert(mCurLineTokens.end(), tokens.begin(), tokens.end());
            }
         }

         if ( ! mCurLineTokens.empty())
         {
            token = mCurLineTokens.front();
            mCurLineTokens.pop_front();
         }

         return token;
      }

      int GetLineTokenCount() const
      {
         return (int)(mCurLineTokens.size());
      }

      bool GetLine(std::string& outLine)
      {
         bool success = false;

         if ( ! mLines.empty())
         {
            outLine = mLines.front();
            mLines.pop_front();
         }

         return success;
      }

      bool GetTokens(const std::string& str, StrArray& outTokens)
      {
         dtUtil::StringTokenizer<dtUtil::IsSpace>::tokenize(outTokens, str);
         return ! outTokens.empty();
      }

      bool GetLineTokens(StrArray& outTokens)
      {
         bool success = false;

         if ( ! mCurLineTokens.empty())
         {
            outTokens.insert(outTokens.end(), mCurLineTokens.begin(), mCurLineTokens.end());
            mCurLineTokens.clear();

            success = true;
         }
         else if ( ! mLines.empty())
         {
            const std::string& line = mLines.front();

            GetTokens(line, outTokens);

            mLines.pop_front();
            success = true;
         }

         return success;
      }

   protected:
      StrList mLines;
      StrList mCurLineTokens;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class MHXData : public osg::Referenced
   {
   protected:
      Tokenizer mTokenizer;

   public:

      MHXData()
      {}

      bool IsLineValid(const std::string& line) const
      {
         // TODO:
         return true;
      }

      bool GetLine(StrArray& outTokens)
      {
         bool success = false;

         outTokens.clear();

         std::string line;

         if (mTokenizer.GetLine(line) && IsLineValid(line))
         {
            success = mTokenizer.GetTokens(line, outTokens);
         }

         return success;
      }

      void SetData(std::istream& fin)
      {
         try
         {
            mTokenizer.SetData(fin);
         }
         catch (std::exception& ex)
         {
            std::string msg(ex.what());
            LOG_ERROR("Failed reading MHX data stream." + msg);
         }
         catch (...)
         {
            LOG_ERROR("Failed reading MHX data stream: Unknown exception");
         }
      }

      bool ReadData()
      {
         bool success = true;

         try
         {
            std::string element;
            do
            {
               element = mTokenizer.GetToken();
         
               // Gather any other tokens following the element.
               StrArray objectTokens;
               if (mTokenizer.GetLineTokenCount() > 0)
               {
                  GetLine(objectTokens);
               }
               
               if (element == ELEMENT_SKELETON)
               { ReadSkeleton(objectTokens); }
               else if (element == ELEMENT_IMAGE)
               { ReadImage(objectTokens); }
               else if (element == ELEMENT_MATERIAL)
               { ReadMaterial(objectTokens); }
               else if (element == ELEMENT_MESH)
               { ReadMesh(objectTokens); }
               else if (element == ELEMENT_OBJECT)
               { ReadObject(objectTokens); }
               else if (element == ELEMENT_POSE)
               { ReadPose(objectTokens); }
               else if (element == ELEMENT_TEXTURE)
               { ReadTexture(objectTokens); }
               else
               {
                  mTokenizer.SkipLine();
               }

            } while ( ! element.empty());
         }
         catch (std::exception& ex)
         {
            success = false;
            std::string msg(ex.what());
            LOG_ERROR("Failed reading MHX data stream." + msg);
         }
         catch (...)
         {
            success = false;
            LOG_ERROR("Failed reading MHX data stream: Unknown exception");
         }

         return success;
      }

      bool ReadAnimation()
      {
         // TODO:
         return false;
      }
      
      bool ReadBone(StrArray& objectTokens, BoneInfo& outInfo)
      {
         bool success = false;

         if ( ! objectTokens.empty())
         {
            outInfo.mName = objectTokens.front();
         }
         
         StrArray tokens;
         while (GetLine(tokens) && tokens.front() != DELIMITER_END)
         {
            std::string element = tokens.front();

            if (element == ATTR_BONE_HEAD)
            {
               // Remove the attribute name.
               tokens.erase(tokens.begin());

               osg::Vec3 point;
               if (ReadVec3(tokens, point))
               {
                  outInfo.mHead = point;
               }
            }
            else if (element == ATTR_BONE_TAIL)
            {
               // Remove the attribute name.
               tokens.erase(tokens.begin());

               osg::Vec3 point;
               if (ReadVec3(tokens, point))
               {
                  outInfo.mTail = point;
               }
            }
            else if (element == ATTR_BONE_ROLL)
            {
               if (tokens.size() >= 2)
               {
                  outInfo.mRoll = atof(tokens[1].c_str());
               }
            }

            success = true;
         }

         return success;
      }
      
      bool ReadEdge(const StrArray& tokens, EdgeInfo& outEdge)
      {
         bool success = false;

         if (tokens.size() >= 2)
         {
            outEdge.mV1 = atoi(tokens[0].c_str());
            outEdge.mV1 = atoi(tokens[1].c_str());

            success = true;
         }

         return success;
      }
      
      bool ReadFaces(const StrArray& tokens, FaceInfo& outFace)
      {
         bool success = false;

         if (tokens.size() >= 4)
         {
            outFace.mV1 = ReadInt(tokens[0]);
            outFace.mV2 = ReadInt(tokens[1]);
            outFace.mV3 = ReadInt(tokens[2]);
            outFace.mV4 = ReadInt(tokens[3]);

            success = true;
         }

         return success;
      }

      bool ReadImage(StrArray& objectTokens)
      {
         bool success = false;

         ImageInfo info;
         if ( ! objectTokens.empty())
         {
            info.mName = objectTokens.front();
         }
         
         StrArray tokens;
         while (GetLine(tokens) && tokens.front() != DELIMITER_END)
         {
            std::string element = tokens.front();

            if (element == ATTR_IMAGE_FILENAME)
            {
               info.mFileName = tokens[1];
            }

            success = true;
         }

         return success;
      }
      
      bool ReadMaterial(StrArray& objectTokens)
      {
         bool success = false;

         MaterialInfo info;
         if ( ! objectTokens.empty())
         {
            info.mName = objectTokens.front();
         }
         
         StrArray tokens;
         while (GetLine(tokens) && tokens.front() != DELIMITER_END)
         {
            std::string element = tokens.front(); 

            if (element == ATTR_MATERIAL_ALPHA)
            {
               info.mAlpha = ReadFloat(tokens[1]);
            }
            else if (element == ATTR_MATERIAL_DIFFUSE)
            {
               // Remove the property name
               // and the subsequent "Array" token.
               RemoveTokens(tokens, 2);

               osg::Vec3 color(1.0f, 1.0f, 1.0f);
               if (ReadVec3(tokens, color))
               {
                  info.mDiffuseColor = color;
               }
            }
            else if (element == ATTR_MATERIAL_SPECULAR)
            {
               // Remove the property name
               // and the subsequent "Array" token.
               RemoveTokens(tokens, 2);

               osg::Vec3 color(1.0f, 1.0f, 1.0f);
               if (ReadVec3(tokens, color))
               {
                  info.mSpecularColor = color;
               }
            }
            else if (element == ATTR_MATERIAL_USE_TRANSPARENCY)
            {
               info.mUseTransparency = ReadBool(tokens[1]);
            }
            else if (element == ELEMENT_MATERIAL_TEXTURE)
            {
               // Remove the element name "MTex"
               RemoveTokens(tokens, 1);

               MaterialTextureInfo matTexInfo;
               if (ReadTextureLayer(tokens, matTexInfo))
               {
                  info.mMatTexInfoMap.insert(std::make_pair(matTexInfo.mName, matTexInfo));
               }
            }

            success = true;
         }

         return success;
      }

      bool ReadMesh(StrArray& objectTokens)
      {
         bool success = false;

         MeshInfo info;
         if ( ! objectTokens.empty())
         {
            info.mName = objectTokens.front();
         }
         
         StrArray tokens;
         while (GetLine(tokens) && tokens.front() != DELIMITER_END)
         {
            std::string element = tokens.front(); 

            if (element == ELEMENT_VERTS)
            {
               osg::Vec3 vert;

               while (GetLine(tokens) && tokens.front() != DELIMITER_END)
               {
                  // Remove the preceeding "v"
                  RemoveTokens(tokens, 1);
                  
                  if (ReadVec3(tokens, vert))
                  {
                     info.mVerts.push_back(vert);
                  }
               }
            }
            else if (element == ELEMENT_EDGES)
            {
               EdgeInfo edge;

               while (GetLine(tokens) && tokens.front() != DELIMITER_END)
               {
                  // Remove the preceeding "e"
                  RemoveTokens(tokens, 1);

                  if (ReadEdge(tokens, edge))
                  {
                     info.mEdges.push_back(edge);
                  }
               }
            }

            success = true;
         }

         return success;
      }
      
      bool ReadObject(StrArray& objectTokens)
      {
         bool success = false;

         ObjectInfo info;
         if ( ! objectTokens.empty())
         {
            info.mName = objectTokens.front();
         }
         
         StrArray tokens;
         while (GetLine(tokens) && tokens.front() != DELIMITER_END)
         {
         // TODO:
         }

         return success;
      }
      
      bool ReadPose(StrArray& objectTokens)
      {
         // TODO:
         return false;
      }
      
      bool ReadPoseBone()
      {
         // TODO:
         return false;
      }
      
      bool ReadSkeleton(StrArray& objectTokens)
      {
         bool success = false;

         SkeletonInfo info;
         if ( ! objectTokens.empty())
         {
            info.mName = objectTokens.front();
         }
         
         StrArray tokens;
         while (GetLine(tokens) && tokens.front() != DELIMITER_END)
         {
            std::string element = tokens.front();

            if (element == ELEMENT_BONE)
            {
               BoneInfo boneInfo;

               // Remove the element name.
               RemoveTokens(tokens, 1);

               if (ReadBone(tokens, boneInfo))
               {
                  info.mBoneMap.insert(std::make_pair(boneInfo.mName, boneInfo));
               }
            }

            success = true;
         }

         return success;
      }
      
      bool ReadTexture(StrArray& objectTokens)
      {
         bool success = false;

         TextureInfo info;
         if ( ! objectTokens.empty())
         {
            info.mName = objectTokens.front();
         }
         
         StrArray tokens;
         while (GetLine(tokens) && tokens.front() != DELIMITER_END)
         {
            std::string element = tokens.front();

            if (element == ATTR_TEXTURE_IMAGE_NAME)
            {
               info.mImageName = tokens[1];
            }
            else if (element == ATTR_TEXTURE_USE_NORMAL_MAP)
            {
               info.mUseNormalMap = ReadBool(tokens[1]);
            }

            success = true;
         }

         return success;
      }
      
      bool ReadTextureLayer(const StrArray& objectTokens, MaterialTextureInfo& outInfo)
      {
         bool success = false;

         if (objectTokens.size() >= 4)
         {
            outInfo.mTextureUnit = ReadInt(objectTokens[0]);
            outInfo.mName = objectTokens[1];
            outInfo.mType = objectTokens[3];
         }
         
         StrArray tokens;
         while (GetLine(tokens) && tokens.front() != DELIMITER_END)
         {
            std::string element = tokens.front();

            if (element == ATTR_MTEX_TEXTURE)
            {
               outInfo.mTextureName = tokens[3];
            }
            else if (element == ATTR_MTEX_USE_MAP_COLOR)
            {
               outInfo.mUseMapColorDiffuse = ReadBool(tokens[1]);
            }
            else if (element == ATTR_MTEX_USE_MAP_ALPHA)
            {
               outInfo.mUseMapAlpha = ReadBool(tokens[1]);
            }
            else if (element == ATTR_MTEX_USE_MAP_NORMAL)
            {
               outInfo.mUseMapNormal = ReadBool(tokens[1]);
            }
            else if (element == ATTR_MTEX_DIFFUSE_FACTOR)
            {
               outInfo.mDiffuseFactor = ReadFloat(tokens[1]);
            }
            else if (element == ATTR_MTEX_ALPHA_FACTOR)
            {
               outInfo.mAlphaFactor = ReadFloat(tokens[1]);
            }
            else if (element == ATTR_MTEX_NORMAL_FACTOR)
            {
               outInfo.mNormalFactor = ReadFloat(tokens[1]);
            }

            success = true;
         }

         return success;
      }

      int ReadLayerFlags()
      {
         int layerFlags = 0;

         // TODO:

         return layerFlags;
      }

      bool ReadVec3(const StrArray& tokens, osg::Vec3& outVec)
      {
         bool success = false;

         if (tokens.size() >= 3)
         {
            outVec.x() = ReadFloat(tokens[0]);
            outVec.y() = ReadFloat(tokens[1]);
            outVec.z() = ReadFloat(tokens[2]);

            success = true;
         }

         return success;
      }

      bool ReadBool(const std::string& value)
      {
         return 0 == stricmp(value.c_str(), "true");
      }

      int ReadInt(const std::string& value)
      {
         return atoi(value.c_str());
      }

      float ReadFloat(const std::string& value)
      {
         return atof(value.c_str());
      }

      int RemoveTokens(StrArray& tokens, int numTokens)
      {
         int numRemoved = 0;

         for (int i = 0; i < numTokens && ! tokens.empty(); ++i)
         {
            tokens.erase(tokens.begin());
            ++numRemoved;
         }

         return numRemoved;
      }

   protected:
      virtual ~MHXData()
      {}
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   MHXReader::MHXReader()
   {
      supportsExtension(dtAnim::OsgExtensionEnum::MAKE_HUMAN_EXCHANGE.GetName(),
         dtAnim::OsgExtensionEnum::MAKE_HUMAN_EXCHANGE.GetDescription());
   }

   MHXReader::~MHXReader()
   {}

   const char* MHXReader::className() const
   {
      return "MHXReader";
   }

   osgDB::ReaderWriter::ReadResult MHXReader::readObject(
      const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
   {
      std::string ext = osgDB::getLowerCaseFileExtension(fileName);

      if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

      if (!dtUtil::FileUtils::GetInstance().FileExists(fileName))
      {
         return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND);
      }

      osgDB::ifstream istream(fileName.c_str(), std::ios::in);

      if (!istream.is_open())
      {
         return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
      }

      osgDB::ReaderWriter::ReadResult result = readObject(istream, options);
      if (istream.is_open())
      {
         istream.close();
      }

      return result;
   }

   osgDB::ReaderWriter::ReadResult MHXReader::readObject(
      std::istream& fin, const osgDB::ReaderWriter::Options* options ) const
   {
      dtCore::RefPtr<MHXData> mhxData = new MHXData;
      mhxData->SetData(fin);
      mhxData->ReadData();

      /*

      return geom.valid()
         ? osgDB::ReaderWriter::ReadResult(geom.get(), ReaderWriter::ReadResult::FILE_LOADED)
         : osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;*/
      
      // TODO:

      return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
   }

   // Register this ReaderWriter with the OSG plugin registry.
   REGISTER_OSGPLUGIN(mhx, MHXReader)

}