
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELAT3D
#include <dtAnim/osgnodebuilder.h>
#include <dtAnim/charactershaderbuilder.h>
#include <dtAnim/constants.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/log.h>
// OSG
#include <osg/Geode>
#include <osgAnimation/BoneMapVisitor>
#include <osgAnimation/RigTransformHardware>
#include <osgAnimation/RigTransformSoftware>
#include <osg/Shader>
#include <sstream>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // HELPER CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   osg::ref_ptr<osg::Program> program;
   // Override the default RigTransformHardware for customized usage
   struct OsgRigTransformHardware : public osgAnimation::RigTransformHardware
   {
      typedef osgAnimation::RigTransformHardware BaseClass;

      OsgRigTransformHardware(dtAnim::OsgModelWrapper& modelWrapper, bool boneModeQuat)
         : BaseClass()
         , mBoneModeQuat(boneModeQuat)
         , mScale(new osg::Uniform(osg::Uniform::FLOAT, "scale", 1))
         , mModelWrapper(&modelWrapper)
      {
         _bonesPerVertex = Constants::DEFAULT_BONES_PER_VERTEX;

         mScale->setDataVariance(osg::Object::DYNAMIC);
      }

       void operator()(osgAnimation::RigGeometry& geom)
       {
           if (_needInit)
           {
               if (!init(geom))
               {
                   return;
               }
           }

           computeMatrixPaletteUniform(geom.getMatrixFromSkeletonToGeometry(), geom.getInvMatrixFromSkeletonToGeometry());
       }

       bool init(osgAnimation::RigGeometry& geom)
       {
           osg::Vec3Array* pos = dynamic_cast<osg::Vec3Array*>(geom.getVertexArray());
           if (!pos) {
               osg::notify(osg::WARN) << "RigTransformHardware no vertex array in the geometry " << geom.getName() << std::endl;
               return false;
           }

           if (!geom.getSkeleton()) {
               osg::notify(osg::WARN) << "RigTransformHardware no skeleton set in geometry " << geom.getName() << std::endl;
               return false;
           }

           osgAnimation::BoneMapVisitor mapVisitor;
           geom.getSkeleton()->accept(mapVisitor);
           osgAnimation::BoneMap bm = mapVisitor.getBoneMap();

           if (!createPalette(pos->size(),bm, geom.getVertexInfluenceSet().getVertexToBoneList()))
               return false;

           bool success = false;
           
           std::string shaderFile("shaders/HardwareCharacterOSGMatrix.vert");
           if (mBoneModeQuat)
           {
              shaderFile = "shaders/HardwareCharacterOSG.vert";

              _uniformMatrixPalette = new osg::Uniform(osg::Uniform::FLOAT_VEC4,
                 dtAnim::CharacterShaderBuilder::BONE_TRANSFORM_UNIFORM, _bonePalette.size() * 3);
              
              _uniformMatrixPalette->setDataVariance(osg::Object::DYNAMIC);
    
              _boneWeightAttribArrays = createVertexAttribList();

              success = initDefaultShader(geom, shaderFile);
           }
           else // Default Matrix Bone Mode
           {
              _uniformMatrixPalette = new osg::Uniform(osg::Uniform::FLOAT_MAT4,
                 dtAnim::CharacterShaderBuilder::BONE_TRANSFORM_UNIFORM, _bonePalette.size());

              success = initDefaultShader(geom, shaderFile);
           }

           _needInit = false;
           return true;
       }

       bool initDefaultShader(osgAnimation::RigGeometry& geom, const std::string shaderFile)
       {
           int attribIndex = 11;
           int nbAttribs = Constants::DEFAULT_BONES_PER_VERTEX;//getNumVertexAttrib();

           // use a global program for all avatar
           if (!program.valid())
           {
               program = new osg::Program;
               program->setName("HardwareSkinning");
               if (!_shader.valid())
               {
                  std::string path = dtUtil::FindFileInPathList(shaderFile);
                  if (!path.empty())
                  {
                     _shader = new osg::Shader(osg::Shader::VERTEX);
                     if (!_shader->loadShaderSourceFromFile(path))
                     {
                        LOG_ERROR("Error loading vertex shader file: " + shaderFile);
                     }
                     _shader->setName(shaderFile);
                     program->addShader(_shader.get());
                  }
               }

               if (!_shader.valid()) {
                   osg::notify(osg::WARN) << "RigTransformHardware can't load VertexShader" << std::endl;
                   return false;
               }

               // replace max matrix by the value from uniform
               {
                   std::string str = _shader->getShaderSource();
                   std::string toreplace = std::string("MAX_BONES");
                   std::size_t start = str.find(toreplace);
                   std::stringstream ss;
                   ss << _uniformMatrixPalette->getNumElements();
                   str.replace(start, toreplace.size(), ss.str());
                   _shader->setShaderSource(str);
                   osg::notify(osg::INFO) << "Shader " << str << std::endl;
               }

               program->addShader(_shader.get());

               for (int i = 0; i < nbAttribs; i++)
               {
                   std::stringstream ss;
                   ss << "boneWeight" << i;
                   program->addBindAttribLocation(ss.str(), attribIndex + i);

                   osg::notify(osg::INFO) << "set vertex attrib " << ss.str() << std::endl;
               }
           } 
           
           for (int i = 0; i < nbAttribs; i++)
           {
               geom.setVertexAttribArray(attribIndex + i, getVertexAttrib(i));
           }

           osg::StateSet* ss = geom.getOrCreateStateSet();
           ss->addUniform(_uniformMatrixPalette);
           ss->addUniform(mScale);
           ss->setAttributeAndModes(program.get());
           ss->setDataVariance(osg::Object::DYNAMIC);
           geom.setStateSet(ss);

           return true;
       }

      //
      // create vertex attribute by 2 bones
      // vec4(boneIndex0, weight0, boneIndex1, weight1)
      // if more bones are needed then other attributes are created
      // vec4(boneIndex2, weight2, boneIndex3, weight3)
      // the idea is to use this format to have a granularity smaller
      // than the 4 bones using two vertex attributes
      //
      RigTransformHardware::BoneWeightAttribList createVertexAttribList()
      {
          BoneWeightAttribList arrayList;

          typedef std::vector<IndexWeightEntry> IndexWeightEntryArray;
          IndexWeightEntryArray* curEntryArray = NULL;
          
          int nbVertexes = getNumVertexes();
          int numBonesPerVertex = Constants::DEFAULT_BONES_PER_VERTEX;

          arrayList.resize(numBonesPerVertex);
          for (int i = 0; i < numBonesPerVertex; i++)
          {
              osg::ref_ptr<osg::Vec4Array> array = new osg::Vec4Array(osg::Array::BIND_PER_VERTEX);
              arrayList[i] = array;
              array->resize(nbVertexes);
              for (int j = 0; j < nbVertexes; j++)
              {
                  for (int b = 0; b < 2; b++)
                  {
                      // the granularity is 2 so if we have only one bone
                      // it's convenient to init the second with a weight 0
                      int boneIndexInList = i*2 + b;
                      int boneIndexInVec4 = b*2;
                      (*array)[j][0 + boneIndexInVec4] = 0;
                      (*array)[j][1 + boneIndexInVec4] = 0;
                      if (boneIndexInList < numBonesPerVertex)
                      {
                         curEntryArray = &_vertexIndexMatrixWeightList[j];
                         if ((int)curEntryArray->size() < numBonesPerVertex)
                         {
                            curEntryArray->resize(numBonesPerVertex);
                         }

                         float boneIndex = static_cast<float>((*curEntryArray)[boneIndexInList].getIndex());
                         float boneWeight = (*curEntryArray)[boneIndexInList].getWeight();
                         // fill the vec4
                         (*array)[j][0 + boneIndexInVec4] = boneIndex;
                         (*array)[j][1 + boneIndexInVec4] = boneWeight;
                      }
                  }
              }
          }
          return arrayList;
      }

      void computeMatrixPaletteUniform(const osg::Matrix& transformFromSkeletonToGeometry, const osg::Matrix& invTransformFromSkeletonToGeometry)
      {
         if (mBoneModeQuat)
         {
            osg::Vec3 zero;
            for (int i = 0; i < (int)_bonePalette.size(); i++)
            {
               osg::ref_ptr<osgAnimation::Bone> bone = _bonePalette[i].get();
               const osg::Matrix& invBindMatrix = bone->getInvBindMatrixInSkeletonSpace();
               osg::Matrix boneMatrix = bone->getMatrixInSkeletonSpace();
               
               osg::Matrix resultBoneMatrix = invBindMatrix * boneMatrix;
               osg::Matrix boneXform = transformFromSkeletonToGeometry * resultBoneMatrix * invTransformFromSkeletonToGeometry;

               osg::Matrix matRot(boneXform.getRotate());
               osg::Vec3 vec(boneXform.getTrans());

               // Turn the xform into a rotation matrix
               // by zeroing out translation.
               boneXform.setTrans(zero);

               osg::Vec4 rotX, rotY, rotZ;
               rotX = dtUtil::MatrixUtil::GetRow4(matRot, 0);
               rotY = dtUtil::MatrixUtil::GetRow4(matRot, 1);
               rotZ = dtUtil::MatrixUtil::GetRow4(matRot, 2);

               rotX[3] = vec.x();
               rotY[3] = vec.y();
               rotZ[3] = vec.z();

               if ( ! (_uniformMatrixPalette->setElement(i * 3 + 0, rotX)
                  && _uniformMatrixPalette->setElement(i * 3 + 1, rotY)
                  && _uniformMatrixPalette->setElement(i * 3 + 2, rotZ)))
               {
                  OSG_WARN << "RigTransformHardware::computeUniformMatrixPalette can't set uniform at " << i << " elements" << std::endl;
               }

               mScale->set(mModelWrapper->GetScale());
            }
         }
         else // Matrix Bone Mode
         {
            for (int i = 0; i < (int)_bonePalette.size(); i++)
            {
               osg::ref_ptr<osgAnimation::Bone> bone = _bonePalette[i].get();
               const osg::Matrix& invBindMatrix = bone->getInvBindMatrixInSkeletonSpace();
               const osg::Matrix& boneMatrix = bone->getMatrixInSkeletonSpace();
               osg::Matrix resultBoneMatrix = invBindMatrix * boneMatrix;
               osg::Matrix result =  transformFromSkeletonToGeometry * resultBoneMatrix * invTransformFromSkeletonToGeometry;
               if (!_uniformMatrixPalette->setElement(i, result))
                  OSG_WARN << "RigTransformHardware::computeUniformMatrixPalette can't set uniform at " << i << " elements" << std::endl;
            }
         }
      }

      bool mBoneModeQuat;
      dtCore::RefPtr<osg::Uniform> mScale;
      dtCore::ObserverPtr<dtAnim::OsgModelWrapper> mModelWrapper;
   };



   struct RigGeometryModeSwitcher : public osg::NodeVisitor
   {
      typedef osg::NodeVisitor BaseClass;

       RigGeometryModeSwitcher(dtAnim::OsgModelWrapper& modelWrapper, CharacterShaderBuilder& shaderBuilder, bool hardware = true)
          : BaseClass(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
          , mHardwareMode(hardware)
          , mBoneQuatMode(false)
          , mNumConversions(0)
          , mModelWrapper(&modelWrapper)
          , mShaderBuilder(&shaderBuilder)
       {}

       bool IsHardwareMode(osgAnimation::RigGeometry& rigGeom)
       {
         return NULL != dynamic_cast<OsgRigTransformHardware*>(rigGeom.getRigTransformImplementation());
       }
    
       void apply(osg::Geode& geode)
       {
          int geomCount = 0;
          osgAnimation::RigGeometry* rigGeom = NULL;
          for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
          {
              rigGeom = dynamic_cast<osgAnimation::RigGeometry*>(geode.getDrawable(i));
              if (rigGeom != NULL)
              {
                 if (mBoneQuatMode)
                 {
                    //mShaderBuilder->LoadShaders(*mModelData, geode);
                 }

                  apply(*rigGeom);
                  ++geomCount;
              }
          }

          if (geomCount > 0)
          {
             //mShaderBuilder->LoadShaders(*mModelData, geode);
          }
       }

       void apply(osgAnimation::RigGeometry& rigGeom)
       {
            // Determine if there is a difference in software/hardware mode.
            if (mHardwareMode != IsHardwareMode(rigGeom))
            {
               dtCore::RefPtr<osgAnimation::RigTransform> newRigTransImp;

               // Convert geometry to hardware mode.
               if (mHardwareMode)
               {
                  newRigTransImp = new OsgRigTransformHardware(*mModelWrapper, mBoneQuatMode);
               }
               else // Convert geometry to software mode.
               {
                  newRigTransImp = new osgAnimation::RigTransformSoftware;
               }
                  
               // Set the new implementation and update
               // it so that it is ready for rendering.
               rigGeom.setRigTransformImplementation(newRigTransImp.get());
               rigGeom.update();

               ++mNumConversions;
            }

#if 0
            if (geom.getName() != std::string("BoundingBox")) // we disable compute of bounding box for all geometry except our bounding box
               geom.setComputeBoundingBoxCallback(new osg::Drawable::ComputeBoundingBoxCallback);
   #endif
       }

       bool mHardwareMode;
       bool mBoneQuatMode;
       unsigned int mNumConversions;

       dtCore::ObserverPtr<dtAnim::OsgModelWrapper> mModelWrapper;
       dtCore::RefPtr<dtAnim::CharacterShaderBuilder> mShaderBuilder;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgNodeBuilder::OsgNodeBuilder()
   {}

   OsgNodeBuilder::~OsgNodeBuilder()
   {}

   dtCore::RefPtr<osg::Node> OsgNodeBuilder::CreateSoftware(osg::RenderInfo* renderInfo, dtAnim::OsgModelWrapper* wrapper)
   {
      dtCore::RefPtr<osg::Node> node = CreateNode(*wrapper);
      
      if (EnsureMode(*wrapper, *node, false))
      {
         // DEBUG:
         printf("Model \"%s\" switched to SOFTWARE mode.\n", node->getName().c_str());
      }
      
      wrapper->HandleModelUpdated();

      return node;
   }

   dtCore::RefPtr<osg::Node> OsgNodeBuilder::CreateSoftwareNoVBO(osg::RenderInfo* renderInfo, dtAnim::OsgModelWrapper* wrapper)
   {
      // TODO:

      return CreateSoftware(renderInfo, wrapper);
   }

   dtCore::RefPtr<osg::Node> OsgNodeBuilder::CreateHardware(osg::RenderInfo* renderInfo, dtAnim::OsgModelWrapper* wrapper)
   {
      dtCore::RefPtr<osg::Node> node = CreateNode(*wrapper);

      if (EnsureMode(*wrapper, *node, true))
      {
         // DEBUG:
         printf("Model \"%s\" switched to HARDWARE mode.\n", node->getName().c_str());
      }
      
      wrapper->HandleModelUpdated();

      return node;
   }
   
   dtCore::RefPtr<osg::Node> OsgNodeBuilder::CreateNode(dtAnim::OsgModelWrapper& wrapper)
   {
      dtCore::RefPtr<osg::Node> node;

      // Construct a drawable from information contained in model data.
      OsgModelData* modelData = wrapper.GetOsgModelData();
      if (modelData != NULL)
      {
         node = modelData->CreateModelClone();
      }

      return node.get();
   }
   
   bool OsgNodeBuilder::EnsureMode(dtAnim::OsgModelWrapper& wrapper, osg::Node& modelNode, bool hardware)
   {
      dtCore::RefPtr<dtAnim::CharacterShaderBuilder> shaderBuilder = new dtAnim::CharacterShaderBuilder;
      RigGeometryModeSwitcher modeSwitcher(wrapper, *shaderBuilder, hardware);
      modeSwitcher.mBoneQuatMode = true;
      modelNode.accept(modeSwitcher);

      return modeSwitcher.mNumConversions > 0;
   }

}
