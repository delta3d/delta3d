
#include <dtAnim/cal3dloader.h>
#include <cal3d/model.h>
#include <cal3d/coremodel.h>
#include <dtAnim/characterfilehandler.h>
#include <dtUtil/xercesparser.h>
#include <dtCore/globals.h>
#include <osgDB/ReadFile>
#include <osg/Texture2D>

using namespace dtAnim;

Cal3DLoader::Cal3DLoader()
{
}

Cal3DLoader::~Cal3DLoader()
{
}

/**
 * @return Could return NULL if the file didn't load.
 */
CalCoreModel* Cal3DLoader::GetCoreModel( const std::string &filename )
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
      if( parser.Parse(filename, handler) )
      {
         coreModel = new CalCoreModel(handler.mName);

         //load skeleton
         coreModel->loadCoreSkeleton(FindFileInPathList(handler.mSkeletonFilename));
      
         //load animations
         std::vector<std::string>::iterator animItr = handler.mAnimationFilenames.begin();
         while (animItr != handler.mAnimationFilenames.end())
         {
            coreModel->loadCoreAnimation( FindFileInPathList(*animItr) );
            ++animItr;
         }

         //load meshes
         std::vector<std::string>::iterator meshItr = handler.mMeshFilenames.begin();
         while (meshItr != handler.mMeshFilenames.end())
         {
            coreModel->loadCoreMesh( FindFileInPathList(*meshItr) );
            ++meshItr;
         }

         //load materials
         std::vector<std::string>::iterator matItr = handler.mMaterialFilenames.begin();
         while (matItr != handler.mMaterialFilenames.end())
         {
            coreModel->loadCoreMaterial( FindFileInPathList(*matItr) );
            ++matItr;
         }

         mFilenameCoreModelMap[filename] = coreModel; //store it for later
      }      
   }

   return coreModel;
}

/**
 * @return a fully defined CalMode, or NULL if file can't be read
 */
CalModel* Cal3DLoader::Load( const std::string &filename )
{
   CalModel *model = NULL;

   CalCoreModel *coreModel = GetCoreModel(filename);
   
   LoadAllTextures(coreModel); //this should be a user-level process.

   if (coreModel != NULL)
   {
      model = new CalModel(coreModel);
   }

   return model;
}


void Cal3DLoader::LoadAllTextures(CalCoreModel *coreModel)
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
         osg::Image *img = osgDB::readImageFile(strFilename);
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
   // NOTE from Ryu: I don't know that the hell is a material thread, so I
   // paste this code as is and pray for it to work...
   for(materialId = 0; materialId < coreModel->getCoreMaterialCount(); materialId++)
   {
      // create the a material thread
      coreModel->createCoreMaterialThread(materialId);

      // initialize the material thread
      coreModel->setCoreMaterialId(materialId, 0, materialId);
   }
}