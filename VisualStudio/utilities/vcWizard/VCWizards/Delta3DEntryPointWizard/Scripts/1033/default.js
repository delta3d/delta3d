
function OnFinish(selProj, selObj)
{
	try
	{
		var strProjectPath = wizard.FindSymbol('PROJECT_PATH');
		var strProjectName = wizard.FindSymbol('PROJECT_NAME');

		selProj = CreateCustomProject(strProjectName, strProjectPath);
		AddConfig(selProj, strProjectName);
		AddFilters(selProj);

		var InfFile = CreateCustomInfFile();
		AddFilesToCustomProj(selProj, strProjectName, strProjectPath, InfFile);
		PchSettings(selProj);
		InfFile.Delete();

		selProj.Object.Save();
	}
	catch(e)
	{
		if (e.description.length != 0)
			SetErrorInfo(e);
		return e.number
	}
}

function CreateCustomProject(strProjectName, strProjectPath)
{
	try
	{
		var strProjTemplatePath = wizard.FindSymbol('PROJECT_TEMPLATE_PATH');
		var strProjTemplate = '';
		strProjTemplate = strProjTemplatePath + '\\default.vcproj';

		var Solution = dte.Solution;
		var strSolutionName = "";
		if (wizard.FindSymbol("CLOSE_SOLUTION"))
		{
			Solution.Close();
			strSolutionName = wizard.FindSymbol("VS_SOLUTION_NAME");
			if (strSolutionName.length)
			{
				var strSolutionPath = strProjectPath.substr(0, strProjectPath.length - strProjectName.length);
				Solution.Create(strSolutionPath, strSolutionName);
			}
		}

		var strProjectNameWithExt = '';
		strProjectNameWithExt = strProjectName + '.vcproj';

		var oTarget = wizard.FindSymbol("TARGET");
		var prj;
		if (wizard.FindSymbol("WIZARD_TYPE") == vsWizardAddSubProject)  // vsWizardAddSubProject
		{
			var prjItem = oTarget.AddFromTemplate(strProjTemplate, strProjectNameWithExt);
			prj = prjItem.SubProject;
		}
		else
		{
			prj = oTarget.AddFromTemplate(strProjTemplate, strProjectPath, strProjectNameWithExt);
		}
		return prj;
	}
	catch(e)
	{
		throw e;
	}
}

function AddFilters(proj)
{
	try
	{
		// Add the folders to your project
		var strSrcFilter = wizard.FindSymbol('SOURCE_FILTER');
		var group = proj.Object.AddFilter('Source Files');
    	group.Filter = "cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx";
    	
 		var strSrcFilter = wizard.FindSymbol('HEADER_FILTER');
		var group = proj.Object.AddFilter('Header Files');
    	group.Filter = "h;hpp;hxx;hm;inl;inc;xsd";
   	
    	
	}
	catch(e)
	{
		throw e;
	}
}

function AddConfig(proj, strProjectName)
{
	try
	{
		var config = proj.Object.Configurations('Debug');
		config.IntermediateDirectory = '$(ConfigurationName)';
		config.OutputDirectory = '$(ConfigurationName)';
		config.ConfigurationType = 2;

		// Add compiler settings
		var CLTool = config.Tools('VCCLCompilerTool');
		CLTool.Optimization = 0;
		CLTool.AdditionalIncludeDirectories = '$(DELTA_INC)';
		CLTool.PreprocessorDefinitions = "WIN32;_DEBUG;ENTRY_POINT_LIBRARY";
		CLTool.MinimalRebuild = true;
		CLTool.BasicRuntimeChecks = 3;
		CLTool.RuntimeLibrary = 3;
		CLTool.WarningLevel = 3;
		CLTool.DebugInformationFormat = 3;

		// Add linker settings
		var LinkTool = config.Tools('VCLinkerTool');
		LinkTool.AdditionalDependencies = "dtCoreD.lib dtGameD.lib osgD.lib";
		LinkTool.AdditionalLibraryDirectories = '$(DELTA_LIB)';
		LinkTool.OutputFile = '$(OutDir)\\' + strProjectName + 'EntryPointD.dll';
		LinkTool.GenerateDebugInformation = true;
		LinkTool.SubSystem = 2;
		LinkTool.TargetMachine = 1;

		config = proj.Object.Configurations('Release');
		config.IntermediateDirectory = '$(ConfigurationName)';
		config.OutputDirectory = '$(ConfigurationName)';
		config.ConfigurationType = 2;

		// Add compiler settings
		var CLTool = config.Tools('VCCLCompilerTool');
		CLTool.AdditionalIncludeDirectories = '$(DELTA_INC)';
		CLTool.PreprocessorDefinitions = "WIN32;ENTRY_POINT_LIBRARY";

		//Add linker settings
		var LinkTool = config.Tools('VCLinkerTool');
		LinkTool.AdditionalDependencies = "dtCore.lib dtGame.lib osg.lib";
		LinkTool.AdditionalLibraryDirectories = '$(DELTA_LIB)';
		LinkTool.OutputFile = '$(OutDir)\\' + strProjectName + 'EntryPoint.dll';

	}
	catch(e)
	{
		throw e;
	}
}

function PchSettings(proj)
{
	// TODO: specify pch settings
}

function DelFile(fso, strWizTempFile)
{
	try
	{
		if (fso.FileExists(strWizTempFile))
		{
			var tmpFile = fso.GetFile(strWizTempFile);
			tmpFile.Delete();
		}
	}
	catch(e)
	{
		throw e;
	}
}

function CreateCustomInfFile()
{
	try
	{
		var fso, TemplatesFolder, TemplateFiles, strTemplate;
		fso = new ActiveXObject('Scripting.FileSystemObject');

		var TemporaryFolder = 2;
		var tfolder = fso.GetSpecialFolder(TemporaryFolder);
		var strTempFolder = tfolder.Drive + '\\' + tfolder.Name;

		var strWizTempFile = strTempFolder + "\\" + fso.GetTempName();

		var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
		var strInfFile = strTemplatePath + '\\Templates.inf';
		wizard.RenderTemplate(strInfFile, strWizTempFile);

		var WizTempFile = fso.GetFile(strWizTempFile);
		return WizTempFile;
	}
	catch(e)
	{
		throw e;
	}
}

function GetTargetName(strName, strProjectName)
{
	try
	{
		// TODO: set the name of the rendered file based on the template filename
		var strTarget = strName;
			
		if (strName == 'EntryPoint.h')
		    strTarget = strProjectName +'EntryPoint.h';
		
		if (strName == 'EntryPoint.cpp')
		    strTarget = strProjectName +'EntryPoint.cpp';
		    
		if (strName == 'Input.h')
		   strTarget = strProjectName + 'Input.h';
		   
		if (strName == 'Input.cpp')
		   strTarget = strProjectName + 'Input.cpp';

		return strTarget; 
	}
	catch(e)
	{
		throw e;
	}
}

function AddFilesToCustomProj(proj, strProjectName, strProjectPath, InfFile)
{
	try
	{
		var projItems = proj.ProjectItems

		var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');

		var strTpl = '';
		var strName = '';

		var strTextStream = InfFile.OpenAsTextStream(1, -2);
		while (!strTextStream.AtEndOfStream)
		{
			strTpl = strTextStream.ReadLine();
			if (strTpl != '')
			{
				strName = strTpl;
				var strTarget = GetTargetName(strName, strProjectName);
				var strTemplate = strTemplatePath + '\\' + strTpl;
				var strFile = strProjectPath + '\\' + strTarget;

				var bCopyOnly = false;  //"true" will only copy the file from strTemplate to strTarget without rendering/adding to the project
				var strExt = strName.substr(strName.lastIndexOf("."));
				if(strExt==".bmp" || strExt==".ico" || strExt==".gif" || strExt==".rtf" || strExt==".css")
					bCopyOnly = true;
				wizard.RenderTemplate(strTemplate, strFile, bCopyOnly);
				proj.Object.AddFile(strFile);
			}
		}
		strTextStream.Close();
	}
	catch(e)
	{
		throw e;
	}
}
