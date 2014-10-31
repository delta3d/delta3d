/*
 * OSGExp - 3D Studio Max plugin for exporting OpenSceneGraph models.
 * Copyright (C) 2003  VR-C
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *	FILE:			Export.cpp
 *
 *	DESCRIPTION:	Methods for doing the real export
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 17.11.2002
 *
 *					22.12.2002 added support for MAX multi/sub material by
 *					making several osg::Geometry nodes for each single MAX
 *					geometry node having a multi/sub material.
 *					
 *					23.12.2002 added support for vertex colors.
 *
 *					09.01.2003 added support for MAX's directional,
 *					spot and omni light into osg::Light.
 *
 *					23.01.2003 added support for produceral 
 *					MAX animations into osg::AnimationPath. Added support
 *					for MAX camera nodes into osg::PositionAttitudeTransform.
 *
 *					24.01.2003 added support for only wrapping textures
 *					when necessary (texture coordinates above 1).
 *
 *					27.01.2003 added support for MAX cameras export into 
 *					osgfIVE::ViewPoint.
 *
 *					30.01.2003 added support for billboards. The exporter
 *					recognises a billboard in MAX by some user defined
 *					properties.
 *					
 *					07.02.2003 added support for OSG Billboard helper 
 *					objects.
 *
 *					11.02.2003 added support for OSG StateSet helper
 *					objects.
 *
 *					28.02.2003 added support for MAX shape objects, such
 *					as lines, arcs, rectangles, text, etc.
 * 
 *					26.05.2003 added support for simple particle systems.
 *
 *					21.09.2005 Joran: Fixed a bug that messed up the texture 
 *					coordinates. Also changed the vertex color generation, 
 *					hoping to prevent the same bug, but it is not tested yet.
 *
 *					21.09.2005 Joran: Fixed a texture repeating bug. Negative
 *					texture coordinates would not result in texture repeating
 *					unless values became smaller then -1.
 *
 *					22.09.2005 Joran: Fixed the shell material handling.
 *
 *                  20.01.2006 Joran: Fixed exporting of the StateSet Helper.
 *                  Because of shared stateset's, the stateset is explicitly
 *                  unshared when this helper is used.
 *
 *					27.09.2006 Farshid Lashkari: Geodes, offset transforms and
 *					drawables will now be exported with a name.
 *
 *					03.10.2007 Nathan Hanish: Fix for normal generation with the
 *					'use indices' flag set.
 * 
 *					08.01.2008: Farshid Lashkari: User modified normals are now 
 *					correctly exported. Currently, this only works for 
 *					non-indexed meshes.
 * 
 *					06.04.2009: Chris Rodgers: Cleaned up the createHelper Object
 *             method for better readability.
 */

#include <osg/Switch>
#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/MultiSegmentPlacer>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>

#include "OSGExp.h"

#include "simpobj.h" // SimpelParticle definition
#include "MeshNormalSpec.h"

/**
 * Create OSG drawables corresponding to the MAX geometry given in the node.
 * The OSG drawables will be transformed and applied with materials and
 * textures as given in the MAX scene.
 */
osg::ref_ptr<osg::MatrixTransform> OSGExp::createGeomObject(osg::Group* rootTransform, INode *node, Object* obj, TimeValue t){

	// Is this a particle system.
	if((ParticleObject*)obj->GetInterface(I_PARTICLEOBJ) && _options->getExportParticles())
		return createParticleSystemObject(rootTransform, node, obj, t);

	// The node maintains two transformations:
	// - The node's transformation matrix which is controlled by the 
	//   transform controller that places the node in the scene. 
	// - The object-offset transformation that represents a seperate position,
	//   rotation and scale orientation of the geometry of the object
	//   independent of the node. 

	// Create a transform node for the geometry.
	osg::ref_ptr<osg::MatrixTransform> nodeTransform = new osg::MatrixTransform();
   InitOSGNode(*nodeTransform, node, false);

	// Are we exporting animations.
	if(_options->getExportAnimations()){
		addAnimation(node, t, nodeTransform.get());
	}

	// Get the object-offset transformation matrix. This will be relative
	// to the node transformation matrix.
	osg::Matrix objectMat = getObjectTransform(node,t);

	// Get the node transformation matrix, this will be relative to the
	// parent node's transformation matrix.
	osg::Matrix nodeMat = getNodeTransform(node,t);

	// Create mesh from information in the node.
	osg::ref_ptr<osg::Geode> geode = createMultiMeshGeometry(rootTransform, node, obj, t);

	// Set name of geode
	if(!nodeTransform->getName().empty()) {
		geode->setName(nodeTransform->getName() + "-GEODE");
	}

	// Set the node transformation
	nodeTransform->setMatrix(nodeMat);

	// If object-offset is identity matrix then only use nodeMat in OSG.
	if(Util::isIdentity(objectMat)){
		// Add geode to node transform.
		nodeTransform->addChild(geode.get());
	}
	// Otherwise make two OSG transformation nodes and use nodeMat and 
	// objectMat.
	else{
		// Make a new OSG transform and set object-offset transform on this.
		osg::ref_ptr<osg::MatrixTransform> objectTransform = new osg::MatrixTransform(); 
	    objectTransform->setMatrix(objectMat);
		// Set static datavariance for better performance
		objectTransform->setDataVariance(osg::Object::STATIC);

		// Set name of object-offset transform
		if(!nodeTransform->getName().empty()) {
			objectTransform->setName(nodeTransform->getName() + "-OFFSET");
		}

		// Use default node mask
		if(_options->getUseDefaultNodeMaskValue())
			objectTransform->setNodeMask(_options->getDefaultNodeMaskValue());

		// Add this to nodeTransform
		nodeTransform->addChild(objectTransform.get());
		// and add geomtry to object-offset transform
		objectTransform->addChild(geode.get());
	}
	applyNodeMaskValue(node, t, nodeTransform.get());
	return nodeTransform;
}

/**
 * This method will create an OSG camera object and transform it as given 
 * by the node's transform matrix.
 */
osg::ref_ptr<osg::Transform> OSGExp::createCameraObject(osg::Group* rootTransform, INode *node, Object* obj, TimeValue t){

	// Create a viewpoint transform node for the camera.
	osg::ref_ptr<osg::PositionAttitudeTransform> nodeTransform = new osg::PositionAttitudeTransform();
   InitOSGNode(*nodeTransform, node, false);

	// Are we exporting animations.
	if(_options->getExportAnimations())
		addAnimation(node, t, nodeTransform.get());

	// Get the node transformation matrix, this will be relative to the
	// parent node's transformation matrix.
	Matrix3 nodeTM = node->GetNodeTM(t);
	Matrix3 parentTM = node->GetParentTM(t);
	nodeTM = nodeTM * Inverse(parentTM);

	// Decompose node transformation to get camera attitude.
	AffineParts ap;
	decomp_affine(nodeTM, &ap);
	Point3 pos = ap.t;
	Quat rot = ap.q;

	// Set the properties on the positions attitude transform.
	nodeTransform->setPosition(osg::Vec3(pos.x, pos.y, pos.z));
	nodeTransform->setAttitude(osg::Vec4(rot.x, rot.y, rot.z, rot.w));

	applyNodeMaskValue(node, t, nodeTransform.get());
	return nodeTransform.get();
}

/**
 * Create a light source corresponding to the information found in
 * the MAX light structure. If the light resides just below the
 * root node in MAX, we will treat it as a global light always
 * illuminating the hole scene.
 */
osg::ref_ptr<osg::MatrixTransform> OSGExp::createLightObject(osg::Group* rootTransform, INode *node, Object* obj, TimeValue t){

	// Create a transform node for the lightsource.
	osg::ref_ptr<osg::MatrixTransform> nodeTransform = new osg::MatrixTransform();
   InitOSGNode(*nodeTransform, node, false);

	// Are we exporting animations.
	if(_options->getExportAnimations())
	addAnimation(node, t, nodeTransform.get());

	// Get the node transformation matrix, this will be relative to the
	// parent node's transformation matrix.
	osg::Matrix nodeMat = getNodeTransform(node,t);

	// Set the node transformation
	nodeTransform->setMatrix(nodeMat);

	// Evaluate this light node.
	ObjectState os = node->EvalWorldState(t);
	if (!os.obj) {
		return nodeTransform;
	}

	// Get light structure from MAX.
	GenLight* sLight = (GenLight*)os.obj;
	struct LightState ls;
	Interval valid = FOREVER;
	sLight->EvalLightState(t, valid, &ls);

	// Create OSG light and set properties..
	osg::ref_ptr<osg::Light> light = new osg::Light;
   light->setName(node->GetName());
	light->setLightNum(_nLights++);
	light->setPosition(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	light->setDirection(osg::Vec3(0.0f, 0.0f, -1.0f));
	float osgR = ls.color.r*ls.intens;
	float osgG = ls.color.g*ls.intens;
	float osgB = ls.color.b*ls.intens;
	light->setDiffuse(osg::Vec4(osgR, osgG, osgB, 1.0f));
	light->setSpecular(osg::Vec4(osgR, osgG, osgB, 1.0f));

	if (ls.type == TSPOT_LIGHT || ls.type == FSPOT_LIGHT ) {
		// The cutOff value in osg ranges from 0 to 90, we need
		// to divide by 2 to avoid openGL error.
		light->setSpotCutoff(ls.fallsize/2.0f);
		// The bigger the differens is between fallsize and hotsize
		// the bigger the exponent should be.
		float diff = ls.fallsize - ls.hotsize;
		light->setSpotExponent(diff);
	}
	else if(ls.type == DIR_LIGHT) {
		// If the last value in the postion, w, is zero
		// the light is treated as directional in openGL.
		light->setPosition(osg::Vec4(0.0f, 0.0f, 1.0f, 0.0f));
	}
	else if(ls.type == OMNI_LIGHT ){
		// The omni light has no direction.
      //we remove the intensity from the rgb and store it in the spot exponent
      float osgR = ls.color.r;
      float osgG = ls.color.g;
      float osgB = ls.color.b;
      light->setDiffuse(osg::Vec4(osgR, osgG, osgB, 1.0f));
      light->setSpecular(osg::Vec4(osgR, osgG, osgB, 1.0f));
		light->setDirection(osg::Vec3(0.0f, 0.0f, 0.0f));
      light->setSpotExponent(ls.intens);//we will use the intensity as the ambient multiplier
	}
	if (ls.type != DIR_LIGHT && ls.useAtten) {
		// From MAX we have:
		//      ls.attenEnd;
		//      ls.attenStart;
		// In OSG we can set:
		//      light->setConstantAttenuation();
		//      light->setLinearAttenuation();
		//      light->setQuadraticAttenuation();
		// Don't have any idea to convert MAX attenuation
		// properties into OSG attenuation properties.
	}

	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource;
   InitOSGNode(*lightSource, NULL, false);

   lightSource->setLight(light.get());
	lightSource->setLocalStateSetModes(osg::StateAttribute::ON);
	lightSource->setStateSetModes(*(rootTransform->getStateSet()),osg::StateAttribute::ON);

	// If the MAX light is located in the root scene node we will
	// treat it as a global light in OSG and never cull it away.
	if(node->GetParentNode()->IsRootNode())
		lightSource->setCullingActive(false);

	nodeTransform->addChild(lightSource.get());
	applyNodeMaskValue(node, t, nodeTransform.get());
	return nodeTransform;
}

/**
 * This method will create shape objects such as lines, rectangles,
 * text, text ect. In OSG we will represent it with line strip
 * or line loops.
 */
osg::ref_ptr<osg::MatrixTransform> OSGExp::createShapeObject(osg::Group* rootTransform, INode* node,	Object* obj, TimeValue t){

	// Create a transform node for the shape object.
	osg::ref_ptr<osg::MatrixTransform> nodeTransform = new osg::MatrixTransform(); 
   InitOSGNode(*nodeTransform, node, false);

	// Are we exporting animations.
	if(_options->getExportAnimations())
		addAnimation(node, t, nodeTransform.get());

	// Get the node transformation matrix, this will be relative to the
	// parent node's transformation matrix.
	osg::Matrix nodeMat = getNodeTransform(node,t);

	// Set the node transformation
	nodeTransform->setMatrix(nodeMat);

	// Create geode
	osg::ref_ptr<osg::Geode> geode = createShapeGeometry(rootTransform, node, obj, t);

	// Set name of geode
	if(!nodeTransform->getName().empty()) {
		geode->setName(nodeTransform->getName() + "-GEODE");
	}

	// Create shape geometry.
	nodeTransform->addChild(geode.get());
	applyNodeMaskValue(node, t, nodeTransform.get());
	return nodeTransform;
}


/** 
 * This node will export helper objects. 
 */
osg::ref_ptr<osg::Node> OSGExp::createHelperObject(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t)
{
	Class_ID id = obj->ClassID();

   osg::ref_ptr<osg::Node> nodePtr;

   if (id == BILLBOARD_CLASS_ID)
   {
		nodePtr = createBillboardFromHelperObject(rootTransform, node, obj, t).get();
	}
	//else if (id == LOD_CLASS_ID)
   //{
	//	nodePtr = createLODFromHelperObject(rootTransform, node, obj, t).get(); 
	//}
	else if (id == SEQUENCE_CLASS_ID)
   {
		nodePtr = createSequenceFromHelperObject(rootTransform, node, obj, t).get(); 
	}
	//else if (id == SWITCH_CLASS_ID)
   //{
	//	nodePtr = createSwitchFromHelperObject(rootTransform, node, obj, t).get(); 
	//}
	else if (id == IMPOSTOR_CLASS_ID)
   {
		nodePtr = createImpostorFromHelperObject(rootTransform, node, obj, t).get(); 
	}
	else if (id == OCCLUDER_CLASS_ID)
   {
		//shouldnt this return some kind of node
		createOccluderFromHelperObject(rootTransform, node, obj, t);
      // Do not assign the pointer.
	}
	else if (id == VISIBILITYGROUP_CLASS_ID)
   {
		nodePtr = createVisibilityGroupFromHelperObject(rootTransform, node, obj, t).get(); 
	}
	//else if (id == OSGGROUP_CLASS_ID)
   //{
	//	nodePtr = createGroupFromHelper(rootTransform, node, obj, t).get(); 
	//}

   //the dof helper is handled as a special case
	//else if (id == DOFTRANSFORM_CLASS_ID)
   //{
	//	nodePtr = createDOFFromHelper(rootTransform, node, obj, t).get(); 
	//}

	return nodePtr;
}

/**
 * This method will create a particle system object.
 */
osg::ref_ptr<osg::MatrixTransform> OSGExp::createParticleSystemObject(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){
	// Create a transform node for the particle system object.
	osg::ref_ptr<osg::MatrixTransform> nodeTransform = new osg::MatrixTransform(); 
   InitOSGNode(*nodeTransform, node, false);

	// Are we exporting animations.
	if(_options->getExportAnimations())
		addAnimation(node, t, nodeTransform.get());

	// Get the node transformation matrix, this will be relative to the
	// parent node's transformation matrix.
	osg::Matrix nodeMat = getNodeTransform(node,t);

	// Need to rotate particle system 180 degree around x-axis
	osg::Matrix rot = osg::Matrix::rotate(PI, 1,0,0);
	nodeMat.preMult(rot);


	// Set the node transformation
	nodeTransform->setMatrix(nodeMat);

	// Create geode
	osg::ref_ptr<osg::Geode> geode = createParticleSystemGeometry(rootTransform, node, obj, t);

	// Set name of geode
	if(!nodeTransform->getName().empty()) {
		geode->setName(nodeTransform->getName() + "-GEODE");
	}

	// Create particle geometry.
	nodeTransform->addChild(geode.get());
	return nodeTransform;
}


/**
 * Retreive the transformation matrix from the node. The transformation matrix
 * will be relative to its parents transforms. 
 */
osg::Matrix OSGExp::getNodeTransform(INode* node, TimeValue t){

	// Get Parent and Node TMs.
	Matrix3 nodeTM = node->GetNodeTM(t);
	Matrix3 parentTM = node->GetParentTM(t);

	// Compute the relative TM.
	nodeTM = nodeTM * Inverse(parentTM);
	return convertMat(nodeTM);
}

/**
 * Retreive the object transformation matrix. The object transformation matrix
 * will be relative to the node transformation matrix, which again is relative
 * to the parent transformation matrix.
 */
osg::Matrix OSGExp::getObjectTransform(INode* node, TimeValue t){

	// Get Parent, Node and Object TMs. These are all in world space coords.
	Matrix3 nodeTM = node->GetNodeTM(t);
	Matrix3 objectTM = node->GetObjectTM(t);
	Matrix3 parentTM = node->GetParentTM(t);

	// Compute the relative node TM.
	nodeTM = nodeTM * Inverse(parentTM);

	// Compute the relative object TM
	objectTM = objectTM * Inverse(parentTM);
	objectTM = objectTM * Inverse(nodeTM);

	return convertMat(objectTM);
}

/**
 * This method will convert a MAX Matrix3 to an OSG Matrix.
 */
osg::Matrix OSGExp::convertMat(Matrix3 maxMat){

 	// Get address on elements in the MAX matrix.
	MRow* m = maxMat.GetAddr();

	// Make OSG matrix from the elements in the MAX matrix;
	osg::Matrix mat(m[0][0], m[0][1], m[0][2], 0,
					m[1][0], m[1][1], m[1][2], 0,
					m[2][0], m[2][1], m[2][2], 0,
					m[3][0], m[3][1], m[3][2], 1);
	return mat;
}

osg::ref_ptr<osg::Geode> OSGExp::createMeshGeometry(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){
	//  Geode to hold the geometry.
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
   InitOSGNode(*geode, NULL, false);

	// Order of the vertices. Get them counter clockwise if the objects is
	// negatively scaled. This is important if an object has been mirrored.
	Matrix3 tm = node->GetObjTMAfterWSM(t);
	BOOL negScale = getTMNegParity(tm);
	int vx1, vx2, vx3;
	if (negScale) {
		vx1 = 2;
		vx2 = 1;
		vx3 = 0;
	}
	else {
		vx1 = 0;
		vx2 = 1;
		vx3 = 2;
	}

	BOOL needDel;
	TriObject* tri = getTriObjectFromObject(obj, t, needDel);
	if (!tri) {
		return geode;
	}
	// Get mesh and build normals.
	Mesh* mesh = &tri->GetMesh();

	//Allocate RVertex array
	mesh->checkNormals(TRUE);

	// Break up the verts
	BitArray vertsToBreak(mesh->numVerts);
	vertsToBreak.ClearAll();
	for(int v = 0; v < mesh->numVerts; v++)
	{
		if(mesh->getRVert(v).ern)
		{
			vertsToBreak.Set(v);
		}
	}
	mesh->BreakVerts(vertsToBreak);

	// Stuff the RVertex normals into the Mesh normals array
	mesh->buildNormals();

	// Get any assigned material.
	Mtl* maxMtl = node->GetMtl();

	// Initialize some variables.
	osg::ref_ptr<osg::Geometry>		geometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array>	vertices = new osg::Vec3Array();
	osg::ref_ptr<osg::UShortArray>	vindices = new osg::UShortArray();
	osg::ref_ptr<osg::Vec3Array>	normals = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec4Array>	colors = new osg::Vec4Array();
	osg::ref_ptr<osg::UShortArray>	cindices = new osg::UShortArray();
	osg::ref_ptr<osg::StateSet>		stateset =  _mtlList->getStateSet(maxMtl);
	std::vector<int>				mappingList = _mtlList->getMappingList(stateset.get());
	std::vector<osg::ref_ptr<osg::Vec2Array> >		tcoords(mappingList.size());
	std::vector<osg::ref_ptr<osg::UShortArray> >		tcindices(mappingList.size());
	for(unsigned int unit=0; unit<mappingList.size(); unit++){
		tcoords[unit] = new osg::Vec2Array();
		tcindices[unit] = new osg::UShortArray();
	}

	// Set name of mesh to material name
	if(maxMtl) {
		geometry->setName( maxMtl->GetName() );
	}
	
	geode->addDrawable(geometry.get());
	// If there is generated any OSG stateset then assign it to the geometry.
	if(stateset.get()){
		geometry->setStateSet(stateset.get());
	}	

	// If the geometry node is referenced by a StateSet helper object
	// we will set the i'th state according to this.
	if(Util::isReferencedByHelperObject(node, STATESET_CLASS_ID)){
        // We don't want the properties on a shared stateset, so make a clone
        stateset=new osg::StateSet(*geometry->getOrCreateStateSet());
        geometry->setStateSet(stateset.get());
		applyStateSetProperties(node, t, stateset.get());
	}

	if(_options->getExportGeometry() && mesh->numVerts){
		// Handle every vertex and vertex normal in the mesh.	
		for(int v=0; v<mesh->numVerts; v++){
			// Extract every vertex from the MAX Mesh.
			Point3 p = mesh->verts[v];
			vertices->push_back(osg::Vec3(p.x, p.y, p.z));
			if(_options->getExportVertexNormals()){
				Point3 pn = mesh->getNormal(v);
				normals->push_back(osg::Vec3(pn.x, pn.y, pn.z));
			}
		}
		// Handle every vertex color in the mesh.
		if(_options->getExportVertexColors() && mesh->numCVerts){
			for(int vc=0; vc<mesh->numCVerts; vc++){
				Point3 pc = mesh->vertCol[vc];
				colors->push_back(osg::Vec4(pc.x, pc.y, pc.z, 1));
			}
		}
		// If no vertex colors or no material is assigned we use the wireframe color.
		else if(!stateset.valid()) {
			COLORREF fillcolor = node->GetWireColor();
			int r = (int) GetRValue(fillcolor);
			int g = (int) GetGValue(fillcolor);
			int b = (int) GetBValue(fillcolor);
			colors->push_back(osg::Vec4(r/255.0, g/255.0, b/255.0, 1.0));
			geometry->setColorArray(colors.get());        
			geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		}

		osg::ref_ptr<osg::TexGen> texGen = new osg::TexGen();
		// Handle texture coords
		if(_options->getExportTextureCoords()){
			for(unsigned int unit=0; unit<mappingList.size(); unit++ ){
				int mapChan = mappingList[unit];
				// If mapChan is zero this texture unit has a texture generator - do nothing.
				if(mapChan==0)
					continue;

            //we have to get the texture tiling values off of the material
            float uWrap = 1.0f;
            float vWrap = 1.0f;
              
            Texmap* tmap = maxMtl->GetSubTexmap(ID_DI);
            if (tmap && tmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) 
            {
               // Cast the texmap to a bitmap texture. 
               BitmapTex *bmt = (BitmapTex*) tmap;
               StdUVGen *uv = bmt->GetUVGen();

               if(uv != NULL)
               {
                  uWrap = uv->GetUScl(t);
                  vWrap = uv->GetVScl(t);
               }
            }

				// If there exist texture coords for this mapping channel then add them to array.
				if(mapChan>0 && mapChan<mesh->numMaps && mesh->maps[mapChan].vnum){
					BOOL wrap_s = FALSE;
					BOOL wrap_t = FALSE;
					for(int tv=0; tv<mesh->maps[mapChan].vnum;tv++){
						// The uv pair for the v'th vertex.
						UVVert uv = mesh->maps[mapChan].tv[tv];
						// uv1.x = u, uv1.y = v
                  float texCoordU = uv.x * uWrap;
                  float texCoordV = uv.y * vWrap;
						tcoords[unit]->push_back(osg::Vec2(texCoordU, texCoordV));
						if(texCoordU >1 || texCoordU <0)
							wrap_s=TRUE;
						if(texCoordV >1 || texCoordV <0)
							wrap_t=TRUE;
					}
					// If uv.x is bigger than 1 or lower than 0 then set WRAP_S mode on texture to REPEAT.
					if(wrap_s)
						_mtlList->setTextureWrap(stateset.get(), unit, osg::Texture::WRAP_S, osg::Texture::REPEAT);
					// If uv.y is bigger than 1 or lower than 0 then set WRAP_T mode on texture to REPEAT.
					if(wrap_t)
						_mtlList->setTextureWrap(stateset.get(), unit, osg::Texture::WRAP_T, osg::Texture::REPEAT);
				}
				// No texture coords exist - assign a standard texture generator.
				else{
					showErrorNoUVWMap(node, mapChan);
					if(stateset.get())
						stateset->setTextureAttributeAndModes(unit, texGen.get(), osg::StateAttribute::ON);				
				}
			}
		}

		// For every face in the MAX Mesh extract information on the indices and normals.
		for(int f=0; f<mesh->numFaces; f++){
			if(_options->getExportGeometry() && mesh->faces){
				// Do vertex indices...
				vindices->push_back(mesh->faces[f].v[vx1]);
				vindices->push_back(mesh->faces[f].v[vx2]);
				vindices->push_back(mesh->faces[f].v[vx3]);
				// Do face normals... 
				if(!_options->getExportVertexNormals()){
					Point3 pn = mesh->getFaceNormal(f);
					normals->push_back(osg::Vec3(pn.x, pn.y, pn.z));
				}
				// Do vertex color indices...
				if(_options->getExportVertexColors() && mesh->getNumVertCol() && mesh->vcFace){
					// to get 3 vertex colours for face f
					cindices->push_back(mesh->vcFace[f].t[0]);
					cindices->push_back(mesh->vcFace[f].t[1]);
					cindices->push_back(mesh->vcFace[f].t[2]);
				}
				// Do texture coords indices...
				if(_options->getExportTextureCoords()){
					for(unsigned int unit=0; unit<mappingList.size(); unit++ ){
						int mapChan = mappingList[unit];
						// If there exist texture coords for this mapping channel then add them to array.
						if(mapChan>0 && mapChan<mesh->numMaps && mesh->maps[mapChan].vnum){
							tcindices[unit]->push_back(mesh->maps[mapChan].tf[f].t[0]);
							tcindices[unit]->push_back(mesh->maps[mapChan].tf[f].t[1]);
							tcindices[unit]->push_back(mesh->maps[mapChan].tf[f].t[2]);
						}
					}
				}
			}
		}

		// Assign vertex array and indices if any.
		if(vertices->size() && vindices->size()){
			geometry->setVertexArray(vertices.get());
			geometry->setVertexIndices(vindices.get());
		}
		// Assign normal array and set normal binding if any.
		if(normals->size()){
			geometry->setNormalArray(normals.get());        
			if(_options->getExportVertexNormals() && vindices->size()){
				geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
				geometry->setNormalIndices(vindices.get());
			}
			else
				geometry->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
		}
		// Assign vertex color array and indices if any.
		if(colors->size() && cindices->size()){
			geometry->setColorArray(colors.get());
			geometry->setColorIndices(cindices.get());
			geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
		}
		// Assign texture coords arrays and indices if any.
		for(unsigned int unit=0; unit<mappingList.size(); unit++){
			if(tcoords[unit]->size() && tcindices[unit]->size()){
				geometry->setTexCoordArray(unit, tcoords[unit].get());
				geometry->setTexCoordIndices(unit, tcindices[unit].get());
			}
		}

		// Set up the primitive type,
		if(!node->GetBoxMode())
			geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, vindices->size()));
	}

	// If we need to delete the triobject then do so.
	if (needDel) {
		delete tri;
	}

	applyNodeMaskValue(node, t, geode.get());
	return geode;
}

/**
 * This method will extract any mesh information from the node and
 * create a correponding mesh in OSG.
 * For every sub materials the given node have, we will make an osg::geometry
 * object, and assign the i'th sub material to it.
 */
 osg::ref_ptr<osg::Geode> OSGExp::createMultiMeshGeometry(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t)
 {

	// If an instance of the same object has already been converted then simply
	// return the already converted instance.
    if(_options->getReferencedGeometry())
    {
       for(OSGExp::GeomList::iterator itr=_geomList.begin(); itr!=_geomList.end(); ++itr)
		   if(itr->first == obj && itr->second.first==t)
			   return itr->second.second;
    }

 
	BOOL isMultiMesh = FALSE;
	int numMeshes = 1;
	Mtl* maxMtl = node->GetMtl();
	// If it is a shell material, take the baked version
	if(maxMtl && maxMtl->ClassID()==Class_ID(BAKE_SHELL_CLASS_ID, 0)) {
		maxMtl=maxMtl->GetSubMtl(1);
	}
	// Is this a multi mesh, and have more materials assigned.
	if (maxMtl && maxMtl->ClassID() == Class_ID(MULTI_CLASS_ID, 0)){
		isMultiMesh = TRUE;
		// Get the number of meshes we need to create.
		numMeshes   = maxMtl->NumSubMtls();
	}

	// If this is not a multi mesh, then we export single mesh.
	// Notice that the export of single mesh are using indices,
	// thus we must assert that the user are exporting using
	// indices. At present time, the multi mesh export can not
	// export using indices!
	if(!isMultiMesh && _options->getUseIndices()){
		osg::ref_ptr<osg::Geode> geode = createMeshGeometry(rootTransform, node, obj, t);
		// Add geode to geomlist
		_geomList[obj] = GeodePair(t, geode);
		return geode;
	}

	//  Geode to hold the geometry.
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
   InitOSGNode(*geode, NULL, false);

	// Order of the vertices. Get them counter clockwise if the objects is
	// negatively scaled. This is important if an object has been mirrored.
	Matrix3 tm = node->GetObjTMAfterWSM(t);
	BOOL negScale = getTMNegParity(tm);
	int vx1, vx2, vx3;
	if (negScale) {
		vx1 = 2;
		vx2 = 1;
		vx3 = 0;
	}
	else {
		vx1 = 0;
		vx2 = 1;
		vx3 = 2;
	}

	BOOL needDel;
	TriObject* tri = getTriObjectFromObject(obj, t, needDel);
	if (!tri) {
		return geode;
	}
	// Get mesh and build normals.
	Mesh* mesh = &tri->GetMesh();
	mesh->buildNormals();

	// The number of mappping channel in this mesh.
	int numMaps = mesh->getNumMaps(); // When there is one texture map this value is 2!

	// This is the i'th mesh in the osg::geode.
	int i=0;

	// The j'th face of the MAX mesh.
	int j=0;

	// Allocate vector to hold the number of faces for the i'th mesh.
	std::vector<int> numFaces(numMeshes);
	// Allocate vector to hold the current face number of the i'th mesh.
	std::vector<int> f(numMeshes);
	// Allocate vector to hold the i'th mesh's material.
	std::vector<Mtl*> material(numMeshes);
	// Allocate vector to hold the i'th mesh's geometry.
	std::vector<osg::ref_ptr<osg::Geometry> > geometry(numMeshes);
	// Allocate vector to hold the state for the i'th mesh.
	std::vector<osg::ref_ptr<osg::StateSet> > state(numMeshes);
	// Allocate vector to hold vertices of the i'th mesh.
	std::vector<osg::ref_ptr<osg::Vec3Array> > coords(numMeshes);
	// Allocate vector to hold normals of the i'th mesh.
	std::vector<osg::ref_ptr<osg::Vec3Array> > normals(numMeshes);
	// Allocate vector to hold texture coordinates of the i'th mesh.
	std::vector< std::vector<osg::ref_ptr<osg::Vec2Array> > > tcoords(numMeshes);
	// Allocate vector to hold vertex colors of the i'th mesh.
	std::vector<osg::ref_ptr<osg::Vec4Array> > colors(numMeshes);
	
	// Allocate arrays to hold information on texture mapping.
	std::vector<int> numTexUnits(numMeshes);
	std::vector<std::vector<int> > mapChannels(numMeshes);

	// Initialize int arrays.
	for(i=0; i<numMeshes; i++){
		numFaces[i] = 0;
		f[i] = 0;
	}

	// Get number of faces in the i'th mesh
	for(j=0;j<mesh->numFaces;j++){
		numFaces[(mesh->faces[j]).getMatID()%numMeshes]++; 
	}

	// Initalize some arrays and set some modes.
	for(i=0;i<numMeshes;i++){
		if(isMultiMesh)
			material[i] = maxMtl->GetSubMtl(i);
		else
			material[i] = maxMtl;
		geometry[i] = new osg::Geometry();
		// Only add the i'th geometry to the geode if it has any faces
		if(numFaces[i]!=0)
			geode->addDrawable(geometry[i].get());
		state[i]    = new osg::StateSet();
		geometry[i]->setStateSet(state[i].get());

		// Set name of mesh to material name
		if(material[i]) {
			geometry[i]->setName( material[i]->GetName() );
		}

		// If there is generated any OSG stateset then set it to the i'th state.
		osg::ref_ptr<osg::StateSet> stateset = _mtlList->getStateSet(material[i]);
		if(stateset.valid()) {
			state[i] = stateset;
			geometry[i]->setStateSet(state[i].get());
		}

		// Figure out number of texture units used for each stateset.
		numTexUnits[i] = _mtlList->getNumTexUnits(stateset.get());
		mapChannels[i].resize(numTexUnits[i]);
		// Figure the used mapping channels in MAX for each stateset.
		for(int unit=0; unit<numTexUnits[i]; unit++)
			mapChannels[i][unit]=(_mtlList->getMapChannel(stateset.get(), unit));


		// If the geometry node is referenced by a StateSet helper object
		// we will set the i'th state according to this.
		if(Util::isReferencedByHelperObject(node, STATESET_CLASS_ID)){
            // We don't want the properties on a shared stateset, so make a clone
            state[i]=new osg::StateSet(*geometry[i]->getOrCreateStateSet());
            geometry[i]->setStateSet(state[i].get());
			applyStateSetProperties(node, t, state[i].get());
		}

		
		if(_options->getExportGeometry() && mesh->faces){

			// Initalize arrays for the coords and normals.
			coords[i] = new osg::Vec3Array(numFaces[i]*3);
			geometry[i]->setVertexArray(coords[i].get());
			// Set up the primitive type, 
			// index to the j'th face's vertex start and number of vertices.
			if(!node->GetBoxMode())
				geometry[i]->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,numFaces[i]*3));

			if(!_options->getExportVertexNormals()){
				normals[i] = new osg::Vec3Array();
				normals[i]->resize(numFaces[i]);
				geometry[i]->setNormalArray(normals[i].get());        
				geometry[i]->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
			}

			// Initialize arrays for the texture coords.
			if(_options->getExportTextureCoords()){
				tcoords[i].resize(numTexUnits[i]);
				// For every texture unit used in the mesh.
				for(int unit=0; unit<numTexUnits[i]; unit++){
					int mapChan = mapChannels[i][unit];
					if(mesh->maps[mapChan].vnum > 0 && mesh->maps[mapChan].tf){
						tcoords[i][unit] = new osg::Vec2Array();
						tcoords[i][unit]->resize(numFaces[i]*3);
						geometry[i]->setTexCoordArray(unit,tcoords[i][unit].get());
					}
				}
			}

			// Initialize arrays for vertex normals.
			if(_options->getExportVertexNormals() && mesh->faces){
				normals[i] = new osg::Vec3Array;
				normals[i]->resize(numFaces[i]*3);
				geometry[i]->setNormalArray(normals[i].get());        
				geometry[i]->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
			}

			// Initialize arrays for vertex colors.
			if(_options->getExportVertexColors() && mesh->getNumVertCol() > 0 && mesh->vcFace){
				colors[i] = new osg::Vec4Array;
				colors[i]->resize(numFaces[i]*3);
				geometry[i]->setColorArray(colors[i].get());        
				geometry[i]->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
			}
			// If no vertex colors or no material is assigned we use the wireframe color.
			else if(!stateset.valid()) {
				COLORREF fillcolor = node->GetWireColor();
				int r = (int) GetRValue(fillcolor);
				int g = (int) GetGValue(fillcolor);
				int b = (int) GetBValue(fillcolor);
				colors[i] = new osg::Vec4Array;
				colors[i]->push_back(osg::Vec4(r/255.0, g/255.0, b/255.0, 1.0));
				geometry[i]->setColorArray(colors[i].get());        
				geometry[i]->setColorBinding(osg::Geometry::BIND_OVERALL);
			}
		}
	}

	osg::ref_ptr<osg::TexGen> texGen = new osg::TexGen();
	// Extract coords, normals, texture coords, vertex colors, and vertex normals
	// from MAX mesh.
	for(j=0; j<mesh->getNumFaces(); j++){
		// Get the mesh ID for this face.
		int i = mesh->faces[j].getMatID()%numMeshes; 

		if(_options->getExportGeometry() && mesh->faces){

			// Set coords and face normals.
			// Get the three vertices representing the j'th face.
			Point3 v1 = mesh->verts[ mesh->faces[j].v[vx1] ];
			Point3 v2 = mesh->verts[ mesh->faces[j].v[vx2] ];
			Point3 v3 = mesh->verts[ mesh->faces[j].v[vx3] ];
			
			// Set up coords for the j'th face.
			(*coords[i])[f[i]*3].  set(v1.x, v1.y, v1.z);
			(*coords[i])[f[i]*3+1].set(v2.x, v2.y, v2.z);
			(*coords[i])[f[i]*3+2].set(v3.x, v3.y, v3.z);
				
			// Set up normal for the j'th face.
			if(!_options->getExportVertexNormals()){
				Point3 fn = mesh->getFaceNormal(j);
				(*normals[i])[f[i]].set(fn.x, fn.y, fn.z);
			}
		
			// Set texture coordinates.
			if(_options->getExportTextureCoords()){



				// For every texture unit in the stateset.
				for(int unit=0; unit<numTexUnits[i]; unit++){
					int mapChan = mapChannels[i][unit];
					// If mapChan is zero this texture unit has a texture generator - do nothing.
					if(mapChan==0)
						continue;

               //get texture tiling 
               float uWrap = 1.0f;
               float vWrap = 1.0f;
               Texmap* tmap = maxMtl->GetSubTexmap(ID_DI);
               if (tmap && tmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) 
               {
                  // Cast the texmap to a bitmap texture. 
                  BitmapTex *bmt = (BitmapTex*) tmap;
                  StdUVGen *uv = bmt->GetUVGen();

                  if(uv != NULL)
                  {
                     uWrap = uv->GetUScl(t);
                     vWrap = uv->GetVScl(t);
                  }
               }

					if(mapChan>0 && mapChan < mesh->numMaps && mesh->maps[mapChan].vnum && mesh->maps[mapChan].tf){
						BOOL wrap_s = FALSE;
						BOOL wrap_t = FALSE;

						// The 3 uv pairs for the j'th face is in the mp'th map:
						UVVert uv1 = mesh->maps[mapChan].tv[ mesh->maps[mapChan].tf[j].t[vx1] ];
						UVVert uv2 = mesh->maps[mapChan].tv[ mesh->maps[mapChan].tf[j].t[vx2] ];
						UVVert uv3 = mesh->maps[mapChan].tv[ mesh->maps[mapChan].tf[j].t[vx3] ];

						// uv1.x = u, uv1.y = v
						(*tcoords[i][unit])[f[i]*3  ].set(uv1.x * uWrap, uv1.y * vWrap);
						(*tcoords[i][unit])[f[i]*3+1].set(uv2.x * uWrap, uv2.y * vWrap);
						(*tcoords[i][unit])[f[i]*3+2].set(uv3.x * uWrap, uv3.y * vWrap);

						// If uv.x is bigger than 1 or lower than 0 then set WRAP_S mode on texture to REPEAT.
						if(uv1.x * uWrap >1 || uv1.x  * uWrap <0 || uv2.x * uWrap >1 || uv2.x * uWrap <0 || uv3.x * uWrap >1 || uv3.x * uWrap <0)
							_mtlList->setTextureWrap(state[i].get(), unit, osg::Texture::WRAP_S, osg::Texture::REPEAT);
						// If uv.y is bigger than 1 or lower than 0 then set WRAP_T mode on texture to REPEAT.
						if(uv1.y * vWrap >1 || uv1.y * vWrap <0 || uv2.y * vWrap >1 || uv2.y * vWrap <0 || uv3.y * vWrap >1 || uv3.y * vWrap <0)
							_mtlList->setTextureWrap(state[i].get(), unit, osg::Texture::WRAP_T, osg::Texture::REPEAT);
					}
					// No texture coords exist - assign a standard texture generator.
					else{
						showErrorNoUVWMap(node, mapChan);
						if(state[i].valid())
							state[i]->setTextureAttributeAndModes(unit, texGen.get(),osg::StateAttribute::ON);				
					}
				}
			}
			
			// Set vertex normals.
			if(_options->getExportVertexNormals() && mesh->faces){
				Point3 vn1 = getVertexNormal(mesh, j, vx1);
				Point3 vn2 = getVertexNormal(mesh, j, vx2);
				Point3 vn3 = getVertexNormal(mesh, j, vx3);
				(*normals[i])[f[i]*3  ].set(vn1.x, vn1.y, vn1.z);
				(*normals[i])[f[i]*3+1].set(vn2.x, vn2.y, vn2.z);
				(*normals[i])[f[i]*3+2].set(vn3.x, vn3.y, vn3.z);
			}

			// Set Vertex colors.
			if(_options->getExportVertexColors() && mesh->getNumVertCol() > 0 && mesh->vcFace){
				// to get 3 vertex colours for face i
				Point3 c1 = mesh->vertCol[ mesh->vcFace[j].t[vx1] ];
				Point3 c2 = mesh->vertCol[ mesh->vcFace[j].t[vx2] ];
				Point3 c3 = mesh->vertCol[ mesh->vcFace[j].t[vx3] ];
				(*colors[i])[f[i]*3  ].set(c1.x, c1.y, c1.z, 1);
				(*colors[i])[f[i]*3+1].set(c2.x, c2.y, c2.z, 1);
				(*colors[i])[f[i]*3+2].set(c3.x, c3.y, c3.z, 1);
			}
		}
		// Indicate that the we have handled yet another face of the i'th mesh.
		f[i]++;
	}


	// If we need to delete the triobject then do so.
	if (needDel) {
		delete tri;
	}

	// Add geode to geomlist
	_geomList[obj]=GeodePair(t,geode);
	
	applyNodeMaskValue(node, t, geode.get());
	return geode;
}

/**
 * This method will create shape geometry as lines, rectangles, text, etc.
 * In OSG we will represent it with line strip or line loops. If the shape
 * is rendered as a mesh in MAX then we will create a mesh instead.
 */
 osg::ref_ptr<osg::Geode> OSGExp::createShapeGeometry(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){

	ShapeObject* shape = (ShapeObject*)obj;

	if(shape->GetDispRenderMesh()){
		return createMultiMeshGeometry(rootTransform, node, obj, t);
	}

	// We will output shapes as a collection of polylines.
	// Each polyline contains collection of line segments.
	PolyShape pShape;
	shape->MakePolyShape(t, pShape);
	int numLines = pShape.numLines;
	
	// Create a geode to hold all the primitives.
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
   InitOSGNode(*geode, NULL, false);

	// For every line object in the polyshape we will make a geode.
	for(int l = 0; l< numLines; l++){
		PolyLine* line = &pShape.lines[l];
		int numPts = line->numPts;

	    // Create a geometry object and vertex array to store all the primitives in.
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(numPts);
		// Pass the created vertex array to the geometry object.
		geometry->setVertexArray(vertices.get());
		// Add the geometry to a geode.
		geode->addDrawable(geometry.get());

		// Set name of mesh to material name
		if(node->GetMtl()) {
			geometry->setName( node->GetMtl()->GetName() );
		}

		// If there is generated any OSG stateset then add it to the geometry.
		osg::ref_ptr<osg::StateSet> stateset = _mtlList->getStateSet(node->GetMtl());
		if(stateset.valid())
			geometry->setStateSet(stateset.get());
		else{
			stateset = new osg::StateSet();
			geometry->setStateSet(stateset.get());
		}

		// If the geometry node is referenced by a StateSet helper object
		// we will set the state according to this.
		if(Util::isReferencedByHelperObject(node, STATESET_CLASS_ID)){
            // We don't want the properties on a shared stateset, so make a clone
            stateset=new osg::StateSet(*geometry->getOrCreateStateSet());
            geometry->setStateSet(stateset.get());
			applyStateSetProperties(node, t, stateset.get());
		}

		// For every point on the line ...
		for(int p=0; p<numPts; p++){
			PolyPt* pt = &line->pts[p];
			// add it to the vertex array.
			(*vertices)[p].set(pt->p.x, pt->p.y, pt->p.z);
		}

		// If this shape represents a spline, helix or arc it should be drawn as a line strip.
		if(shape->ClassID() == Class_ID(SPLINE3D_CLASS_ID,0) ||
			shape->ClassID() == Class_ID(HELIX_CLASS_ID,0) ||
			shape->ClassID() == Class_ID(ARC_CLASS_ID,0) )
			geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, numPts));
		else
			// Everything else is drawn with line loops.
			geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, numPts));

		// Set the overall normal to point in positive z-axis direction
		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		normals->push_back(osg::Vec3(0.0f,0.0f, 1.0f));
		geometry->setNormalArray(normals.get());
		geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	}// End for every line in polyshape

	// Return geode
	return geode;
}

#define PB_VPTPARTICLES		0
#define PB_RNDPARTICLES		1
#define PB_DROPSIZE			2
#define PB_SPEED			3
#define PB_VARIATION		4
#define PB_DISPTYPE			5
#define PB_STARTTIME		6
#define PB_LIFETIME			7
#define PB_EMITTERWIDTH		8
#define PB_EMITTERHEIGHT	9
#define PB_HIDEEMITTER		10
#define PB_BIRTHRATE		11
#define PB_CONSTANT			12
#define PB_RENDER			13
#define PB_TUMBLE			14
#define PB_SCALE			15
osg::ref_ptr<osg::Geode> OSGExp::createParticleSystemGeometry(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){

	// Is this a spray or snow particle system.
	if(obj->ClassID() == Class_ID(RAIN_CLASS_ID,0) || obj->ClassID() == Class_ID(SNOW_CLASS_ID,0) ){
	
		// Get values from parameterblock.
		IParamBlock* pblock = ((SimpleParticle*)(obj))->pblock;
		Interval iv;
		// Only support version 3.
		int version = pblock->GetVersion();
		if(version!=3)
			return NULL;
		int vptCount;	 		// Viewport particles count
		int rndCount;			// Render particles count
		float dropSize;			// Particle size
		float speed;
		float variation;
		int dispType;
		TimeValue startTime;
		TimeValue lifeTime;
		float emitterWidth;
		float emitterHeight;
		int hideEmitter;
		float birthRate;
		int constantBirthRate;
		int render;
		float tumble;
		float tumbleScale;

		pblock->GetValue(PB_VPTPARTICLES, t, vptCount, iv);
		pblock->GetValue(PB_RNDPARTICLES, t, rndCount, iv);
		pblock->GetValue(PB_DROPSIZE, t, dropSize, iv);
		pblock->GetValue(PB_SPEED, t, speed, iv);
		pblock->GetValue(PB_VARIATION, t, variation, iv);
		pblock->GetValue(PB_DISPTYPE, t, dispType, iv);
		pblock->GetValue(PB_STARTTIME, t, startTime, iv);
		pblock->GetValue(PB_LIFETIME, t, lifeTime, iv);
		pblock->GetValue(PB_EMITTERWIDTH, t, emitterWidth, iv);
		pblock->GetValue(PB_EMITTERHEIGHT, t, emitterHeight, iv);
		pblock->GetValue(PB_HIDEEMITTER, t, hideEmitter, iv);
		pblock->GetValue(PB_BIRTHRATE, t, birthRate, iv);
		pblock->GetValue(PB_CONSTANT, t, constantBirthRate, iv);
		pblock->GetValue(PB_RENDER, t, render, iv);
		if(obj->ClassID() == Class_ID(SNOW_CLASS_ID,0)){
			pblock->GetValue(PB_TUMBLE, t, tumble, iv);
			pblock->GetValue(PB_SCALE, t, tumbleScale, iv);
		}

		// If there is generated any OSG stateset for the system, then extract information.
		osg::ref_ptr<osg::StateSet> stateset = _mtlList->getStateSet(node->GetMtl());
		osg::Vec4 diffuse;
		osg::ref_ptr<osg::Texture> osgTex;
		int texUnit = 0;

		if(stateset.valid()){
			// Extract diffuse color from stateset.
			osg::Material* osgMtl = (osg::Material*) stateset->getAttribute(osg::StateAttribute::MATERIAL);
			if(osgMtl){
				diffuse = osgMtl->getDiffuse(osg::Material::FRONT_AND_BACK);
			}
			// Extract texture from stateset.
			osgTex =_mtlList->getTexture(stateset.get(), texUnit);
		}

		// Create particle template.
		osgParticle::Particle ptemplate;
		ptemplate.setLifeTime(((float)lifeTime/(float)GetTicksPerFrame()));		// lifetime in seconds.
		// Spray type:
		if(obj->ClassID() == Class_ID(RAIN_CLASS_ID,0)){
			switch(render){
				// TetraHedron
				case 0:
					ptemplate.setShape(osgParticle::Particle::HEXAGON);
					break;
				// Facing
				case 1:
					ptemplate.setShape(osgParticle::Particle::QUAD);
					break;
			}
		}
		// Snow type:
		if(obj->ClassID() == Class_ID(SNOW_CLASS_ID,0)){
			switch(render){
				case 0:
					ptemplate.setShape(osgParticle::Particle::HEXAGON);
					break;
				case 1:
					ptemplate.setShape(osgParticle::Particle::QUAD);
					break;
				case 2:
					ptemplate.setShape(osgParticle::Particle::QUAD);
					break;
			}
		}
		// Size and radius of particle.
		ptemplate.setSizeRange(osgParticle::rangef(dropSize/2.0f, dropSize));
		ptemplate.setRadius(1.0f/dropSize);//  width of particles in cm
		//ptemplate.setAlphaRange(osgParticle::rangef(0.0f, 1.5f));
		ptemplate.setColorRange(osgParticle::rangev4(diffuse,diffuse)); 


		// Create particle system.
		osgParticle::ParticleSystem *ps = new osgParticle::ParticleSystem;
//		ps->setDefaultAttributes(osgTex.get(), true, false, texUnit);
		// Assign the particle template to the system.
		ps->setDefaultParticleTemplate(ptemplate);

		// Create emmiter.
		osgParticle::ModularEmitter *emitter = new osgParticle::ModularEmitter;
		emitter->setParticleSystem(ps);
		// Setup the counter.
		osgParticle::RandomRateCounter *counter = new osgParticle::RandomRateCounter;
		counter->setRateRange(rndCount, rndCount); // generate rndCount to rndCount particles per second
		emitter->setCounter(counter);
		// Setup the placer. It will be a rectangle of emitterWidth*emitterHeight.
		osgParticle::MultiSegmentPlacer *placer = new osgParticle::MultiSegmentPlacer ;
		placer->addVertex(emitterWidth/2.0f, -emitterHeight/2.0f , 0);
		placer->addVertex(emitterWidth/2.0f, emitterHeight/2.0f , 0);
		placer->addVertex(-emitterWidth/2.0f, emitterHeight/2.0f , 0);
		placer->addVertex(-emitterWidth/2.0f, -emitterHeight/2.0f , 0);
		placer->addVertex(emitterWidth/2.0f, -emitterHeight/2.0f , 0);
		emitter->setPlacer(placer);
		// Setup shooter.
		osgParticle::RadialShooter *shooter = new osgParticle::RadialShooter;
		speed = speed*9.81f;
		shooter->setInitialSpeedRange(speed/2.0f, speed);
		float theta = variation * (PI/30.0f);
		shooter->setThetaRange(0, theta);
		emitter->setShooter(shooter);
		// Add emitter to rootTransform.
		rootTransform->addChild(emitter);

		// Create particle system updater and set particle system.
		osgParticle::ParticleSystemUpdater *psu = new osgParticle::ParticleSystemUpdater;
		psu->addParticleSystem(ps);

		// Add the updater node to the scene graph.
		rootTransform->addChild(psu);

		// Add particle system to geode.
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;

		// Use default node mask
		if(_options->getUseDefaultNodeMaskValue())
			geode->setNodeMask(_options->getDefaultNodeMaskValue());

		// Set name of mesh to material name
		if(node->GetMtl()) {
			ps->setName( node->GetMtl()->GetName() );
		}

		geode->addDrawable(ps);
		return geode;
	
	}
	return NULL;
}

/**
 * Determine is the node has negative scaling.
 * This is used for mirrored objects for example. They have a negative scale factor
 * so when calculating the normal we should take the vertices counter clockwise.
 * If we don't compensate for this the objects will be 'inverted'.
 */
BOOL OSGExp::getTMNegParity(Matrix3 &m)
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}

/**
 * Return a pointer to a TriObject given an INode or return NULL
 * if the node cannot be converted to a TriObject.
 */
TriObject* OSGExp::getTriObjectFromObject(Object* obj, TimeValue t, int &deleteIt)
{
	deleteIt = FALSE;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) { 
		TriObject *tri = (TriObject *) obj->ConvertToType(t, 
			Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri) deleteIt = TRUE;
		return tri;
	}
	else {
		return NULL;
	}
}

/**
 * Return a vertex normal.
 */
Point3 OSGExp::getVertexNormal(Mesh* mesh, int faceNo, int vert){
	Face* f = &mesh->faces[faceNo];
	RVertex *rv = mesh->getRVertPtr(f->getVert(vert));
	DWORD smGroup = f->smGroup;
	int numNormals;
	Point3 vertexNormal;
	
	//Check for explicit normals (i.e. Edit Normals modifier)
	MeshNormalSpec* meshNorm = mesh->GetSpecifiedNormals();
	if(meshNorm && meshNorm->GetNumFaces()) {
		int normID = meshNorm->Face(faceNo).GetNormalID(vert);
		if(meshNorm->GetNormalExplicit(normID)) {
			return meshNorm->Normal(normID);
		}
	}

	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL) {
		vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) {
		// If there is only one vertex is found in the rn member.
		if (numNormals == 1) {
			vertexNormal = rv->rn.getNormal();
		}
		else {
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++) {
				if (rv->ern[i].getSmGroup() & smGroup) {
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else {
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = mesh->getFaceNormal(faceNo);
	}
	
	return vertexNormal;

}


void OSGExp::showErrorNoUVWMap(INode* node, int mapChan){
	if(_hasShownError || !_options->getShowErrMsg())
		return;

	char buf[500];
	sprintf(buf,"No UVW coordinates found in Map Channnel %d when exporting %s.\nYou may want to add an UVW Map to this channel!",
				mapChan, node->GetName());
	MessageBox (GetActiveWindow(), buf, TEXT("Warning"), MB_OK | MB_ICONWARNING) ;
	_hasShownError = TRUE;
}

