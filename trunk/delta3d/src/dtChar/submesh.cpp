#include <osg/Material>
#include <osg/Texture2D>
#include <dtChar/submesh.h>

using namespace dtChar;

class SubMeshDirtyCallback: public osg::Geometry::UpdateCallback {
public:
	virtual void 	update (osg::NodeVisitor *, osg::Drawable *d) {
		d->dirtyBound();
	}
};

SubMesh::SubMesh() {
	std::cout << "SubMesh::SubMesh(): You should never call this constructor!" << std::endl;
	exit(1);
}

SubMesh::SubMesh(CalModel *model, unsigned mesh, unsigned submesh) {
	this->model=model;
	this->mesh=mesh;
	this->submesh=submesh;

	setUseDisplayList(false);
	setUpMaterial();

	// initialize arrays, just in case
	meshVertices = 0;
	meshNormals = 0;
	meshTextureCoordinates = 0;
	meshFaces = 0;

	// set the model to LOD 1, so that we have the maximal number of
	// vertices and faces in order to be able to pre-allocate the arrays
	model->setLodLevel(1);
	pCalRenderer = model->getRenderer();
	if(pCalRenderer->beginRendering())
	{
		if(pCalRenderer->selectMeshSubmesh(mesh, submesh))
		{
			vertexCount = pCalRenderer->getVertexCount();
			faceCount = pCalRenderer->getFaceCount();
			meshVertices = new float[vertexCount*3];
			meshNormals = new float[vertexCount*3];
			meshTextureCoordinates = new float[vertexCount*2];
			meshFaces = new CalIndex[faceCount*3];
			pCalRenderer->getFaces(meshFaces);
		}

		pCalRenderer->endRendering();
	}
	setUpdateCallback(new SubMeshDirtyCallback());
}

SubMesh::~SubMesh(void)
{
	delete [] meshVertices;
	delete [] meshNormals;
	delete [] meshFaces;
	delete [] meshTextureCoordinates;
}

void SubMesh::setUpMaterial() {
	CalRenderer *pCalRenderer;
	pCalRenderer = model->getRenderer();
	osg::StateSet *set = this->getOrCreateStateSet();
	if(pCalRenderer->beginRendering()) {
		// select mesh and submesh for further data access
		if(pCalRenderer->selectMeshSubmesh(mesh, submesh)) {
			osg::Material *material = new osg::Material();
			material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
			set->setAttributeAndModes(material, osg::StateAttribute::ON);
			unsigned char meshColor[4];
			osg::Vec4 materialColor;

			// set the material ambient color
			pCalRenderer->getAmbientColor(&meshColor[0]);
			materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
			if (materialColor[3] == 0) materialColor[3]=1.0f;
			material->setAmbient(osg::Material::FRONT, materialColor);

			// set the material diffuse color
			pCalRenderer->getDiffuseColor(&meshColor[0]);
			materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
			if (materialColor[3] == 0) materialColor[3]=1.0f;
			material->setDiffuse(osg::Material::FRONT, materialColor);

			// set the material specular color
			pCalRenderer->getSpecularColor(&meshColor[0]);
			materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
			if (materialColor[3] == 0) materialColor[3]=1.0f;
			material->setSpecular(osg::Material::FRONT, materialColor);

			// set the material shininess factor
			float shininess;
			shininess = pCalRenderer->getShininess();
			material->setShininess(osg::Material::FRONT, shininess);

			if (pCalRenderer->getMapCount() > 0)
			{
				osg::Texture2D *texture = (osg::Texture2D*)pCalRenderer->getMapUserData(0);
				if (texture != 0) {
					set->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
				}
			}
		}
		pCalRenderer->endRendering();
	}
}

void SubMesh::drawImplementation(osg::State& state) const {
	// begin the rendering loop
	if(pCalRenderer->beginRendering())
	{
		// select mesh and submesh for further data access
		if(pCalRenderer->selectMeshSubmesh(mesh, submesh))
		{
			// get the transformed vertices of the submesh
			vertexCount = pCalRenderer->getVertexCount();
			vertexCount = pCalRenderer->getVertices(meshVertices);

			// get the transformed normals of the submesh
			pCalRenderer->getNormals(meshNormals);

			// get the texture coordinates of the submesh
			// this is still buggy, it renders only the first texture.
			// it should be a loop rendering each texture on its own texture unit
			unsigned tcount = pCalRenderer->getTextureCoordinates(0, meshTextureCoordinates);

			// flip coordinates
			for (unsigned int i=1;i<vertexCount*2;i=i+2) {
				meshTextureCoordinates[i] = 1.0f - meshTextureCoordinates[i];
			}

			// set the vertex and normal buffers
			state.setVertexPointer(3, GL_FLOAT, 0, meshVertices);
			state.setNormalPointer(GL_FLOAT, 0, meshNormals);

			// set the texture coordinate buffer and state if necessary
			if((pCalRenderer->getMapCount() > 0) && (tcount > 0))
			{
				// set the texture coordinate buffer
				state.setTexCoordPointer(0, 2, GL_FLOAT, 0, meshTextureCoordinates);
			}
            /// White color!
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			// draw the submesh

			if(sizeof(CalIndex)==2)
				glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_SHORT, meshFaces);
			else
				glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, meshFaces);

			// get the faces of the submesh for the next frame
			faceCount = pCalRenderer->getFaces(meshFaces);
		}

		// end the rendering
		pCalRenderer->endRendering();
	}
}


void SubMesh::accept(osg::PrimitiveFunctor& functor) const
{
	functor.setVertexArray(vertexCount, (osg::Vec3f *)(meshVertices));

	if(sizeof(CalIndex) == 2)
	{
		osg::ref_ptr<osg::DrawElementsUShort> pset = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLES,
																				 faceCount*3, (GLushort *) meshFaces);
		pset->accept(functor);
	} else {
		osg::ref_ptr<osg::DrawElementsUInt> pset = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,
																			 faceCount*3, (GLuint *) meshFaces);
		pset->accept(functor);
	}

	return;
}

osg::Object* SubMesh::clone(const osg::CopyOp&) const {
	return new SubMesh(model, mesh, submesh);
}

osg::BoundingBox SubMesh::computeBound() const {
	osg::BoundingBox bbox;
    bbox.init();
	// We have to traverse all vertices to racalculate bounds
	CalRenderer *pCalRenderer;
	pCalRenderer = model->getRenderer();
	if(pCalRenderer->beginRendering()) {
		if(pCalRenderer->selectMeshSubmesh(mesh, submesh)) {
			// get the transformed vertices of the submesh
			int vertexCount = pCalRenderer->getVertexCount();
			float *meshVertices = new float[vertexCount*3];
			vertexCount = pCalRenderer->getVertices(meshVertices);
			for (int vertex=0;vertex<vertexCount;vertex++) {
				float* v=meshVertices+vertex*3;
				bbox.expandBy(v[0],v[1],v[2]);
			}
			delete []meshVertices;
		}
		pCalRenderer->endRendering();
	}
	return bbox;
}
