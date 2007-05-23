
#include <dtAnim/cal3dloader.h>
#include <cal3d/model.h>
#include <cal3d/coremodel.h>
#include <cal3d/coreanimation.h>
#include <dtAnim/characterfilehandler.h>
#include <dtAnim/cal3dmodelwrapper.h> 
#include <dtAnim/animationwrapper.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/animationcontroller.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/log.h>
#include <dtCore/globals.h>
#include <osgDB/ReadFile>
#include <osg/Texture2D>

using namespace dtAnim;
using namespace dtUtil;

/////////////////////////////////////////////////////////////////////////////////
Cal3DLoader::ModelData::ModelData()
: mAnimWrappers()
, mAnimatables()
{
}

Cal3DLoader::ModelData::~ModelData()
{
   mAnimWrappers.clear();
   mAnimatables.clear();
}

/////////////////////////////////////////////////////////////////////////////////
Cal3DLoader::Cal3DLoader()
{
}

/////////////////////////////////////////////////////////////////////////////////
Cal3DLoader::~Cal3DLoader()
{
   PurgeAllCaches();
}

/////////////////////////////////////////////////////////////////////////////////
const Cal3DLoader::AnimatableVector* Cal3DLoader::GetAnimatables(const Cal3DModelWrapper& wrapper) const
{
   ModelDataMap::const_iterator iter = mModelData.find(wrapper.GetCalModel()->getCoreModel());
   if(iter != mModelData.end())
   {
      return &((*iter).second->mAnimatables);
   }
   else
   {
      LOG_ERROR("Unable to find animations associated with Cal3DModelWrapper.");
      return 0;
   }
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

   //see if we have already created a CalCoreModel for this filename
   FilenameCoreModelMap::iterator found = mFilenameCoreModelMap.find(filename);
   if (found != mFilenameCoreModelMap.end())
   {
      coreModel = (*found).second;
   }
   else
   {
      //gotta parse the file and create/store a new CalCoreModel
      dtUtil::XercesParser parser;
      
      if (parser.Parse(filename, handler, "animationdefinition.xsd"))
      {
         coreModel = new CalCoreModel(handler.mName);      
         ModelData* newModelData = new ModelData();

         mModelData.insert(ModelDataMapping(coreModel, newModelData));

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
            std::string filename = FindFileInPathList(path + (*animItr).filename);

            if (!filename.empty()) 
            {
               coreModel->loadCoreAnimation( filename, (*animItr).name );
            }
            else
            {
               LOG_ERROR("Can't find animation file named:'" + path + (*animItr).filename + "'.");
            }
            ++animItr;
         }

         //load meshes
         std::vector<CharacterFileHandler::MeshStruct>::iterator meshItr = handler.mMeshes.begin();
         while (meshItr != handler.mMeshes.end())
         {
            std::string filename = FindFileInPathList(path + (*meshItr).filename);
            if (!filename.empty())
            {
               coreModel->loadCoreMesh( filename, (*meshItr).name );
            }
            else
            {
               LOG_ERROR("Can't find mesh file named:'" + path + (*meshItr).filename + "'.");
            }
            ++meshItr;
         }

         //load materials
         std::vector<CharacterFileHandler::MaterialStruct>::iterator matItr = handler.mMaterials.begin();
         while (matItr != handler.mMaterials.end())
         {
            std::string filename = FindFileInPathList(path + (*matItr).filename);

            if (!filename.empty())  
            {
               coreModel->loadCoreMaterial( filename, (*matItr).name );
            }
            else
            {
               LOG_ERROR("Can't find material file named:'" + path + (*matItr).filename + "'.");
            }
            ++matItr;
         }

         mFilenameCoreModelMap[filename] = coreModel; //store it for later
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
dtCore::RefPtr<Cal3DModelWrapper> Cal3DLoader::Load( const std::string &filename )
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

   dtCore::RefPtr<Cal3DModelWrapper> wrapper;
   CalCoreModel *coreModel = NULL;

   //see if we have already created a CalCoreModel for this filename
   FilenameCoreModelMap::iterator found = mFilenameCoreModelMap.find(filename);
   if (found != mFilenameCoreModelMap.end())
   {
      coreModel = (*found).second;      
   }
   else
   {
      CharacterFileHandler handler;
      coreModel = GetCoreModel(handler, filename, path);
      LoadModelData(handler, coreModel);
   }

   if (coreModel != NULL)
   {
      LoadAllTextures(coreModel, path); //this should be a user-level process.
      CalModel *model = new CalModel(coreModel);
      wrapper = new Cal3DModelWrapper(model);
   }

   return wrapper;
}

/////////////////////////////////////////////////////////////////////////////////
void Cal3DLoader::LoadModelData(dtAnim::CharacterFileHandler& handler, CalCoreModel* model)
{
   dtCore::RefPtr<ModelData> modelData = new ModelData();

   //create animation wrappers
   int numAnims = model->getCoreAnimationCount();
   modelData->mAnimWrappers.reserve(numAnims);
   
   for(int i = 0; i < numAnims; ++i)
   {
      CalCoreAnimation* anim = model->getCoreAnimation(i);
      if(anim)
      {
         AnimationWrapper* pWrapper = new AnimationWrapper(anim->getName(), i);
         modelData->mAnimWrappers.push_back(pWrapper);
      }
      else
      {
         LOG_ERROR("Cannot find CalCoreAnimation for animation '" + anim->getName() + "'");
      }
   }

   //register animations
   if(!handler.mAnimationChannels.empty())
   {
      AnimatableVector& vect = modelData->mAnimatables;
      modelData->mAnimatables.reserve(vect.size());

      std::vector<CharacterFileHandler::AnimationChannelStruct>::iterator channelIter = handler.mAnimationChannels.begin();
      std::vector<CharacterFileHandler::AnimationChannelStruct>::iterator channelEnd = handler.mAnimationChannels.end();
      for(;channelIter != channelEnd; ++channelIter)
      {         
         CharacterFileHandler::AnimationChannelStruct& pStruct = *channelIter;

         int id = model->getCoreAnimationId(pStruct.animation_name);
         if(id >= 0 && id < int(modelData->mAnimWrappers.size()))
         {
            AnimationChannel* pChannel = new AnimationChannel();
            
            pChannel->SetAnimation(modelData->mAnimWrappers[id].get());

            pChannel->SetName(pStruct.channel_name);
            pChannel->SetLooping(pStruct.is_looping);
            pChannel->SetAction(pStruct.is_action);
            pChannel->SetMaxDuration(pStruct.max_duration);
            pChannel->SetStartDelay(pStruct.start_delay);
            pChannel->SetFadeIn(pStruct.fade_in);
            pChannel->SetFadeOut(pStruct.fade_out);
            pChannel->SetSpeed(pStruct.speed);

            modelData->mAnimatables.push_back(pChannel);
         }
         else
         {
            LOG_ERROR("Unable to find animation '" + pStruct.animation_name + "' within the CalCoreModel.");
         }
         
      }
   }

   
   mModelData[model] = modelData;
}

/////////////////////////////////////////////////////////////////////////////////
void Cal3DLoader::LoadAllTextures(CalCoreModel *coreModel, const std::string &path)
{
   int materialId;
   for(materialId = 0; materialId < coreModel->getCoreMaterialCount(); materialId++)
   {
      // get the core material
      CalCoreMaterial *pCoreMaterial;
      pCoreMaterial = coreModel->getCoreMaterial(materialId);

      // loop through all maps of the core material
      int mapId;
      for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
      {
         // get the filename of the texture
         std::string strFilename;
         strFilename = pCoreMaterial->getMapFilename(mapId);

         // load the texture from the file
         osg::Image *img = osgDB::readImageFile(path + strFilename);
         assert(img);

         osg::Texture2D *texture = new osg::Texture2D();
         texture->setImage(img);
         texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
         texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
         mTextures.push_back(texture);

         // store the opengl texture id in the user data of the map
         pCoreMaterial->setMapUserData(mapId, (Cal::UserData)texture);
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
   for(materialId = 0; materialId < coreModel->getCoreMaterialCount(); materialId++)
   {
      // create the a material thread
      coreModel->createCoreMaterialThread(materialId);

      // initialize the material thread
      coreModel->setCoreMaterialId(materialId, 0, materialId);
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
   mModelData.clear();

   FilenameCoreModelMap::iterator itr = mFilenameCoreModelMap.begin();

   while (itr != mFilenameCoreModelMap.end())
   {
      delete itr->second;
      itr->second = NULL;
      ++itr;
   }

   mFilenameCoreModelMap.clear();
}
