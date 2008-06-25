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
 *	FILE:			OSGHelpers.cpp
 *
 *	DESCRIPTION:	Methods to create and apply user defined stuff
 *					from helper objects in 3ds max.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 29.01.2003
 *
 *					22.09.2005 Joran: Fixed a bug in the normal generation
 *					for billboards.
 *
 *                  04.10.2005 Joran: Added exporting point helpers.
 *
 *                  12.10.2005 Joran: Modified stateset helper to a super stateset
 *                  helper.
 *
 *                  14.10.2005 Boto: Added Use Alpha Channel to stateset helper.
 *
 *					22.6.2006 Anderegg: Added osg::Group and osgSim::DOFTransform support
 */
 
#include "OSGExp.h"
#include <osg/OccluderNode>
#include <osg/PagedLOD>
#include <osg/Point>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Switch>
#include <osgSim/DOFTransform>

#include <crtdbg.h>

/**
 * Method to handle nodes referenced by an OSG Helper NodeMask Object.
 * The method will apply a user defined nodemask value to an OSG::Node.
 */
void OSGExp::applyNodeMaskValue(INode* node, TimeValue t, osg::Node* osgNode){
	IParamBlock2* pblock2;
	RefList refList;

	// Check to see if the node or the node's parent is referenced by an OSG Helper NodeMask object.
	if(pblock2 = Util::isReferencedByHelperObject(node, NODEMASK_CLASS_ID)){
		// Get properties from OSG helper NodeMask.
		Interval iv;
		int mode;
		unsigned int value;
		TCHAR* pValue;
		pblock2->GetValue(nodemask_mode, t, mode, iv);
		// Swith the nodemask modes
		switch(mode){
			case 0:		// Terrain
				value = 0xFF000000;
				break;
			case 1:		// Collision
				value = 0x00FF0000;
				break;
			case 2:		// No Collision
				value = 0x0000FFFF;
				break;
			case 3:		// Custom; get entered value
				pblock2->GetValue(nodemask_value, t, pValue, iv);
				value = Util::getUInt(pValue);
				break;
			}
		// Set NodeMask value.
		osgNode->setNodeMask(value);
		// If the osgNode is as group we should also set its children's nodemasks.
		if(dynamic_cast<osg::Group*>(osgNode)){
			osg::Group* group = static_cast<osg::Group*>(osgNode);
			for(unsigned int i = 0; i < group->getNumChildren(); i++)
				group->getChild(i)->setNodeMask(value);
		}
	}
}

/**
 * Method to handle nodes referenced by an OSG Helper StateSet Object.
 * The method will apply user defined properties to an OSG::StateSet.
 */
void OSGExp::applyStateSetProperties(INode* node, TimeValue t, osg::StateSet* stateset){
	if(!stateset)
		return;

	IParamBlock2* pblock2;
	RefList refList;

	// Assure the node is referenced by an OSG helper StateSet object.
	refList = node->GetRefList();
	RefListItem* ptr = refList.first;
	while (ptr){
		if(ptr->maker){
			SClass_ID sid = ptr->maker->SuperClassID();
			if(sid==PARAMETER_BLOCK2_CLASS_ID){
				pblock2 = (IParamBlock2*) ptr->maker;
				ClassDesc2 * desc = pblock2->GetDesc()->cd;
				if(desc){
					Class_ID id  = desc->ClassID();
					if(id == STATESET_CLASS_ID){
						applyStateSetRenderBinProperties(pblock2,t,stateset);
                        applyStateSetGLModeProperties(pblock2,t,stateset);
                        applyStateSetPolygonModeProperties(pblock2,t,stateset);
                        applyStateSetPolygonOffsetProperties(pblock2,t,stateset);
						break;
					}
				}
			}
		}
		ptr = ptr->next;
	}
}

void OSGExp::applyStateSetPolygonModeProperties(IParamBlock2* pblock2, TimeValue t, osg::StateSet* stateset){
    Interval iv;
    BOOL enabled;

    pblock2->GetValue(polygonmode_enable,t,enabled,iv);
    if(enabled) {
        int frontface;
        int backface;

        pblock2->GetValue(polygonmode_frontface,t,frontface,iv);
        pblock2->GetValue(polygonmode_backface,t,backface,iv);

        osg::PolygonMode *pm=new osg::PolygonMode;
        pm->setDataVariance(osg::Object::STATIC);
        switch(frontface) {
            case 0: pm->setMode(osg::PolygonMode::FRONT,osg::PolygonMode::FILL);break;
            case 1: pm->setMode(osg::PolygonMode::FRONT,osg::PolygonMode::LINE);break;
            case 2: pm->setMode(osg::PolygonMode::FRONT,osg::PolygonMode::POINT);break;
        }
        switch(backface) {
            case 0: pm->setMode(osg::PolygonMode::BACK,osg::PolygonMode::FILL);break;
            case 1: pm->setMode(osg::PolygonMode::BACK,osg::PolygonMode::LINE);break;
            case 2: pm->setMode(osg::PolygonMode::BACK,osg::PolygonMode::POINT);break;
        }

        stateset->setAttributeAndModes(pm);
    }
}

void OSGExp::applyStateSetPolygonOffsetProperties(IParamBlock2* pblock2, TimeValue t, osg::StateSet* stateset){
    Interval iv;
    BOOL enabled;

    pblock2->GetValue(polygonoffset_enable,t,enabled,iv);
    if(enabled) {
        float factor;
        float unit;
        BOOL depthFunct;

        pblock2->GetValue(polygonoffset_factor,t,factor,iv);
        pblock2->GetValue(polygonoffset_unit,t,unit,iv);
        pblock2->GetValue(polygonoffset_depthfunc,t,depthFunct,iv);

        osg::PolygonOffset *po=new osg::PolygonOffset;
        po->setFactor(factor);
        po->setUnits(unit);
        stateset->setAttributeAndModes(po,osg::StateAttribute::ON);

        if(depthFunct) {
            osg::Depth *depth=new osg::Depth;
            depth->setFunction(osg::Depth::LEQUAL);
            stateset->setAttributeAndModes(depth,osg::StateAttribute::ON);
        }
    }
}

void OSGExp::applyStateSetGLModeProperties(IParamBlock2* pblock2, TimeValue t, osg::StateSet* stateset){
    Interval iv;
    BOOL cullface;
	BOOL normalize;
	BOOL turnofflighting;
    BOOL usealphablending;

    pblock2->GetValue(glmode_cullface, t, cullface, iv);
	pblock2->GetValue(glmode_normalize, t, normalize, iv);
	pblock2->GetValue(glmode_turnofflighting, t, turnofflighting, iv);
    pblock2->GetValue(glmode_alphablending, t, usealphablending, iv);

	if(cullface) stateset->setMode(GL_CULL_FACE,osg::StateAttribute::ON);
	else stateset->setMode(GL_CULL_FACE,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);

	if(normalize) stateset->setMode(GL_NORMALIZE,osg::StateAttribute::ON);
	else stateset->setMode(GL_NORMALIZE,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);

	if(turnofflighting) stateset->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
	else stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);

    if(usealphablending) MtlKeeper::addAlphaBlendFunc(stateset);
}

void OSGExp::applyStateSetRenderBinProperties(IParamBlock2* pblock2, TimeValue t, osg::StateSet* stateset){
    Interval iv;
    BOOL renderBinEnabled;

    pblock2->GetValue(render_bin_enable, t, renderBinEnabled, iv);
    if(renderBinEnabled) {
	    int renderBinNum;
	    TCHAR* renderBinName;
	    int renderBinMode;

        pblock2->GetValue(render_bin_num, t, renderBinNum, iv);
	    pblock2->GetValue(render_bin_name, t, renderBinName, iv);
	    pblock2->GetValue(render_bin_mode, t, renderBinMode, iv);

		switch(renderBinMode){
		case 0:
			stateset->setRenderBinDetails(renderBinNum, std::string(renderBinName), osg::StateSet::INHERIT_RENDERBIN_DETAILS);
			break;
		case 1:
			stateset->setRenderBinDetails(renderBinNum, std::string(renderBinName), osg::StateSet::USE_RENDERBIN_DETAILS);
			break;
		case 2:
			stateset->setRenderBinDetails(renderBinNum, std::string(renderBinName), osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
			break;
		case 3:
            //apparently this has been removed from osg but is still supported as the default case, so this will still work
            //even though it is commented out, and so we leave it as an option in the dialog box
            //http://osgcvs.no-ip.org/osgarchiver/archives/August2006/0286.html
            //B.Anderegg

			//stateset->setRenderBinDetails(renderBinNum, std::string(renderBinName), osg::StateSet::ENCLOSE_RENDERBIN_DETAILS);
			break;
		}
    }
}


/**
 * This method will create an OSG billboard defined by an OSG
 * billboard helper object. 
 */
osg::ref_ptr<osg::Billboard> OSGExp::createBillboardFromHelperObject(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){
	// Make the billboard from properties in helper object.
	osg::ref_ptr<osg::Billboard> bilbo = new osg::Billboard();
	bilbo->setName(node->GetName());
	// Set static datavariance for better performance
	bilbo->setDataVariance(osg::Object::STATIC);
	// Use default node mask
	if(_options->getUseDefaultNodeMaskValue())
		bilbo->setNodeMask(_options->getDefaultNodeMaskValue());

	// The billboard properties are saved in a parameter block.
	IParamBlock2* pblock2 = (IParamBlock2*)obj->GetParamBlock();
	Interval iv;
	int mode;
	osg::Vec3 axis;
	osg::Vec3 normal;

	if(pblock2){
		pblock2->GetValue(bilbo_mode, t, mode, iv);
		pblock2->GetValue(bilbo_axis_x, t, axis[0], iv);
		pblock2->GetValue(bilbo_axis_y, t, axis[1], iv);
		pblock2->GetValue(bilbo_axis_z, t, axis[2], iv);
		pblock2->GetValue(bilbo_normal_x, t, normal[0], iv);
		pblock2->GetValue(bilbo_normal_y, t, normal[1], iv);
		pblock2->GetValue(bilbo_normal_z, t, normal[2], iv);
	}
	// Set properties on OSG billboard.
	switch(mode){
	case 0:	
		bilbo->setMode(osg::Billboard::AXIAL_ROT);
		break;
	case 1:
		bilbo->setMode(osg::Billboard::POINT_ROT_WORLD);
		break;
	case 2:
		bilbo->setMode(osg::Billboard::POINT_ROT_EYE);
		break;
	}
	bilbo->setAxis(axis);
	bilbo->setNormal(normal);

	// Add geometry to OSG billboard.
	if(pblock2){
		for(int n=0; n<pblock2->Count(bilbo_nodes); n++){
			INode* bilboNode = pblock2->GetINode(bilbo_nodes, t, n);
			traverseBillboardNodes(rootTransform, bilbo.get(), bilboNode, t);
		}
	}

	// Return OSG billboard.
	return bilbo;
}

/**
 * This method will allow for adding groups of geometry to the billboard helper object.
 * Billboard geoemtry in OSG must be added directly to the rootscene - no matrixtransforms, 
 * groups or the like are allowed. This method will extract any geometry and its position
 * from any max geometry node or group node. The resulting geometry with position will be added
 * to the given billboard object.
 */
void OSGExp::traverseBillboardNodes(osg::Group* rootTransform, osg::Billboard* bilbo, INode* node, TimeValue t){
	if(node == NULL)
		return;
	// Is this a group node.
	if(node->IsGroupHead()){
		// Traverse and add children.
		for (int c = 0; c < node->NumberOfChildren(); c++)
			traverseBillboardNodes(rootTransform, bilbo, node->GetChildNode(c), t);
	}
	else{
		// Is this a geometry object.
		ObjectState os = node->EvalWorldState(t); 
		if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID ){
			// Get the absolute position of geometry.
			osg::Matrix mat = convertMat(node->GetObjTMBeforeWSM(t));
	
			// Create the node's geometry.
			osg::ref_ptr<osg::Geode> geode = createMultiMeshGeometry(rootTransform, node, os.obj, t);
					
			// MAX Geometry are exported as OSG geodes.
			if(geode.valid()){
				// Get drawables from geode. We know this is always
				// a _single_geometry_ node, because it was create in the
				// createMultiMesh method. 
				osg::Geometry* geometry = (osg::Geometry*)geode->getDrawable(0);
				// Rotate, scale the geometry and get its position.
				osg::Vec3 pos = transformBillboardGeometry(geometry, mat);
				// Add it to the billboard,
				bilbo->addDrawable(geometry);
				// Set the position.
				bilbo->setPosition(bilbo->getNumDrawables()-1,pos);
			}
		}
	}
}

/** 
 * This method will transform the given geometry's vertices
 * and normals by the given matrix.
 */
osg::Vec3 OSGExp::transformBillboardGeometry(osg::Geometry* geometry, osg::Matrix mat){

	// Rotate and scale every vertex to absolute coordinates.
	// The geometry will still be positioned in its local coordinate system.
	osg::Matrix cMat(mat);
	cMat.setTrans(0,0,0);
	osg::Vec3Array* coords = static_cast<osg::Vec3Array*>(geometry->getVertexArray());
	if(coords){
		int numCoords = coords->getNumElements();
		for(int i=0;i<numCoords;i++){
			osg::Vec3 coord = (*coords)[i];
			coord = cMat.preMult(coord);
			(*coords)[i].set(coord.x(), coord.y(), coord.z());
		
		}
	}
	
	// Transform every normal to absolute coordinates.
	osg::Matrix nMat(mat);
	nMat.setTrans(0,0,0);
	osg::Vec3Array* normals = (osg::Vec3Array*)geometry->getNormalArray();
	if(normals){
		int numNormals = normals->getNumElements();
		for(int i=0;i<numNormals;i++){
			osg::Vec3 normal = (*normals)[i];
			normal = nMat.preMult(normal);
			normal.normalize();
			(*normals)[i].set(normal.x(), normal.y(), normal.z());
		}
	}
	return mat.getTrans();
}

/**
 * This method will create an osg::(Paged)LOD defined by an OSG
 * LOD helper object. 
 */
osg::ref_ptr<osg::LOD> OSGExp::createLODFromHelperObject(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){

	// The lod properties are saved in a parameter block.
	IParamBlock2* pblock2 = (IParamBlock2*)obj->GetParamBlock();
	Interval iv;
	osg::Vec3 center;
	BOOL usePagedLOD;
	TCHAR* templateFilename;

	if(pblock2){
		pblock2->GetValue(lod_center_x, t, center[0], iv);
		pblock2->GetValue(lod_center_y, t, center[1], iv);
		pblock2->GetValue(lod_center_z, t, center[2], iv);
		pblock2->GetValue(lod_usepaged, t, usePagedLOD, iv);
		pblock2->GetValue(lod_paged_filename, t, templateFilename, iv);
	}

	// Make the LOD from properties in helper object.
	osg::ref_ptr<osg::LOD> lod;
	if(usePagedLOD)
		lod = new osg::PagedLOD();
	else
		lod = new osg::LOD();
	lod->setName(node->GetName());
	// Set static datavariance for better performance
	lod->setDataVariance(osg::Object::STATIC);
	// Use default node mask
	if(_options->getUseDefaultNodeMaskValue())
		lod->setNodeMask(_options->getDefaultNodeMaskValue());

	// Set properties on OSG lod.
	lod->setCenter(center);

	// Add geometry to lod.
	if(pblock2){
		for(int n=0; n<NUM_LOD_OBJECTS; n++){
			// Get min and max values from helper object.
			float min;
			float max;
			pblock2->GetValue(lod_min, t, min, iv, n);
			pblock2->GetValue(lod_max, t, max, iv, n);

			INode* lodGeomNode = pblock2->GetINode(lod_node+n, t);
			osg::ref_ptr<osg::Node> transform = createGeomObjectForHelperObjects(rootTransform, lodGeomNode, obj, t);
			if(transform.valid()){
				// If use paged LOD
				if(usePagedLOD && dynamic_cast<const osg::PagedLOD*>(lod.get())){
					char filename[500];
					sprintf(filename,"%s_%d%s", templateFilename,
												n,
												_options->getExportExtension().c_str());
					osg::PagedLOD* pagedLOD = (osg::PagedLOD*)(lod.get());
					pagedLOD->addChild(transform.get(), min, max);
					pagedLOD->setFileName(n,filename);
					std::string absoluteFilename = std::string(_options->getExportPath());
					absoluteFilename.append(filename);
					osgDB::writeNodeFile(*transform, absoluteFilename);
				}
				// or use standard LODs
				else{
					lod->addChild(transform.get(), min, max);
				}
			}
		}// End for number of LOD objects in pblock2
	}

	// Return OSG LOD.
	return lod;
}

/**
 * This method will create an osg::Sequence object from the 
 * properties defined in an OSG Sequence helper object. 
 */
osg::ref_ptr<osg::Sequence> OSGExp::createSequenceFromHelperObject(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){
	// Make the LOD from properties in helper object.
	osg::ref_ptr<osg::Sequence> seq = new osg::Sequence();
	seq->setName(node->GetName());
	// Set static datavariance for better performance
	seq->setDataVariance(osg::Object::STATIC);
	// Use default node mask
	if(_options->getUseDefaultNodeMaskValue())
		seq->setNodeMask(_options->getDefaultNodeMaskValue());


	// The sequence properties are saved in a parameter block.
	IParamBlock2* pblock2 = (IParamBlock2*)obj->GetParamBlock();
	Interval iv;
	float begin;
	float end;
	int loop;
	float speed;
	int reps;
	int mode;
	int approach;
	if(pblock2){
		pblock2->GetValue(sequence_begin, t, begin, iv);
		pblock2->GetValue(sequence_end, t, end, iv);
		pblock2->GetValue(sequence_loop, t, loop, iv);
		pblock2->GetValue(sequence_speed, t, speed, iv);	
		pblock2->GetValue(sequence_reps, t, reps, iv);	
		pblock2->GetValue(sequence_mode, t, mode, iv);
		pblock2->GetValue(sequence_approach, t, approach, iv);
	}
	// Set properties on OSG sequence.
	switch(loop){
		case 0:
			seq->setInterval(osg::Sequence::LOOP, begin, end);			
			break;
		case 1:
			seq->setInterval(osg::Sequence::SWING, begin, end);			
			break;
	}
	seq->setDuration(speed, reps);
	switch(mode){
		case 0:
			seq->setMode(osg::Sequence::START);
			break;
		case 1:
			seq->setMode(osg::Sequence::STOP);
			break;
		case 2:
			seq->setMode(osg::Sequence::PAUSE);
			break;
		case 3:
			seq->setMode(osg::Sequence::RESUME);
			break;
	}
	// Add geometry to sequence.
	if(pblock2){
		// Sample sequence objects.
		if(approach==0){
			INode* seqGeomNode = pblock2->GetINode(sequence_sample_node, t);
			if(seqGeomNode){
				// Assert this is a geometry node
				ObjectState os = seqGeomNode->EvalWorldState(t); 
				if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID ){

					// Frames per sample
					int fps;
					pblock2->GetValue(sequence_frame_per_sample, t, fps, iv);

					// The start and end of animation.
					TimeValue animStart = _ip->GetAnimRange().Start();
					TimeValue animEnd = _ip->GetAnimRange().End();
					TimeValue step;
					int n=0;
					int delta = GetTicksPerFrame()*fps;

					// If the interval given in pblock2 is different from -1
					// we should use this interval instead. This is given in sec.
					if(end > begin){
						animStart = begin*GetFrameRate()*GetTicksPerFrame();
						animEnd = end*GetFrameRate()*GetTicksPerFrame();
					}

					// Sample the MAX geometry node.
					//  frameTime  frameTime  frameTime  frameTime 
					//      A          B           C         D
					// |----------|----------|----------|----------|    time
					// A          B			 C          D	        	sampled geometry 
					osg::Geode* prevGeode = NULL;
					float frameTime = 0.0f;
					for (step=animStart; step<=animEnd-delta; step+=delta) {
						os = seqGeomNode->EvalWorldState(step); 
						// Create sequence geometry; this geometry is placed relative!
						osg::ref_ptr<osg::MatrixTransform> transform =  createGeomObject(rootTransform, seqGeomNode, os.obj,  step);
						osg::Geode* geode = Util::getGeode(transform.get());

						// Only add geode to OSG sequence if it is different
						// from the previous sampled geometry.
						if(!Util::isGeodeEqual(prevGeode, geode)){
							// Save geode so we can compared it to the next sampled geometry.
							prevGeode = geode;

							// Only set the frame time when we know for how long time we are going
							// to see geometry, this is first possible, when we know when we should display
							// the next geometry. Hence do not set it the first time.
							if(step!=animStart){
								seq->setTime(n++, frameTime);
								frameTime = 0.0f;
							}
						
							// If the seqGeomNode has a parentnode different from
							// the MAX rootNode, we need to create an OSG MatrixTransform
							// to put the sequence geometry in before adding it to the OSG Sequence.
							if(!seqGeomNode->GetParentNode()->IsRootNode()){
								// Get absolute position of parent node.
								osg::Matrix parentMat = convertMat(seqGeomNode->GetParentNode()->GetObjTMBeforeWSM(t));
								osg::ref_ptr<osg::MatrixTransform> parentTransform = new osg::MatrixTransform();
								// Set static datavariance for better performance
								parentTransform->setDataVariance(osg::Object::STATIC);
								// Use default node mask
								if(_options->getUseDefaultNodeMaskValue())
									parentTransform->setNodeMask(_options->getDefaultNodeMaskValue());

								parentTransform->setMatrix(parentMat);
								parentTransform->addChild(transform.get());
								seq->addChild(parentTransform.get());
							}
							else{
								seq->addChild(transform.get());
							}
						}
						// The frame time for the geometry in secs.
						frameTime += (float)(delta)/(float)(GetTicksPerFrame()*GetFrameRate());					
					}
					// Set frame time for last geoemtry
					seq->setTime(n++, frameTime);
				}
			}
		}
		// or use static objects.
		else{
			for(int n=0; n<NUM_SEQUENCE_OBJECTS; n++){
				INode* seqGeomNode = pblock2->GetINode(sequence_node+n, t);
				osg::ref_ptr<osg::Node> transform = createGeomObjectForHelperObjects(rootTransform, seqGeomNode, obj, t);
				if(transform.valid()){
					seq->addChild(transform.get());
					// Get time value from helper object.
					float time;
					pblock2->GetValue(sequence_time, t, time, iv, n);
			        seq->setTime(n, time);
				}
			}// End for number of static sequence objects in pblock2
		}
	}

	// Return OSG Sequence.
	return seq;
}

/**
 * This method will create an osg::Switch object from the 
 * properties defined in an OSG Switch helper object. 
 */
osg::ref_ptr<osg::Switch> OSGExp::createSwitchFromHelperObject(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){
	osg::ref_ptr<osg::Switch> switcher = new osg::Switch();
	switcher->setName(node->GetName());
	// Set static datavariance for better performance
	switcher->setDataVariance(osg::Object::STATIC);
	// Use default node mask
	if(_options->getUseDefaultNodeMaskValue())
		switcher->setNodeMask(_options->getDefaultNodeMaskValue());


	// The switch properties are saved in a parameter block.
	IParamBlock2* pblock2 = (IParamBlock2*)obj->GetParamBlock();
	Interval iv;

	BOOL switchEnableChildren[9];
	//memset(switchEnableChildren, 0, sizeof(BOOL) * 9);

	if(pblock2)
	{
		pblock2->GetValue(switch_enable_children1, t, switchEnableChildren[0], iv);
		pblock2->GetValue(switch_enable_children2, t, switchEnableChildren[1], iv);
		pblock2->GetValue(switch_enable_children3, t, switchEnableChildren[2], iv);
		pblock2->GetValue(switch_enable_children4, t, switchEnableChildren[3], iv);
		pblock2->GetValue(switch_enable_children5, t, switchEnableChildren[4], iv);
		pblock2->GetValue(switch_enable_children6, t, switchEnableChildren[5], iv);
		pblock2->GetValue(switch_enable_children7, t, switchEnableChildren[6], iv);
		pblock2->GetValue(switch_enable_children8, t, switchEnableChildren[7], iv);
		pblock2->GetValue(switch_enable_children9, t, switchEnableChildren[8], iv);
	}


	// Add geometry to osg::switch.
	if(pblock2){
		for(int n=0; n<pblock2->Count(switch_nodes); n++){
			INode* switchGeomNode = pblock2->GetINode(switch_nodes, t,n);
			osg::ref_ptr<osg::Node> transform = createGeomObjectForHelperObjects(rootTransform, switchGeomNode, obj, t);
			if(transform.valid())
			{
				switcher->addChild(transform.get());
				if(n < 9)
				{
					switcher->setChildValue(transform.get(), switchEnableChildren[n]);
				}
			}
		}// End for all geometry nodes.
	}
	// Return switch.
	return switcher;
}



osg::ref_ptr<osg::Group> OSGExp::createGroupFromHelper(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){
	osg::ref_ptr<osg::Group> pGroup = new osg::Group();
	pGroup->setName(node->GetName());
	// Set static datavariance for better performance
	pGroup->setDataVariance(osg::Object::STATIC);
	// Use default node mask
	if(_options->getUseDefaultNodeMaskValue())
		pGroup->setNodeMask(_options->getDefaultNodeMaskValue());


	// The switch properties are saved in a parameter block.
	IParamBlock2* pblock2 = (IParamBlock2*)obj->GetParamBlock();
	Interval iv;


	// Add geometry to osg::group.
	if(pblock2){
		for(int n=0; n<pblock2->Count(osggroup_nodes); n++){
			INode* switchGeomNode = pblock2->GetINode(osggroup_nodes, t,n);
			osg::ref_ptr<osg::Node> transform = createGeomObjectForHelperObjects(rootTransform, switchGeomNode, obj, t);
			if(transform.valid())
			{
				pGroup->addChild(transform.get());
			}
		}// End for all geometry nodes.
	}
	// Return group.
	return pGroup;
}




osg::ref_ptr<osg::MatrixTransform> OSGExp::createDOFFromHelper(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){
	osg::ref_ptr<osgSim::DOFTransform> pDOF = new osgSim::DOFTransform();
	pDOF->setName(node->GetName());
	// Set static datavariance for better performance
	pDOF->setDataVariance(osg::Object::STATIC);
	// Use default node mask
	if(_options->getUseDefaultNodeMaskValue())
		pDOF->setNodeMask(_options->getDefaultNodeMaskValue());


	// The switch properties are saved in a parameter block.
	IParamBlock2* pblock2 = (IParamBlock2*)obj->GetParamBlock();
	Interval iv;


	int multOrder;
	osg::Vec3 minHPR, maxHPR, minTrans, maxTrans, minScale, maxScale;

	if(pblock2){
		pblock2->GetValue(dof_mult_order, t, multOrder, iv);

		pblock2->GetValue(dof_min_hpr_h, t, minHPR[0], iv);
		pblock2->GetValue(dof_min_hpr_p, t, minHPR[1], iv);
		pblock2->GetValue(dof_min_hpr_r, t, minHPR[2], iv);

		pblock2->GetValue(dof_max_hpr_h, t, maxHPR[0], iv);
		pblock2->GetValue(dof_max_hpr_p, t, maxHPR[1], iv);
		pblock2->GetValue(dof_max_hpr_r, t, maxHPR[2], iv);

		pblock2->GetValue(dof_min_translate_x, t, minTrans[0], iv);
		pblock2->GetValue(dof_min_translate_y, t, minTrans[1], iv);
		pblock2->GetValue(dof_min_translate_z, t, minTrans[2], iv);

		pblock2->GetValue(dof_max_translate_x, t, maxTrans[0], iv);
		pblock2->GetValue(dof_max_translate_y, t, maxTrans[1], iv);
		pblock2->GetValue(dof_max_translate_z, t, maxTrans[2], iv);

		pblock2->GetValue(dof_min_scale_x, t, minScale[0], iv);
		pblock2->GetValue(dof_min_scale_y, t, minScale[1], iv);
		pblock2->GetValue(dof_min_scale_z, t, minScale[2], iv);

		pblock2->GetValue(dof_max_scale_x, t, maxScale[0], iv);
		pblock2->GetValue(dof_max_scale_y, t, maxScale[1], iv);
		pblock2->GetValue(dof_max_scale_z, t, maxScale[2], iv);
	}
	// Set properties on OSG billboard.
	switch(multOrder){
	case 0:	
		pDOF->setHPRMultOrder(osgSim::DOFTransform::PRH);
		break;
	case 1:
		pDOF->setHPRMultOrder(osgSim::DOFTransform::PHR);
		break;
	case 2:
		pDOF->setHPRMultOrder(osgSim::DOFTransform::HPR);
		break;
	case 3:
		pDOF->setHPRMultOrder(osgSim::DOFTransform::HRP);
		break;
	case 4:
		pDOF->setHPRMultOrder(osgSim::DOFTransform::RPH);
		break;
	case 5:
		pDOF->setHPRMultOrder(osgSim::DOFTransform::RHP);
		break;
	}

	minHPR.set(osg::DegreesToRadians(minHPR[0]), osg::DegreesToRadians(minHPR[1]), osg::DegreesToRadians(minHPR[2]));
	maxHPR.set(osg::DegreesToRadians(maxHPR[0]), osg::DegreesToRadians(maxHPR[1]), osg::DegreesToRadians(maxHPR[2]));
	
	pDOF->setMinHPR(minHPR);
	pDOF->setMaxHPR(maxHPR);
	pDOF->setMinTranslate(minTrans);
	pDOF->setMaxTranslate(maxTrans);
	pDOF->setMinScale(minScale);
	pDOF->setMaxScale(maxScale);

	pDOF->setCurrentScale(osg::Vec3(1.0f, 1.0f, 1.0f));
	
	osg::Matrix pivotTransform = convertMat(node->GetNodeTM(t));
	osg::Matrix pivotTransform_inverse;
	pivotTransform_inverse.invert(pivotTransform);

	osg::ref_ptr<osg::MatrixTransform> dofParent = new osg::MatrixTransform(pivotTransform);
	osg::ref_ptr<osg::MatrixTransform> dofChild = new osg::MatrixTransform(pivotTransform_inverse);

	if(pblock2)
	{
		for(int n=0; n<pblock2->Count(doftrans_nodes); n++)
		{
			INode* dofChildNode = pblock2->GetINode(doftrans_nodes, t,n);					
			osg::ref_ptr<osg::Node> newChild = createGeomObjectForHelperObjects(rootTransform, dofChildNode, obj, t);
		
			if(newChild.valid())
			{	
				dofChild->addChild(newChild.get());	
			}
		}
	}

	pDOF->addChild(dofChild.get());
	dofParent->addChild(pDOF.get());
	return dofParent;
}



/**
 * This method will create an osg::Impostor object from the 
 * properties defined in an OSG Impostor helper object. 
 */
osg::ref_ptr<osg::Group> OSGExp::createImpostorFromHelperObject(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){

	osg::ref_ptr<osg::Group> group = new osg::Group();
	group->setName(node->GetName());
	// Set static datavariance for better performance
	group->setDataVariance(osg::Object::STATIC);
	// Use default node mask
	if(_options->getUseDefaultNodeMaskValue())
		group->setNodeMask(_options->getDefaultNodeMaskValue());

	// The impostor properties are saved in a parameter block.
	IParamBlock2* pblock2 = (IParamBlock2*)obj->GetParamBlock();
	Interval iv;
	// Add geometry to osg::impostor
	if(pblock2){
		for(int n=0; n<pblock2->Count(impostor_nodes); n++){
			INode* impostorGeomNode = pblock2->GetINode(impostor_nodes, t,n);
			osg::ref_ptr<osg::Node> transform = createGeomObjectForHelperObjects(rootTransform, impostorGeomNode, obj, t);
			if(transform.valid()){
				// Create an impostor for every geometry node.
				osg::ref_ptr<osgSim::Impostor> impostor = new osgSim::Impostor;
				// Set static datavariance for better performance
				impostor->setDataVariance(osg::Object::STATIC);
				// Use default node mask
				if(_options->getUseDefaultNodeMaskValue())
					impostor->setNodeMask(_options->getDefaultNodeMaskValue());

				// Add geometry to impostor.
				impostor->addChild(transform.get());
				// Find bounding sphere for the impostor geometry.
				osg::BoundingSphere bs;
				bs = transform->getBound();
				// Set impostor properties.
				float threshold;
				Interval iv;
				pblock2->GetValue(threshold_value, t, threshold, iv);
				impostor->setImpostorThresholdToBound(threshold);
				impostor->setRange(0,0.0f,1e7f);
				impostor->setCenter(bs.center());
				// Add impostor to group.
				group->addChild(impostor.get());
			}
		}// End for all geometry nodes
	}
	// Return group with impostors.
	return group;
}

/**
 * This method will create an osg::Occluder object from the 
 * properties defined in an OSG Occluder helper object. 
 */
void OSGExp::createOccluderFromHelperObject(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){

	// The Occluder properties are saved in a parameter block.
	IParamBlock2* pblock2 = (IParamBlock2*)obj->GetParamBlock();
	Interval iv;
	// Add geometry to osg::Occluder.
	if(pblock2){
		for(int n=0; n<pblock2->Count(occluder_planes); n++){
			INode* geomNode = pblock2->GetINode(occluder_planes, t,n);
			if(geomNode){
				// Assert this is a shape node
				ObjectState os = geomNode->EvalWorldState(t); 
            if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID){//SHAPE_CLASS_ID ){

					// Get the absolute position of shape.
					osg::Matrix mat = convertMat(geomNode->GetObjTMBeforeWSM(t));
	
					// Get shape geometry.
					osg::ref_ptr<osg::Geode>  geode = createMeshGeometry(rootTransform, geomNode, os.obj, t);

					// Transform all vertices in shape geometry to absolute position
					// and insert them into occluder.
					osg::Vec3Array* coords = static_cast<osg::Vec3Array*>(((osg::Geometry*)geode->getDrawable(0))->getVertexArray());
					if(coords){
						// Create occluder.
						osg::OccluderNode* occluderNode = new osg::OccluderNode;
						occluderNode->setName(node->GetName());
						// Set static datavariance for better performance
						occluderNode->setDataVariance(osg::Object::STATIC);
						// Use default node mask
						if(_options->getUseDefaultNodeMaskValue())
							occluderNode->setNodeMask(_options->getDefaultNodeMaskValue());
						osg::ConvexPlanarOccluder* cpo = new osg::ConvexPlanarOccluder;
						occluderNode->setOccluder(cpo);
						osg::ConvexPlanarPolygon& occluder = cpo->getOccluder();
						// Add occluder to rootTransform.
						rootTransform->addChild(occluderNode);
						int numCoords = coords->getNumElements();
						for(int i=0;i<numCoords;i++){
							osg::Vec3 v = (*coords)[i];
							v = mat.preMult(v);
							occluder.add(v);
						}
					}
				}
			}
		}
	}
}

/**
 * This method will create an osgSim::VisibilityGroup object from the 
 * properties defined in an OSG VisibilityGroup helper object. 
 */
osg::ref_ptr<osgSim::VisibilityGroup> OSGExp::createVisibilityGroupFromHelperObject(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){
	osg::ref_ptr<osgSim::VisibilityGroup> visibilityGroup = new osgSim::VisibilityGroup();
	visibilityGroup->setName(node->GetName());
	// Set static datavariance for better performance
	visibilityGroup->setDataVariance(osg::Object::STATIC);
	// Use default node mask
	if(_options->getUseDefaultNodeMaskValue())
		visibilityGroup->setNodeMask(_options->getDefaultNodeMaskValue());

	// The VisibilityGroup's properties are saved in a parameter block.
	IParamBlock2* pblock2 = (IParamBlock2*)obj->GetParamBlock();
	Interval iv;

	if(pblock2){
		// Set visibility volume.
		INode* volumeNode = pblock2->GetINode(visibilitygroup_volume, t, 0);
		osg::ref_ptr<osg::Node> transform = createGeomObjectForHelperObjects(rootTransform, volumeNode, obj, t);
		if(transform.valid())
			visibilityGroup->setVisibilityVolume(transform.get());

		// Add all the geometry nodes to the VisibilityGroup.
		for(int n=0; n<pblock2->Count(visibilitygroup_nodes); n++){
			INode* geomNode = pblock2->GetINode(visibilitygroup_nodes, t, n);
			osg::ref_ptr<osg::Node> transform = createGeomObjectForHelperObjects(visibilityGroup.get(), geomNode, obj, t);
			if(transform.valid())
				visibilityGroup->addChild(transform.get());
		}
	}
	// Return VisibilityGroup.
	return visibilityGroup;
}

/**
 * This method will create geometry object for the helper objects
 * and place them in absolute positions.
 */
osg::ref_ptr<osg::Node> OSGExp::createGeomObjectForHelperObjects(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t){
	if(node == NULL)
		return NULL;

	// Traverse the node and export geometry or groups.
	osg::ref_ptr<osg::Node> transform = traverseNode(rootTransform, node, t);

	// If the node has a parentnode different from
	// the MAX rootNode, we need to create an OSG MatrixTransform
	// to put the geometry/group node in before returning it.
	if(!node->GetParentNode()->IsRootNode()){
		// Get absolute position of parent node.
		osg::Matrix parentMat = convertMat(node->GetParentNode()->GetObjTMBeforeWSM(t));
		osg::ref_ptr<osg::MatrixTransform> parentTransform = new osg::MatrixTransform();
		// Set static datavariance for better performance
		parentTransform->setDataVariance(osg::Object::STATIC);
		// Use default node mask
		if(_options->getUseDefaultNodeMaskValue())
			parentTransform->setNodeMask(_options->getDefaultNodeMaskValue());
		parentTransform->setMatrix(parentMat);
		parentTransform->addChild(transform.get());
		applyNodeMaskValue(node, t, parentTransform.get());
		return osg::ref_ptr<osg::Node>(static_cast<osg::Node*>(parentTransform.get()));
	}
	return transform;
}

/**
 * This method will traverse the node and create geometry objects or groups.
 */
osg::ref_ptr<osg::Node> OSGExp::traverseNode(osg::Group* rootTransform, INode* node, TimeValue t){

	// If this a group node then make an OSG group node
	// and traverse into the children.
	if (node->IsGroupHead()) {
		osg::ref_ptr<osg::MatrixTransform> groupNode = new osg::MatrixTransform();
		// Set name of group node.
		groupNode->setName(std::string(node->GetName()));
		// Set static datavariance for better performance
		groupNode->setDataVariance(osg::Object::STATIC);
		// Set NodeMask
		if(_options->getUseDefaultNodeMaskValue())
			groupNode->setNodeMask(_options->getDefaultNodeMaskValue());
		groupNode->setMatrix(getNodeTransform(node, _ip->GetTime()));
		// Traverse and add children.
		for (int c = 0; c < node->NumberOfChildren(); c++){
			osg::ref_ptr<osg::Node> transform = traverseNode(rootTransform, node->GetChildNode(c), t);
			if (transform.valid())
				groupNode->addChild(transform.get());
		}
		applyNodeMaskValue(node, t, groupNode.get());
		return osg::ref_ptr<osg::Node>(static_cast<osg::Node*>(groupNode.get()));
	}
	else{
		// Is this a geometry group.
		ObjectState os = node->EvalWorldState(t); 
		if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID )
		{
			return osg::ref_ptr<osg::Node>(static_cast<osg::Node*>(createGeomObject(rootTransform, node, os.obj, t).get()));
		}
		else if(os.obj->SuperClassID() == HELPER_CLASS_ID)
		{
			return createHelperObject(rootTransform, node, os.obj, _ip->GetTime());
		}
	}
	//we shouldnt ever get here... hopefully
	return NULL;
}

osg::ref_ptr<osg::MatrixTransform> OSGExp::createPointFromHelperObject(osg::Group* rootTransform, INode* node, Object* obj, TimeValue t)
{
    osg::MatrixTransform *matrixTransform=new osg::MatrixTransform;
    matrixTransform->setDataVariance(osg::Object::STATIC);
    osg::Geode *geode=new osg::Geode;
    geode->setCullingActive(false);
    geode->setDataVariance(osg::Object::STATIC);

    IParamBlock2* pblock2 = (IParamBlock2*)obj->GetParamBlock();
    if(pblock2) {
        float size;
        Interval iv;
        pblock2->GetValue(pointobj_size,t,size,iv);

        if(size!=1.0f) {
            osg::Point *point=new osg::Point;
            point->setSize(size);
            geode->getOrCreateStateSet()->setAttribute(point);
        }
    }

    osg::Geometry *geometry=new osg::Geometry;
    geometry->setDataVariance(osg::Object::STATIC);

	Matrix3 nodeTM = node->GetNodeTM(t);
	Matrix3 parentTM = node->GetParentTM(t);
	nodeTM = nodeTM * Inverse(parentTM);

    osg::Vec3Array *vertices=new osg::Vec3Array;
    Point3 pos=node->GetObjOffsetPos();
    pos=nodeTM*pos;
    
    vertices->push_back(osg::Vec3(pos.x,pos.y,pos.z));

    geometry->setVertexArray(vertices);
    geometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS,0,vertices->size()));

    geode->addDrawable(geometry);
    matrixTransform->addChild(geode);

    return matrixTransform;
}
