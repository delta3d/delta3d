#include <osg/NodeCallback>
#include <dtChar/model.h>
#include <dtChar/submesh.h>

using namespace dtChar;

Model::Model() {
	mCalModel = 0;
}

Model::Model(CoreModel *core) {
	mCalModel = new CalModel(core->get());
	Create(core);
}

void Model::SetTimeScale(float timeScale) {
	if (mCalUpdateCallback != NULL) {
		mCalUpdateCallback->setTimeScale(timeScale);
	}
	else {
		std::cerr << "Model:: calUpdateCallback isn't valid!" << std::endl;
	}
}

float Model::GetTimeScale() {
	if (mCalUpdateCallback != NULL) {
		return (mCalUpdateCallback->getTimeScale());
	}
	else {
		std::cerr << "Model:: calUpdateCallback isn't valid!" << std::endl;
      return 0.f;
	}
}

void Model::Create(CoreModel *core) {
	if (!mCalModel) {
		mCalModel  = new CalModel(core->get());
	}
	mCalUpdateCallback = new CalUpdateCallback(mCalModel);
	
	osg::NodeCallback* nodeCallback = getUpdateCallback();
	
	if ( nodeCallback ) {
		nodeCallback->addNestedCallback(mCalUpdateCallback);
	}
	else {
		setUpdateCallback(mCalUpdateCallback);
	}

	//setUpdateCallback(new CalUpdateCallback(model));
	// attach all meshes to the model
	for(int meshId = 0; meshId < core->get()->getCoreMeshCount(); meshId++)
	{
		mCalModel->attachMesh(meshId);
	}
	mCalModel->setMaterialSet(0);

	CalRenderer *pCalRenderer = mCalModel->getRenderer();
	if(pCalRenderer->beginRendering()) {
		int meshCount = pCalRenderer->getMeshCount();
		for(int meshId = 0; meshId < meshCount; meshId++) {
			int submeshCount = pCalRenderer->getSubmeshCount(meshId);
			for(int submeshId = 0; submeshId < submeshCount; submeshId++) {
				SubMesh *submesh = new SubMesh(mCalModel, meshId, submeshId);
				addDrawable(submesh);
			}
		}
		pCalRenderer->endRendering();
	}

	/// Force generation of first mesh
	mCalModel->update(0);
	this->mCoreModel=core;
}

void Model::StartAction(unsigned id, float delay_in, float delay_out) {
	if (!mCalModel) {
		std::cerr << "Model:: Call Model::create() first!" << std::endl;
		throw -1;
	}
	if (mCalModel->getMixer() != 0)
		mCalModel->getMixer()->executeAction(id, delay_in, delay_out);
	else std::cerr << "Model::action: ERROR: The model doesn't have a mixer" << std::endl;
}

void Model::StopAction(unsigned id) {
	if (!mCalModel) {
		std::cerr << "Model:: Call Model::create() first!" << std::endl;
		throw -1;
	}
	if (mCalModel->getMixer() != 0)
		mCalModel->getMixer()->removeAction(id);
	else std::cerr << "Model::action: ERROR: The model doesn't have a mixer" << std::endl;
}

void Model::StartLoop(unsigned id, float weight, float delay) {
	if (!mCalModel) {
		std::cerr << "Model:: Call Model::create() first!" << std::endl;
		throw -1;
	}
	if (mCalModel->getMixer() != 0)
		mCalModel->getMixer()->blendCycle(id, weight, delay);
	else std::cerr << "Model::loop: ERROR: The model doesn't have a mixer" << std::endl;
}

void Model::StopLoop(unsigned id, float delay) {
	if (!mCalModel) {
		std::cerr << "Model:: Call Model::create() first!" << std::endl;
		throw -1;
	}
	if (mCalModel->getMixer() != 0)
		mCalModel->getMixer()->clearCycle(id, delay);
	else std::cerr << "Model::stop: ERROR: The model doesn't have a mixer" << std::endl;
}

osg::Object* Model::clone(const osg::CopyOp&) const {
	return new Model();
}

