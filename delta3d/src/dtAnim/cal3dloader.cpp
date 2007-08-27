
#include <dtAnim/cal3dloader.h>
#include <cal3d/model.h>
#include <cal3d/coremodel.h>
#include <cal3d/coreanimation.h>
#include <dtAnim/characterfilehandler.h>
#include <dtAnim/cal3dmodelwrapper.h> 
#include <dtAnim/animationwrapper.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/log.h>
#include <dtCore/globals.h>
#include <osgDB/ReadFile>
#include <osg/Texture2D>

using namespace dtUtil;

namespace dtAnim 
{

   /////////////////////////////////////////////////////////////////////////////////
   Cal3DLoader::Cal3DLoader()
   : mTextures()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   Cal3DLoader::~Cal3DLoader()
   {
      PurgeAllCaches();
   }

   /**
    * @return Could return NULL if the file didn't load.
    * @throw SAXParseException if the file didn't parse correctly
    * @note Relies on the the "animationdefinition.xsd" schema file
    */
   CalCoreModel* Cal3DLoader::GetCoreModel(CharacterFileHandler& handler, const std::string &filename, const std::string &path )
   {
      using namespace dtCore;

      CalCoreModel *coreModel = NULL;

      //gotta parse the file and create/store a new CalCoreModel
      dtUtil::XercesParser parser;

      if (parser.Parse(filename, handler, "animationdefinition.xsd"))
      {
         coreModel = new CalCoreModel(handler.mName);      

         //load skeleton
         std::string skelFile = FindFileInPathList(path + handler.mSkeletonFilename);
         if (!skelFile.empty())
         {
            coreModel->loadCoreSkeleton(FindFileInPathList(path + handler.mSkeletonFilename));
         }
         else
         {
            LOG_ERROR("Can't find the skeleton file named:'" + path + handler.mSkeletonFilename + "'.");
         }

         //load animations
         std::vector<CharacterFileHandler::AnimationStruct>::iterator animItr = handler.mAnimations.begin();
         while (animItr != handler.mAnimations.end())
         {
            std::string filename = FindFileInPathList(path + (*animItr).mFileName);

            if (!filename.empty()) 
            {
               coreModel->loadCoreAnimation( filename, (*animItr).mName );
            }
            else
            {
               LOG_ERROR("Can't find animation file named:'" + path + (*animItr).mFileName + "'.");
            }
            ++animItr;
         }

         //load meshes
         std::vector<CharacterFileHandler::MeshStruct>::iterator meshItr = handler.mMeshes.begin();
         while (meshItr != handler.mMeshes.end())
         {
            std::string filename = FindFileInPathList(path + (*meshItr).mFileName);
            if (!filename.empty())
            {
               coreModel->loadCoreMesh( filename, (*meshItr).mName );
            }
            else
            {
               LOG_ERROR("Can't find mesh file named:'" + path + (*meshItr).mFileName + "'.");
            }
            ++meshItr;
         }

         //load materials
         std::vector<CharacterFileHandler::MaterialStruct>::iterator matItr = handler.mMaterials.begin();
         while (matItr != handler.mMaterials.end())
         {
            std::string filename = FindFileInPathList(path + (*matItr).mFileName);

            if (!filename.empty())  
            {
               coreModel->loadCoreMaterial( filename, (*matItr).mName);
            }
            else
            {
               LOG_ERROR("Can't find material file named:'" + path + (*matItr).mFileName + "'.");
            }
            ++matItr;
         }
      }            

      return coreModel;
   }

   /**
    * Will use the Delta3D search paths to find the supplied filename.  Will create
    * a new Cal3DModelWrapper, but you're responsible for deleting it.
    * @note The animations are named with their filenames by default, or by an
    *       optional name attribute in the .xml file.
    * @return A fully defined CalModel wrapped by a Cal3DModelWrapper.  RefPtr could
    *         be not valid (wrapper->valid()==false) if the file didn't load correctly.
    * @see SetDataFilePathList()
    * @throw SAXParseException If the file wasn't formatted correctly
    */
   bool Cal3DLoader::Load(const std::string &filename, Cal3DModelData*& data_in)
   {
      std::string path;
      std::string::size_type stringIndex = filename.find_last_of("\\");
      std::string::size_type lastIndex = filename.find_last_of("/");

      //lets take the bigger of the two that isnt equal to npos
      if(lastIndex != std::string::npos)
      {
         if(stringIndex != std::string::npos) stringIndex = (stringIndex > lastIndex) ? stringIndex : lastIndex;
         else stringIndex = lastIndex;
      }


      if (stringIndex != std::string::npos)
      {
         // The index is the position of the first backslash, so add 1
         path = filename.substr(0, stringIndex + 1);
      }

      CalCoreModel *coreModel = NULL;

      CharacterFileHandler handler;
      coreModel = GetCoreModel(handler, filename, path);
      if(coreModel != NULL)
      {
         data_in = new Cal3DModelData(coreModel, filename);
         LoadModelData(handler, *coreModel, *data_in);
         LoadAllTextures(*coreModel, path); //this should be a user-level process.
         
         return true;
      }  
      else
      {
         LOG_ERROR("Unable to load character file: '" + filename + "'");
      }
   

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void Cal3DLoader::LoadModelData(dtAnim::CharacterFileHandler& handler, CalCoreModel& model, Cal3DModelData& modelData)
   {
      //create animation wrappers
      int numAnims = model.getCoreAnimationCount();
      modelData.GetAnimationWrappers().reserve(numAnims);

      for(int i = 0; i < numAnims; ++i)
      {
         CalCoreAnimation* anim = model.getCoreAnimation(i);
         if(anim)
         {
            AnimationWrapper* pWrapper = new AnimationWrapper(anim->getName(), i);
            pWrapper->SetDuration(anim->getDuration());
            pWrapper->SetSpeed(float(anim->getTotalNumberOfKeyframes()) / anim->getDuration());
            modelData.Add(pWrapper);
         }
         else
         {
            LOG_ERROR("Cannot find CalCoreAnimation for animation '" + anim->getName() + "'");
         }
      }

      modelData.SetShaderGroupName(handler.mShaderGroup);
      modelData.SetShaderName(handler.mShaderName);
      modelData.SetShaderMaxBones(handler.mShaderMaxBones);
      
      //register animations
      if(!handler.mAnimationChannels.empty())
      {
         int numAnimatables = handler.mAnimationChannels.size() + handler.mAnimationSequences.size();
         modelData.GetAnimatables().reserve(numAnimatables);

         std::vector<CharacterFileHandler::AnimationChannelStruct>::iterator channelIter = handler.mAnimationChannels.begin();
         std::vector<CharacterFileHandler::AnimationChannelStruct>::iterator channelEnd = handler.mAnimationChannels.end();
         for(;channelIter != channelEnd; ++channelIter)
         {         
            CharacterFileHandler::AnimationChannelStruct& pStruct = *channelIter;

            int id = model.getCoreAnimationId(pStruct.mAnimationName);
            if(id >= 0 && id < numAnims)
            {
               dtCore::RefPtr<AnimationChannel> pChannel = new AnimationChannel();

               pChannel->SetAnimation(modelData.GetAnimationWrappers()[id].get());

               pChannel->SetName(pStruct.mName);
               pChannel->SetLooping(pStruct.mIsLooping);
               pChannel->SetAction(pStruct.mIsAction);
               pChannel->SetMaxDuration(pStruct.mMaxDuration);
               pChannel->SetStartDelay(pStruct.mStartDelay);
               pChannel->SetBaseWeight(pStruct.mBaseWeight);
               pChannel->SetFadeIn(pStruct.mFadeIn);
               pChannel->SetFadeOut(pStruct.mFadeOut);
               pChannel->SetSpeed(pStruct.mSpeed);

               modelData.Add(pChannel.get());
            }
            else
            {
               LOG_ERROR("Unable to find animation '" + pStruct.mAnimationName + "' within the CalCoreModel.");
            }

         }

         std::vector<CharacterFileHandler::AnimationSequenceStruct>::iterator sequenceIter = handler.mAnimationSequences.begin();
         std::vector<CharacterFileHandler::AnimationSequenceStruct>::iterator sequenceEnd = handler.mAnimationSequences.end();
         for(;sequenceIter != sequenceEnd; ++sequenceIter)
         {         
            CharacterFileHandler::AnimationSequenceStruct& pStruct = *sequenceIter;

            dtCore::RefPtr<AnimationSequence> pSequence = new AnimationSequence();

            pSequence->SetName(pStruct.mName);
            pSequence->SetStartDelay(pStruct.mStartDelay);
            pSequence->SetFadeIn(pStruct.mFadeIn);
            pSequence->SetFadeOut(pStruct.mFadeOut);
            pSequence->SetSpeed(pStruct.mSpeed);
            pSequence->SetBaseWeight(pStruct.mBaseWeight);

            //find children
            std::vector<std::string>::const_iterator i = pStruct.mChildNames.begin();
            std::vector<std::string>::const_iterator end = pStruct.mChildNames.end();
            for (; i != end; ++i)
            {
               const std::string& nameToFind = *i;
               Cal3DModelData::AnimatableArray::iterator animIter = modelData.GetAnimatables().begin();
               Cal3DModelData::AnimatableArray::iterator animIterEnd = modelData.GetAnimatables().end();

               for (; animIter != animIterEnd; ++animIter)
               {
                  Animatable* animatable = animIter->get();
                  if (animatable->GetName() == nameToFind)
                  {
                     pSequence->GetChildAnimations().push_back(animatable);
                  }
               }
            }
            
            modelData.Add(pSequence.get());

         }
      }

   }

   /////////////////////////////////////////////////////////////////////////////////
   void Cal3DLoader::LoadAllTextures(CalCoreModel& coreModel, const std::string &path)
   {
      int materialId;
      for(materialId = 0; materialId < coreModel.getCoreMaterialCount(); materialId++)
      {
         // get the core material
         CalCoreMaterial *pCoreMaterial;
         pCoreMaterial = coreModel.getCoreMaterial(materialId);

         // loop through all maps of the core material
         int mapId;
         for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
         {
            // get the filename of the texture
            std::string strFilename;
            strFilename = pCoreMaterial->getMapFilename(mapId);

            TextureMap::iterator textureIterator = mTextures.find(strFilename);

            if(textureIterator == mTextures.end())
            {
               // load the texture from the file
               osg::Image *img = osgDB::readImageFile(path + strFilename);

               if(!img)
               {
                  LOG_ERROR("Unable to load image file: " + strFilename);
                  continue;
               }

               osg::Texture2D *texture = new osg::Texture2D();
               texture->setImage(img);
               texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
               texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
               mTextures[strFilename] = texture;

               // store the opengl texture id in the user data of the map
               pCoreMaterial->setMapUserData(mapId, (Cal::UserData)texture);
            }
            else
            {            
               pCoreMaterial->setMapUserData(mapId, (Cal::UserData)((*textureIterator).second.get()));
            }
         }
      }

      // make one material thread for each material
      // NOTE: this is not the right way to do it, but this viewer can't do the right
      // mapping without further information on the model etc., so this is the only
      // thing we can do here.

      // Every part of the core model (every submesh to be more exact) has a material
      // thread assigned. You can now very easily change the look of a model instance, 
      // by simply select a new current material set for its parts. The Cal3D library 
      // is now able to look up the material in the material grid with the given new 
      // material set and the material thread stored in the core model parts.
      for(materialId = 0; materialId < coreModel.getCoreMaterialCount(); materialId++)
      {
         // create the a material thread
         coreModel.createCoreMaterialThread(materialId);

         // initialize the material thread
         coreModel.setCoreMaterialId(materialId, 0, materialId);
      }
   }

   /** Use if you want to completely start over with no history of previous 
    * animated entities that have been created.  This will allow you to reload
    * files for a second time.
    * @note: currently this will remove reference to all created osg Textures as well, which
    *        might cause the texture to be deleted.
    */
   void Cal3DLoader::PurgeAllCaches()
   {
      mTextures.clear(); 
   }
}
