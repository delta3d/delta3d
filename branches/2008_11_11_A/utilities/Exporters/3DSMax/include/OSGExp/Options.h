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
 *	FILE:			Options.h
 *
 *	DESCRIPTION:	Header file for the Options class. This class 
 *					provides methods to get and set the options of the 
 *					exporter.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 27.01.2003
 *
 *                  05.11.2007 Daniel Sjolie: Added useOriginalTextures Option
 */		


#ifndef __OPTIONS__H
#define __OPTIONS__H

#include "max.h"
#include <string>

class Options{

public:

	// Constructor/Destructor
	Options();
	~Options();

	// Methods to get options.
	BOOL  getExportGeometry()				{return _exportGeometry;};
	BOOL  getExportTextureCoords()			{return _exportTextureCoords;};
	BOOL  getExportVertexNormals()			{return _exportVertexNormals;};
	BOOL  getExportVertexColors()			{return _exportVertexColors;};
	BOOL  getExportMaterials()				{return _exportMaterials;};
	BOOL  getExportTextures()				{return _exportTextures;};
	BOOL  getExportCameras()				{return _exportCameras;};
	BOOL  getExportLights()					{return _exportLights;};
	BOOL  getExportHelpers()				{return _exportHelpers;};
	BOOL  getExportShapes()					{return _exportShapes;};
	BOOL  getExportHiddenNodes()			{return _exportHiddenNodes;};
	BOOL  getExportAnimations()				{return _exportAnimations;};
	BOOL  getExportParticles()				{return _exportParticles;};
	BOOL  getNormalize()					{return _normalize;};
	BOOL  getWhitenTexMat()					{return _whitenTexMat;};
	BOOL  getWhitenSelfIllum()				{return _whitenSelfIllum;};
	BOOL  getSaveFile()						{return _saveFile;};
	BOOL  getIncludeImageDataInIveFile()	{return _includeImageDataInIveFile;};
	BOOL  getQuickView()					{return _quickView;};
	BOOL  getShowErrMsg()					{return _showErrMsg;};
	BOOL  getUseIndices()					{return _useIndices;};
	BOOL  getWriteTexture()					{return _writeTexture;};
	BOOL  getUseOriginalTextureFiles()		{return _useOriginalTextureFiles;};
	int   getNumTexFormat()					{return _numTexFormat;};
	int   getTexFormatIndex()				{return _texFormatIndex;};
	char* getTexFormat(int i)				{return _texFormat[i];};
	int   getNumTexComp()					{return _numTexComp;};
	int   getTexCompIndex()					{return _texCompIndex;};
	char* getTexComp(int i)					{return _texComp[i];};
	std::string getExportPath()				{return _exportPath;};
	std::string getExportFilename()			{return _exportFilename;};
	std::string getExportExtension()		{return _exportExtension;};
	BOOL getUseDefaultNodeMaskValue()		{return _useDefaultNodeMaskValue;};
	unsigned int getDefaultNodeMaskValue()	{return _defaultNodeMaskValue;};
	BOOL getTriStripGeometry()				{return _triStripGeometry;};
	BOOL getMergeGeometry()					{return _mergeGeometry;};
	BOOL getFlattenStaticTransform()		{return _flattenStaticTransform;};
	BOOL getShareDuplicateStates()			{return _shareDuplicateStates;};
	BOOL getSpatializeGroups()				{return _spatializeGroups;};
	BOOL getTurnOffLighting()				{return _turnOffLighting;};
    BOOL getExportOpacityMaps()             {return _exportOpacityMaps;}
    BOOL getExportUnknownMaps()             {return _exportUnknownMaps;}
    BOOL getAutoAlphaTextures()             {return _autoAlphaTextures;}
    BOOL getAutoTwoSidedLighting()          {return _autoTwoSidedLighting;}
    BOOL getExportSelfIllumMaps()           {return _exportSelfIllumMaps;}
    BOOL getExportPointHelpers()            {return _exportPointHelpers;}
    BOOL getReferencedGeometry()            {return _referenceGeometry;}


	// Methods to set options.
	void setExportGeometry(BOOL b)			{_exportGeometry = b;};
	void setExportTextureCoords(BOOL b)		{_exportTextureCoords = b;};
	void setExportVertexNormals(BOOL b)		{_exportVertexNormals = b;};
	void setExportVertexColors(BOOL b)		{_exportVertexColors = b;};
	void setExportMaterials(BOOL b)			{_exportMaterials = b;};
	void setExportTextures(BOOL b)			{_exportTextures = b;};
	void setExportCameras(BOOL b)			{_exportCameras = b;};
	void setExportLights(BOOL b)			{_exportLights = b;};
	void setExportHelpers(BOOL b)			{_exportHelpers = b;};
	void setExportShapes(BOOL b)			{_exportShapes = b;};
	void setExportHiddenNodes(BOOL b)		{_exportHiddenNodes = b;};
	void setExportAnimations(BOOL b)		{_exportAnimations = b;};
	void setExportParticles(BOOL b)			{_exportParticles = b;};
	void setNormalize(BOOL b)				{_normalize = b;};
	void setWhitenTexMat(BOOL b)			{_whitenTexMat = b;};
	void setWhitenSelfIllum(BOOL b)			{_whitenSelfIllum = b;};
	void setSaveFile(BOOL b)				{_saveFile = b;};
	void setQuickView(BOOL b)				{_quickView = b;};
	void setShowErrMsg(BOOL b)				{_showErrMsg = b;};
	void setUseIndices(BOOL b)				{_useIndices = b;};
	void setWriteTexture(BOOL b)			{_writeTexture = b;};
	void setUseOriginalTextureFiles(BOOL b)	{_useOriginalTextureFiles = b;};
	void setTexFormatIndex(int i)			{_texFormatIndex = i;};
	void setTexCompIndex(int i)				{_texCompIndex = i;};
	void setIncludeImageDataInIveFile(BOOL b){_includeImageDataInIveFile = b;};
	void setExportPath(std::string p)		{_exportPath = p;};
	void setExportFilename(std::string f)	{_exportFilename = f;};
	void setExportExtension(std::string e)	{_exportExtension = e;};
	void setUseDefaultNodeMaskValue(BOOL b)	{_useDefaultNodeMaskValue = b;}
	void setDefaultNodeMaskValue(unsigned int i) {_defaultNodeMaskValue = i;}
	void setTriStripGeometry(BOOL b)		{_triStripGeometry = b;}
	void setMergeGeometry(BOOL b)			{_mergeGeometry = b;}
	void setFlattenStaticTransform(BOOL b)	{_flattenStaticTransform = b;}
	void setShareDuplicateStates(BOOL b)	{_shareDuplicateStates = b;}
	void setSpatializeGroups(BOOL b)		{_spatializeGroups = b;}
	void setTurnOffLighting(BOOL b)			{_turnOffLighting = b;}
    void setExportOpacityMaps(BOOL b)       {_exportOpacityMaps = b;}
    void setExportUnknownMaps(BOOL b)       {_exportUnknownMaps = b;}
    void setAutoAlphaTextures(BOOL b)       {_autoAlphaTextures = b;}
    void setAutoTwoSidedLighting(BOOL b)    {_autoTwoSidedLighting = b;}
    void setExportSelfIllumMaps(BOOL b)     {_exportSelfIllumMaps = b;}
    void setExportPointHelpers(BOOL b)      {_exportPointHelpers = b;}
   void setReferencedGeometry(BOOL b)      {_referenceGeometry = b;}

	// Methods to write and load options from disk.
	BOOL write(TSTR filename);
	BOOL load(TSTR filename);

private:

	// Options for the plugin.
	BOOL			_exportGeometry;
	BOOL			_exportTextureCoords;
	BOOL			_exportVertexNormals;
	BOOL			_exportVertexColors;
	BOOL			_exportMaterials;
	BOOL			_exportTextures;
	BOOL			_exportCameras;
	BOOL			_exportLights;
	BOOL			_exportHelpers;
	BOOL			_exportShapes;
	BOOL			_exportHiddenNodes;
	BOOL			_exportAnimations;
	BOOL			_exportParticles;
	BOOL			_flipNormals;
	BOOL			_normalize;
	BOOL			_whitenTexMat;
	BOOL			_whitenSelfIllum;
	BOOL			_saveFile;
	BOOL			_includeImageDataInIveFile;
	BOOL			_quickView;
	BOOL			_showErrMsg;
	BOOL			_useIndices;
	BOOL			_writeTexture;
	BOOL			_useOriginalTextureFiles;
	int				_numTexFormat;
	int				_texFormatIndex;
	char**			_texFormat;
	int				_numTexComp;
	int				_texCompIndex;
	char**			_texComp;
	std::string		_exportPath;
	std::string		_exportFilename;
	std::string		_exportExtension;
	BOOL			_useDefaultNodeMaskValue;
	unsigned int	_defaultNodeMaskValue;
	BOOL			_triStripGeometry;
	BOOL			_mergeGeometry;
	BOOL			_flattenStaticTransform;
	BOOL			_shareDuplicateStates;
	BOOL			_spatializeGroups;
	BOOL			_turnOffLighting;
    BOOL            _exportOpacityMaps;
    BOOL            _exportUnknownMaps;
    BOOL            _autoAlphaTextures;
    BOOL            _autoTwoSidedLighting;
    BOOL            _exportSelfIllumMaps;
    BOOL            _exportPointHelpers;
    BOOL            _referenceGeometry;
};

#endif // __OPTIONS__H
