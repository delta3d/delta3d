#include <osgDB/ReadFile>
#include <dtChar/coremodel.h>

using namespace dtChar;

CoreModel::CoreModel() 
{
	//nothing
}

CoreModel::CoreModel(const std::string& name) 
{
	mCalCoreModel = new CalCoreModel(name);
}

void CoreModel::LoadMaterialTextures(std::string directory, int materialId) {
	// get the core material
	CalCoreMaterial *pCoreMaterial;
	pCoreMaterial = mCalCoreModel->getCoreMaterial(materialId);

	// loop through all maps of the core material
	int mapId;
	for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
	{
		// get the filename of the texture
		std::string strFilename;
		strFilename = pCoreMaterial->getMapFilename(mapId);

		// load the texture from the file
		osg::Image *img = osgDB::readImageFile(directory+"/"+strFilename);
        assert(img);

		osg::Texture2D *texture = new osg::Texture2D();
		texture->setImage(img);
		texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
		mTextures.push_back(texture);

		// store the opengl texture id in the user data of the map
		pCoreMaterial->setMapUserData(mapId, (Cal::UserData)texture);
	}

	// make one material thread for each material
	// NOTE: this is not the right way to do it, but this viewer can't do the right
	// mapping without further information on the model etc., so this is the only
	// thing we can do here.
	// NOTE from Ryu: I don't know that the hell is a material thread, so I
	// paste this code as is and pray for it to work...

	// create the a material thread
	mCalCoreModel->createCoreMaterialThread(materialId);

	// initialize the material thread
	mCalCoreModel->setCoreMaterialId(materialId, 0, materialId);
}

void CoreModel::LoadAllTextures(std::string directory) {
	int materialId;
	for(materialId = 0; materialId < mCalCoreModel->getCoreMaterialCount(); materialId++)
	{
		// get the core material
		CalCoreMaterial *pCoreMaterial;
		pCoreMaterial = mCalCoreModel->getCoreMaterial(materialId);

		// loop through all maps of the core material
		int mapId;
		for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
		{
			// get the filename of the texture
			std::string strFilename;
			strFilename = pCoreMaterial->getMapFilename(mapId);

			// load the texture from the file
			osg::Image *img = osgDB::readImageFile(directory+"/"+strFilename);
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
	for(materialId = 0; materialId < mCalCoreModel->getCoreMaterialCount(); materialId++)
	{
		// create the a material thread
		mCalCoreModel->createCoreMaterialThread(materialId);

		// initialize the material thread
		mCalCoreModel->setCoreMaterialId(materialId, 0, materialId);
	}
}

void CoreModel::LoadSkeleton(std::string skeleton_file) 
{
	if (mCalCoreModel->loadCoreSkeleton(skeleton_file)) 
    {
		mSkeleton=skeleton_file;
	}
    else
    {
		CalError::printLastError();
	}
}

void CoreModel::LoadAnimation(std::string animation_file) 
{
	if (mCalCoreModel->loadCoreAnimation(animation_file) >= 0) 
    {
		mAnimations.push_back(std::make_pair(animation_file,"nombre_animacion"));
	} 
    else
    {
		CalError::printLastError();
	}
}

void CoreModel::LoadAnimation(std::string animation_file, std::string animation_name) 
{
	if (mCalCoreModel->loadCoreAnimation(animation_file, animation_name) >= 0) 
    {
		mAnimations.push_back(std::make_pair(animation_file,animation_name));
	}
    else
    {
		CalError::printLastError();
	}
}

int CoreModel::GetAnimationId(std::string animation_name) const
{
    int animation_id;
	if ((animation_id = mCalCoreModel->getCoreAnimationId(animation_name)) >= 0) 
    {
		return animation_id;
	}
	else
    {
	   CalError::printLastError();
       return -1;
	}
}

void CoreModel::LoadMesh(std::string mesh_file)
{
	if (mCalCoreModel->loadCoreMesh(mesh_file) >= 0) 
    {
		mMeshes.push_back(mesh_file);
	}
    else
    {
		CalError::printLastError();
	}
}

void CoreModel::LoadMaterial(std::string material_file) 
{
    int material_id;
	if ((material_id = mCalCoreModel->loadCoreMaterial(material_file)) >= 0) 
    {
		mMaterials.push_back(material_file);
		// get the directory path of the textures
		std::string textures_path = std::string(material_file,0,material_file.rfind("/"));        

		LoadMaterialTextures(textures_path,material_id);
	}
    else
    {
		CalError::printLastError();
	}
}

osg::Object* CoreModel::clone(const osg::CopyOp&) const 
{
	dtChar::CoreModel *model = new dtChar::CoreModel("dummy");
	return model;
}
