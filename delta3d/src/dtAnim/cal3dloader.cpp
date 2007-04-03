
#include <dtAnim/cal3dloader.h>
#include <cal3d/model.h>
#include <cal3d/coremodel.h>
#include <dtAnim/characterfilehandler.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/log.h>
#include <dtCore/globals.h>
#include <osgDB/ReadFile>
#include <osg/Texture2D>

using namespace dtAnim;
using namespace dtUtil;

Cal3DLoader::Cal3DLoader()
{
}

Cal3DLoader::~Cal3DLoader()
{
}

/**
 * @return Could return NULL if the file didn't load.
 * @throw SAXParseException if the file didn't parse correctly
 * @note Relies on the the "animationdefinition.xsd" schema file
 */
CalCoreModel* Cal3DLoader::GetCoreModel( const std::string &filename, const std::string &path )
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
      CharacterFileHandler handler;
      
      parser.Parse(filename, handler, "animationdefinition.xsd");

      coreModel = new CalCoreModel(handler.mName);      

      //load skeleton
      coreModel->loadCoreSkeleton(FindFileInPathList(path + handler.mSkeletonFilename));

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
      std::vector<std::string>::iterator meshItr = handler.mMeshFilenames.begin();
      while (meshItr != handler.mMeshFilenames.end())
      {
         std::string name = FindFileInPathList(path + (*meshItr));
         if (!name.empty())
         {
            coreModel->loadCoreMesh( name );
         }
         else
         {
            LOG_ERROR("Can't find mesh file named:'" + path + (*meshItr) + "'.");
         }
         ++meshItr;
      }

      //load materials
      std::vector<std::string>::iterator matItr = handler.mMaterialFilenames.begin();
      while (matItr != handler.mMaterialFilenames.end())
      {
         std::string name = FindFileInPathList(path + (*matItr));

         if (!name.empty())  
         {
            coreModel->loadCoreMaterial( name );
         }
         else
         {
            LOG_ERROR("Can't find material file named:'" + path + (*matItr) + "'.");
         }
         ++matItr;
      }

      mFilenameCoreModelMap[filename] = coreModel; //store it for later
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

   if (stringIndex != std::string::npos)
   {
      // The index is the position of the first backslash, so add 1
      path = filename.substr(0, stringIndex + 1);
   }

   dtCore::RefPtr<Cal3DModelWrapper> wrapper;
   CalCoreModel *coreModel = GetCoreModel(filename, path);

   if (coreModel != NULL)
   {
      LoadAllTextures(coreModel, path); //this should be a user-level process.
      CalModel *model = new CalModel(coreModel);
      wrapper = new Cal3DModelWrapper(model);
   }

   return wrapper;
}


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
