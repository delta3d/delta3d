import glob
import os 
import shutil
import re
import sys
import time
import string
import platform
import macosx
import gch
import SCons.Util
from SCons.Environment import Base
from SCons.Script.SConscript import SConsEnvironment

##############################

def TOOL_BUNDLE(env):

########################
   def SetupDepsAndPCH(env, envCopy, deps, prefixHeader, extraDefines=[], sharedLib = False):
      #expand pre-built libs
      for lib in deps :
         if env['depsHash'].has_key(lib) :
            for newLib in env['depsHash'][lib] :
               if not newLib in deps :
                  deps.append(newLib)
          
      # merge external deps and delta libs
      # there's probably some function to do this
      allLibs = {}
      for lib_name in env['dtLibs'].keys() :
         allLibs[lib_name] = env['dtLibs'][lib_name]
         # all of the "dtLibs" are assumed to be shared libraries
         env['foundLibs'][env['dtLibs'][lib_name]] = 'sharedLib'
         
      for lib_name in env['extLibs'].keys() :
         allLibs[lib_name] = env['extLibs'][lib_name]
          
      # change to platform+mode specific names
      deps = [ allLibs[lib] for lib in deps if allLibs.has_key(lib) ]
      
      addToLink = []
      if envCopy.has_key('LINKFLAGS'):
         addToLink = envCopy['LINKFLAGS']
    
      addToLibs = []
      addToFrameworks = []
      for lib_name in deps:
         if env['foundLibs'].has_key(lib_name):
            if env['foundLibs'][lib_name] == 'framework':
               addToFrameworks.append(lib_name)
            elif env['foundLibs'][lib_name] == 'sharedLib':
                addToLibs.append(lib_name)
   
      envCopy['FRAMEWORKS'] = addToFrameworks
 
      cppDefines = []
      if envCopy.has_key('CPPDEFINES'):
         cppDefines = envCopy['CPPDEFINES']
    
      if len(prefixHeader) > 0 and envCopy.get('pch'):
          cppDefines += ['DELTA_PCH']

          prefixHeaderName = prefixHeader.split('/')[2][0:-2]

          if env['OS'] == 'darwin' or env['OS'] == 'linux':
             oldCxxFlags = envCopy['CXXFLAGS'] + ""
             envCopy['CXXFLAGS'] += " -include " + prefixHeader
             srcHeader = (prefixHeader[0:-2]) + "-src.h"
             if sharedLib:
                outputGCH = "#" + prefixHeader + ".gch/lib.h.gch"
                if not env.has_key('BuiltLibHeader' + prefixHeaderName):
                   envCopy['GchSh'] = env.GchSh(outputGCH, "#" + srcHeader, CPPDEFINES = cppDefines + extraDefines, CXXFLAGS=oldCxxFlags)[0]
                   env['BuiltLibHeader' + prefixHeaderName] = envCopy['GchSh']
                else:
                   envCopy['GchSh'] = env['BuiltLibHeader' + prefixHeaderName]   
             else:
                outputGCH = "#" + prefixHeader + ".gch/staticlib.h.gch"
                if not env.has_key('BuiltStaticLibHeader' + prefixHeaderName):
                   envCopy['Gch'] = env.Gch(outputGCH, "#" + srcHeader, CPPDEFINES = cppDefines + extraDefines, CXXFLAGS=oldCxxFlags)[0]
                   env['BuiltStaticLibHeader' + prefixHeaderName] = envCopy['Gch']
                else:
                   envCopy['Gch'] = env['BuiltStaticLibHeader' + prefixHeaderName]

          elif env['OS'] == 'windows':
             envCopy['PCH'] = envCopy.PCH("precomp.cpp", CPPDEFINES = cppDefines + extraDefines)[0]
      
      addMap = {}
      addMap['addToLink'] = addToLink
      addMap['addToLibs'] = addToLibs
      return addMap
   
########################

   def BuildProgram(env,  name, srcs, deps, extraDefines = [], extraLinkFlags = [], buildAppBundle = 1, prefixHeader='inc/prefix/dtcoreprefix.h', targetPath = "bin") :
    
       #remove the windows precompiled header file.
       if "precomp.cpp" in srcs:
          srcs.remove("precomp.cpp")
    
       target = '#'+ targetPath + '/' + name
    
       if env.get('fastBuild'):
          path = string.replace(env.GetBuildPath('SConscript'),  'SConscript', '')
          cxxFileName = name + "_.cxx"
          cxxFile = open(path + cxxFileName, "w")
          for srcFile in srcs :
             if srcFile != cxxFileName:
                cxxFile.write("#include \"" + srcFile + "\"\n")
          
          sourceList = [cxxFileName]
       else:          sourceList = srcs
    
       if env['OS'] == 'windows' and env.get('mode') == 'debug' :
          target += 'd'
    
       envProg = env.Copy()
    
       addMap = SetupDepsAndPCH(env, envProg, deps, prefixHeader, extraDefines)
       
       addToLink = addMap['addToLink']
       addToLibs = addMap['addToLibs']

       cppDefines = []
       if envProg.has_key('CPPDEFINES'):
          cppDefines = envProg['CPPDEFINES']
       
       program = envProg.Program( target, sourceList,
                                CPPDEFINES = cppDefines + extraDefines, 
                                LIBS = addToLibs, LINKFLAGS = addToLink + extraLinkFlags )
    
       if env['OS'] == 'darwin' and buildAppBundle:
          envProg ['VERSION_NUM'] = "1.2.0"
          envProg ['VERSION_NAME'] = name + '.app'
          bundle = envProg.MakeBundle ('#Applications/' + name + '.app', target,
                                  'old.delta3d.' + name,
                                  '#macosx/Info.plist',
                                  'APPL',
                                  'Dt3D',
                                  '#macosx/icons/Default.icns')
          envProg.Default (bundle)
    
          inst = envProg.InstallBundle ('$prefix/share/delta3d/Applications', bundle)
       elif env['OS'] == 'linux':
          if env.get('bindir') and env.GetLaunchDir()+'/' + targetPath != env.get('bindir'):
             envProg.Install( '$bindir', program )
          elif env.get('prefix') and env.GetLaunchDir() != env.get('prefix') :
             envProg.Install( '$prefix/share/delta3d/' + targetPath, program )
       else:
          if env.GetLaunchDir() != env.get('prefix') : 
             envProg.Install( '$prefix/share/delta3d/' + targetPath, program )
                   
########################
    
   def BuildLib(env, name, srcs, deps, extraDefines = [], extraLinkFlags = [],
          overrideShLinkFlags = '',  overrideShLibSuffix = '', prefixHeader='inc/prefix/dtcoreprefix.h', staticLib = False) :

       #remove the windows precompiled header file.
       if "precomp.cpp" in srcs:
          srcs.remove("precomp.cpp")

       if env.get('fastBuild'):
          path = string.replace(env.GetBuildPath('SConscript'),  'SConscript', '')
          cxxFileName = name + "_.cxx"
          cxxFile = open(path + cxxFileName, "w")
          for srcFile in srcs :
             if srcFile != cxxFileName:
                cxxFile.write("#include \"" + srcFile + "\"\n")
          
          sourceList = [cxxFileName]
       else:
          sourceList = srcs

       target = name
       if env['OS'] == 'windows' and env.get('mode') == 'debug' :
          target += 'd'
          
       #add dependencies to global index
       env['depsHash'][name] = deps
       
       envLib = env.Copy()
    
       if overrideShLinkFlags != '':
          envLib['SHLINKFLAGS'] = overrideShLinkFlags
       
       if overrideShLibSuffix != '':
          envLib['SHLIBSUFFIX'] = overrideShLibSuffix
    
       envLib['LIBNAME'] = name
    
       addMap = SetupDepsAndPCH(env, envLib, deps, prefixHeader, extraDefines, not staticLib)
       
       addToLink = addMap['addToLink']
       addToLibs = addMap['addToLibs']
       
       cppDefines = []
       if envLib.has_key('CPPDEFINES'):
          cppDefines = envLib['CPPDEFINES']
       
       if not staticLib :
          builtlib = envLib.SharedLibrary( target, sourceList, 
                                           CPPDEFINES = cppDefines + extraDefines,
                                           LIBS = addToLibs, LINKFLAGS = addToLink + extraLinkFlags )
       else :
          builtlib = envLib.StaticLibrary( target, sourceList, 
                                           CPPDEFINES = cppDefines + extraDefines,
                                           LIBS = addToLibs, LINKFLAGS = addToLink + extraLinkFlags )
           
       #envLib.AddPostAction(builtlib, env.Action ('strip ' + inst[0].get_abspath()))
           
       envLib.Precious( builtlib )
       
       if env['OS'] == 'windows' and not staticLib :
        
          lib = envLib.Install( '#/bin', builtlib[0] ) # .dll
          envLib.Install( '#/lib', builtlib[1] ) # .lib, 
          envLib.Install( '#/lib', builtlib[2] ) # .exp
    
          if env.GetLaunchDir() != env.get('prefix') :
             envLib.Install( '$prefix/bin', builtlib[0] )
             envLib.Install( '$prefix/lib', builtlib[1] )
             envLib.Install( '$prefix/lib', builtlib[2] )
       else:

          # TODO: This should be a build dir settable by command-line params
          lib = envLib.Install( '#/lib', builtlib )

          if env.get('libdir') and env.get('libdir') != env.GetLaunchDir()+'/lib':
             lib = envLib.Install( '$libdir', builtlib )
          elif env.GetLaunchDir() != env.get('prefix'):
             lib = envLib.Install( '$prefix/lib', builtlib )
             
       incDirDict = { 'dtCore'      : 'dtCore',
                      'dtABC'       : 'dtABC',
                      'dtAI'        : 'dtAI',
                      'dtActors'    : 'dtActors',
                      'dtAudio'     : 'dtAudio',
                      'dtAnim'      : 'dtAnim',
                      'dtChar'      : 'dtChar',
                      'dtInputPLIB'  : 'dtInputPLIB',
                      'dtInputISense' : 'dtInputISense',
                      'dtDAL'       : 'dtDAL',
                      'dtDIS'       : 'dtDIS',
                      'dtHLAGM'     : 'dtHLAGM',
                      'dtGUI'       : 'dtGUI',
                      'dtNet'       : 'dtNet',
                      'dtScript'    : 'dtScript',
                      'dtUtil'      : 'dtUtil',
                      'dtBSP'       : 'dtBSP',
                      'dtInspector' : 'dtInspector',
                      'dtTerrain'   : 'dtTerrain',
                      'dtGame'      : 'dtGame',
                      'dtLMS'       : 'dtLMS' }
    
       #install headers, there's gotta be a cleaner way ;)
       if incDirDict.has_key(name):
          if env.get('includedir') and env.GetLaunchDir()+'/inc' != env.get('includedir'):
             for header in env.SGlob('../../inc/'+incDirDict[name]+'/*.h') :
                envLib.InstallAs( '$includedir/'+incDirDict[name]+'/'+os.path.basename(header), '../'+header )          
          elif env.get('prefix') and env.GetLaunchDir() != env.get('prefix'):
             for header in env.SGlob('../../inc/'+incDirDict[name]+'/*.h') :
                envLib.InstallAs( '$prefix/include/'+incDirDict[name]+'/'+os.path.basename(header), '../'+header )
    
       return lib   

   def CreateConf(env, errorLog):
      # append the outside env to ours
      for K in os.environ.keys():
         if K in env['ENV'].keys() and K in [ 'PATH', 'LD_LIBRARY_PATH', 'LIB', 'INCLUDE', 'QTDIR', 'CC', 'CXX', 'CPPPATH', 'LIBPATH' ]:
           env['ENV'][K]=SCons.Util.AppendPath( env['ENV'][K], os.environ[K] )
         else:
           env['ENV'][K]=os.environ[K]

      if os.environ.has_key( 'CC' ) :
         env.Replace( CC = env['ENV']['CC'] )
      if os.environ.has_key( 'CXX' ) :
         env.Replace( CXX = env['ENV']['CXX'] )  
           
      # don't forget the Visual Studio paths...
      if env['OS'] == 'windows' :
         if os.environ.has_key( 'INCLUDE' ) :
           env.Append( CPPPATH = env['ENV']['INCLUDE'].split(os.pathsep) )
         if os.environ.has_key( 'LIB' ) :
           env.Append( LIBPATH = env['ENV']['LIB'].split(os.pathsep) )

         # Python stuff
         vi = sys.version_info   
         python_version = 'python' + str(vi[0]) + str(vi[1])
         
         env.Append( CPPPATH = [os.path.join( sys.prefix, 'include' ) ] ) 
         env.Append( LIBPATH = [os.path.join( sys.prefix, 'libs' ) ] ) 

      else :
         vi = sys.version_info   
         python_version = 'python' + str(vi[0])+'.'+str(vi[1])
         
         # Python stuff
         env.Append( CPPPATH = [os.path.join( sys.prefix, 'include',  python_version ) ] ) 
         env.Append( LIBPATH = [os.path.join( sys.prefix, 'lib',  python_version ) ] ) 
         
      env.Append( CPPPATH = [os.path.join( os.getcwd(), 'inc' ) ] ) # Delta3D includes
      env.Append( CPPPATH = [os.path.join( os.getcwd(), 'ext', 'inc' ) ] ) # Dependencies includes

      env.Append( CPPPATH = env.get('cpppath') )
      env.Append( LIBPATH = env.get('libpath') )

      #add Xerces framework includes in case xerces is a framework
      #since the xerces framework has broken headers.
      #the Crazy eddie's case is similar, but they were wrtten not to have
      #a parent directory, so the framework doesn't really work well with it
      #anyway.
      if env['OS'] == 'darwin':
         env.Append( CPPPATH = [os.path.join( '/Library', 'Frameworks', 'Xerces.framework', 'Headers' ) ] )

      env.Append( LIBPATH = [os.path.join( os.getcwd(), 'lib' ) ] ) # Delta3D libs

      extPath = os.path.join( os.getcwd(), 'ext', 'lib' )
      if os.path.exists(extPath) :
         env.Append( LIBPATH = [extPath] ) # Dependencies libs

      ## Boost Python
      if env.get('boost') not in [ 0, 1 ] and os.path.exists( env.get('boost') ) :
        
         env.Append( CPPPATH = [ env.get('boost') ] )
         
         libPath = os.path.join( env.get('boost'), 'lib' )
         if os.path.exists(libPath) :
            env.Append( LIBPATH = [libPath] )
         else :
            stagePath = os.path.join(env.get('boost'),'stage','lib')
            if os.path.exists(stagePath) :
               env.Append( LIBPATH = [stagePath] )
               
       # RTI 
      rtiLibs = []
      if env.get('rti') not in [ 0, 1 ] and os.path.exists( env.get('rti') ) and os.path.isdir( env.get('rti') ) :
         
         env['rti'] = os.path.abspath(env['rti'])
         
         rtiHeader = find_file( 'RTI.hh', [ env.get('rti') + '/include' ] )
         env.Append( CPPPATH = [ os.path.dirname( rtiHeader ) ] )
         
         #we don't need the java libraries
         javaPattern = '(.*?)java(.*?)'
         
         if env['OS'] == 'windows' :      
           pattern = '(.*?)\.dll'
         elif env['OS'] == 'darwin' :
           pattern = 'lib(.*?)\.dylib'
         else :
           pattern = 'lib(.*?)\.so'

         rtiLibPath = ''
         libDir = env.get('rti') + "/lib"
         for root, dirs, files in os.walk( libDir ) :
            for file in files :
               match = re.search( pattern, file )
               if match is not None :
                  rtiLibPath = root
                  if re.search( javaPattern, file) is None:
                     newLib = match.group(1)
                     rtiLibs.append( newLib )
            
         if rtiLibPath is not '' :
            env.Append( LIBPATH = [ rtiLibPath ] )
            if 'rtiada' in rtiLibs :
               rtiLibs.remove('rtiada')
            if 'rtiadafedtime' in rtiLibs :
               rtiLibs.remove('rtiadafedtime')

      env['rtiLibs'] = rtiLibs

      if env.get('mode') == 'debug':
         mode = 'debug'
      elif env.get('mode') == 'release':
         mode = 'release'
      else:
         print 'Build Failed: Unsupported mode'
         errorLog.write('Build Failed: Unsupportedmode: ' + sys.platform)
         errorLog.close()
         env.Exit(-1)

      print 'OS: ' + env['OS']
      errorLog.write('OS: ' + env['OS'] + '\n\n')

      # Set compiler options for debug/release modes
      if mode == 'debug':
         print 'Build Configuration: Debug'
         errorLog.write('Build Configuration: Debug\n\n')
         
         if env['OS'] == 'windows':
            env.Append( CPPDEFINES = ['WIN32', '_DEBUG', '_NOAUTOLIBMSG'],
                        CXXFLAGS = ['/EHsc', '/GR', '/MDd' ], #synchronous exception handling (c-?), run-time type info, multi-threaded debug dll
                        LINKFLAGS = ['/NODEFAULTLIB:LIBCMTD', '/NODEFAULTLIB:LIBCD'] ) 
         elif env['OS'] == 'linux':      
            env.Append( CXXFLAGS=['-g', '-O0', '-pipe'], #deubg info, no optimizations, pipe object data
                        #removed _DEBUG for now because of problem with GNE and DEBUG because of a python issue.
              CPPDEFINES=['SIGSLOT_PURE_ISO', "LINUX"] )
         elif env['OS'] == 'darwin':      
            env.Append(CXXFLAGS=['-gdwarf-2', '-O0', '-pipe', '-mmacosx-version-min=10.4'], 
              CPPDEFINES=['_DEBUG', 'SIGSLOT_PURE_ISO'])#,
            #LINKFLAGS=['-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk'] )
      else:
         print 'Build Configuration: Release'
         errorLog.write('Build Configuration: Release\n\n')
         
         if env['OS'] == 'windows':
           env.Append( CPPDEFINES = ['WIN32', 'NDEBUG', '_NOAUTOLIBMSG'],
                    CXXFLAGS = ['/EHsc', '/GR', '/MD', '/Ox'], #synchronous exception handling (c-?), run-time type info, multi-threaded dll
                    LINKFLAGS = ['/NODEFAULTLIB:LIBCMT', '/NODEFAULTLIB:LIBC'] )  
         elif env['OS'] == 'linux':
           env.Append( CXXFLAGS=['-O2', '-pipe'], #optimizations, pipe object data
                    CPPDEFINES=['NDEBUG', 'SIGSLOT_PURE_ISO'],
                    LINKFLAGS=['-s'] )
       #             LINKFLAGS=['-Wl,-rpath-link=' + env['ENV']['DELTA_ROOT'] + '/lib,-rpath-link=' + env['ENV']['DELTA_ROOT'] + '/ext/lib'] )

         elif env['OS'] == 'darwin':     
            env.Append(CXXFLAGS=['-Os', '-pipe', '-mmacosx-version-min=10.4'],
               CPPDEFINES=['NDEBUG', 'SIGSLOT_PURE_ISO'])#, 
               #LINKFLAGS=['-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk'] )

      #add to the CPPFLAGS variable if it's set in the environment
      if os.environ.has_key('CPPFLAGS'):
         env.Append(CPPFLAGS=os.environ['CPPFLAGS'])
      if os.environ.has_key('LINKFLAGS'):
         env.Append(LINKFLAGS=os.environ['LINKFLAGS'])

      if env['OS'] == 'windows':
         env.Append(CXXFLAGS=['/W3'])
      elif env['OS'] == 'linux' or env['OS'] == 'darwin':
         if env['CC'].find('gcc') >= 0 or env['CXX'].find('g++') >= 0:
            env.Append(CXXFLAGS=['-Wall'])
            if env.has_key('CCVERSION') :
               if int( env['CCVERSION'][:1] ) >= 4 :
                  env.Append(CXXFLAGS=['-ftree-vectorize'])
            if env.has_key('CXXVERSION') :
               if int( env['CXXVERSION'][:1] ) >= 4 :
                  env.Append(CXXFLAGS=['-ftree-vectorize'])
         if env['CXX'].find('g++') >= 0:
            env.Append(CXXFLAGS=['-Wno-non-virtual-dtor'])

      if env['OS'] == 'linux' :
        env.Append(CPPPATH=['/usr/X11R6/include','/usr/include','/usr/local/include'])
        
        globalCEGUIPath = '/usr/include/CEGUI'
        if os.path.exists( globalCEGUIPath ) and os.path.isdir( globalCEGUIPath ) :
           env.Append(CPPPATH=[globalCEGUIPath])
        else:
           localCEGUIPath = '/usr/local/include/CEGUI'
           if os.path.exists( localCEGUIPath ) and os.path.isdir( localCEGUIPath ) :
              env.Append(CPPPATH=[localCEGUIPath])

        if platform.architecture()[0] == '64bit':
           env.Append(LIBPATH=['/usr/X11R6/lib64','/usr/lib64','/usr/local/lib64'])
        else:
           env.Append(LIBPATH=['/usr/X11R6/lib','/usr/lib','/usr/local/lib'])
      if env['OS'] == 'darwin' :
        env.Append(CPPPATH=['/usr/include','/usr/local/include', '/usr/include/malloc'])
        env.Append(FRAMEWORKPATH=['ext/Frameworks' ])
        env['FRAMEWORKS'] = []
        env.Append(LIBPATH=['/usr/lib','/usr/local/lib'])
        env['SHLINKFLAGS'] = '$LINKFLAGS -Wl,-single_module -dynamiclib -compatibility_version 1 -current_version 4 -install_name "' + os.path.abspath(env.get('prefix')) + '/lib/$SHLIBPREFIX$LIBNAME$SHLIBSUFFIX"'
      
      #############################################
      # Configure: Testing for dependencies 
      ##############################################

      depsHash = {}

      dtLibs = { 'dtABC'                : 'dtABC',
                 'dtAI'                 : 'dtAI',
                 'dtActors'             : 'dtActors',
                 'dtAudio'              : 'dtAudio',
                 'dtAnim'               : 'dtAnim',
                 'dtChar'               : 'dtChar',
                 'dtCore'               : 'dtCore',
                 'dtInputPLIB'          : 'dtInputPLIB',
                 'dtInputISense'        : 'dtInputISense',
                 'dtDAL'                : 'dtDAL',
                 'dtDIS'                : 'dtDIS',
                 'dtHLAGM'              : 'dtHLAGM',
                 'dtGUI'                : 'dtGUI',
                 'dtNet'                : 'dtNet',
                 'dtScript'             : 'dtScript',
                 'dtUtil'               : 'dtUtil',
                 'dtBSP'                : 'dtBSP',
                 'dtInspector'          : 'dtInspector',
                 'dtTerrain'            : 'dtTerrain', 
                 'dtGame'               : 'dtGame',
                 'dtLMS'                : 'dtLMS',
                 'testGameActorLibrary' : 'testGameActorLibrary' }

      if mode == 'debug' and env['OS'] == 'windows' :
         for lib in dtLibs :
            dtLibs[lib] = lib + 'd'
            
      # platform-specific naming of dependencies
      if env['OS'] == 'windows' :
         if mode == 'debug' :
            extLibs =  { 
               'CEGUIBase'           : 'CEGUIBase_d',
               'CEGUIOpenGLRenderer' : 'OpenGLGUIRenderer_d',
               'osg'                 : 'osgd',
               'osgDB'               : 'osgDBd',
               'osgGA'               : 'osgGAd',
               'osgUtil'             : 'osgUtild',
               'osgText'             : 'osgTextd',
               'osgSim'              : 'osgSimd',
               'osgFX'               : 'osgFXd',
               'osgParticle'         : 'osgParticled',
               'osgViewer'           : 'osgViewerd',
               'OpenThreads'         : 'OpenThreadsd',
               'python'              : python_version,
               'cal3d'               : 'cal3d_d',
               'DIS'                 : 'DIS_debug',
               'fltk'                : 'fltkd',
               'gdal'                : 'gdal_i',
               'gne'                 : 'gned',
               'HawkNL'              : 'NLstaticD',
               'isense'              : 'isensed',
               'openal'              : 'OpenAL32', 
               'alut'                : 'alut',       
               'ode'                 : 'oded', 
               'ul'                  : 'ul_d', 
               'js'                  : 'js_d',  
               'rvrutils'            : 'rvrutilsd',
               'rcfgscript'          : 'rcfgscriptd', 
               'rbody'               : 'ReplicantBodyd',
               'xerces-c'            : 'xerces-c_2D',
               'User32'              : 'User32',
               'Advapi32'            : 'Advapi32',
               'Rpcrt4'              : 'Rpcrt4',
               'Winmm'               : 'Winmm',
               'Gdi32'               : 'Gdi32',
               'opengl'              : 'Opengl32',
               'winsock'             : 'wsock32',
               'shell32'             : 'shell32',
               'ole32'               : 'ole32',
               'Comctl32'            : 'Comctl32',
               'cppunit'             : 'cppunit',
               'QtCore'              : 'QtCored4',
               'QtGui'               : 'QtGuid4',
               'QtOpenGL'            : 'QtOpenGLd4'
                
           }
         else :
            extLibs = { 
               'CEGUIBase'           : 'CEGUIBase',
               'CEGUIOpenGLRenderer' : 'OpenGLGUIRenderer',
               'osg'                 : 'osg',
               'osgDB'               : 'osgDB',
               'osgGA'               : 'osgGA',
               'osgUtil'             : 'osgUtil',
               'osgText'             : 'osgText',
               'osgSim'              : 'osgSim',
               'osgFX'               : 'osgFX',
               'osgParticle'         : 'osgParticle',
               'osgViewer'           : 'osgViewer',
               'OpenThreads'         : 'OpenThreads',
               'python'              :  python_version,
               'cal3d'               : 'cal3d',
               'DIS'                 : 'DIS',
               'fltk'                : 'fltk',
               'gdal'                : 'gdal_i',
               'gne'                 : 'gne',
               'HawkNL'              : 'NLstatic',
               'isense'              : 'isense',
               'openal'              : 'OpenAL32', 
               'alut'                : 'alut',       
               'ode'                 : 'ode', 
               'ul'                  : 'ul', 
               'js'                  : 'js',  
               'rvrutils'            : 'rvrutils',
               'rcfgscript'          : 'rcfgscript', 
               'rbody'               : 'ReplicantBody',
               'xerces-c'            : 'xerces-c_2',
               'User32'              : 'User32',
               'Advapi32'            : 'Advapi32',
               'Rpcrt4'              : 'Rpcrt4',
               'Winmm'               : 'Winmm',
               'Gdi32'               : 'Gdi32',
               'opengl'              : 'Opengl32',
               'winsock'             : 'wsock32',
               'shell32'             : 'shell32',
               'ole32'               : 'ole32', 
               'Comctl32'            : 'Comctl32',
               'cppunit'             : 'cppunit',
               'QtCore'              : 'QtCore4',
               'QtGui'               : 'QtGui4',
               'QtOpenGL'            : 'QtOpenGL4'
               }
         
      elif env['OS'] == 'linux' :
            extLibs = { 
                'CEGUIBase'           : 'CEGUIBase',
                'CEGUIOpenGLRenderer' : 'CEGUIOpenGLRenderer',
                'osg'                 : 'osg',
                'osgDB'               : 'osgDB',
                'osgGA'               : 'osgGA',
                'osgUtil'             : 'osgUtil',
                'osgText'             : 'osgText',
                'osgSim'              : 'osgSim',
                'osgFX'               : 'osgFX',
                'osgParticle'         : 'osgParticle',
                'osgViewer'           : 'osgViewer',
                'python'              :  python_version,
                'OpenThreads'         : 'OpenThreads',
                'cal3d'               : 'cal3d',
                'DIS'                 : 'DIS',
                'fltk'                : 'fltk',
                'gdal'                : 'gdal',
                'gne'                 : 'gne',
                'HawkNL'              : 'NL',
                'isense'              : 'isense',
                'openal'              : 'openal',
                'alut'                : 'alut', 
                'ode'                 : 'ode', 
                'ul'                  : 'plibul', 
                'js'                  : 'plibjs',  
                'rvrutils'            : 'rvrutils',
                'rcfgscript'          : 'rcfgscript', 
                'rbody'               : 'rbody',
                'xerces-c'            : 'xerces-c',
                'Xxf86vm'             : 'Xxf86vm',
                'uuid'                : 'uuid',
                'opengl'              : 'GL',
                'cppunit'             : 'cppunit',
                'ncurses'             : 'ncurses',
                'QtCore'              : 'QtCore',
                'QtGui'               : 'QtGui',
                'QtOpenGL'            : 'QtOpenGL'
            }
      elif env['OS'] == 'darwin' :
         extLibs = { 
            'CEGUIBase'           : 'CEGUI',
            'CEGUIOpenGLRenderer' : 'CEGUIOpenGLRenderer',
            'osg'            : 'osg',
            'osgDB'          : 'osgDB',
            'osgGA'          : 'osgGA', 
            'osgUtil'        : 'osgUtil',
            'osgText'        : 'osgText',
            'osgSim'         : 'osgSim',
            'osgFX'          : 'osgFX',
            'osgParticle'    : 'osgParticle',
            'osgViewer'       : 'osgViewer',
      #      'Producer'       : 'Producer',
      #      'python'         : python_version,
      #Default to the system level framework in OS X
            'python'         : 'Python',
            'OpenThreads'    : 'OpenThreads',
            'cal3d'          : 'cal3d',
            'DIS'            : 'DIS',
            'fltk'           : 'fltk',
            'gdal'           : 'gdal',
            'gne'            : 'gne',
            'HawkNL'         : 'NL',
      #      'isense'         : 'isense', # broken on intel
            'openal'         : 'OpenAL', 
      #      'alut'           : 'alut',  # OSX is still on a static alut     
            'ode'            : 'ode', 
            'ul'             : 'plibul', 
            'js'             : 'plibjs',  
            'rbody'          : 'rbody',
            'xerces-c'       : 'Xerces',
            'opengl'         : 'OpenGL',
            'AGL'            : 'AGL',
            'CoreFoundation' : 'CoreFoundation',
            'IOKit'          : 'IOKit',
            'Carbon'         : 'Carbon',
            'cppunit'        : 'cppunit',
            'ncurses'        : 'ncurses',
            'QtCore'         : 'QtCore',
            'QtGui'          : 'QtGui',
            'QtOpenGL'       : 'QtOpenGL'
            }
      else :
         extLibs = {}

      if env.has_key('rtiLibs'):
         for lib in env['rtiLibs']:
             extLibs[lib]=lib
      
      env['depsHash'] = depsHash
      env['dtLibs'] = dtLibs
      env['extLibs'] = extLibs

      conf = env.Configure(custom_tests = {
         'CheckFramework' : CheckFramework,
      })
      
      return conf


   def CompilerConf(env, conf, errorLog):		
      conf.env['LIBS'] = []
      conf.env['FRAMEWORKS'] = []

      depsHash = env['depsHash']
      dtLibs = env['dtLibs']
      extLibs = env['extLibs']

      foundLibs = {}
      
      # TODO: don't run for install
      def CheckLinkGroup(linkGroup, name, required, add=True):
         if env.GetOption('clean') :
            for lib in linkGroup :
               if extLibs.has_key(lib):
                  foundLibs[extLibs[lib]] = 'sharedLib'
         else:
            oldLibs = conf.env['LIBS']
            oldFrameworks = conf.env['FRAMEWORKS']
            for lib in linkGroup :
               if extLibs.has_key(lib):
                  resultText = "notFound"
                  result = False
                  if env['OS'] == 'darwin':
                     result = conf.CheckFramework(extLibs[lib], language = 'C++', autoadd = 1, symbol="printf")
                     if result:
                        resultText = 'framework'
                  else:
                     result = False
   
                  if not result:
                     result = conf.CheckLib(extLibs[lib], language = 'C++', autoadd = 1, symbol="printf");
                     if result:
                        resultText = 'sharedLib'
         
                  foundLibs[extLibs[lib]] = resultText 
            if not add:
               conf.env.Replace(FRAMEWORKS = oldFrameworks) 
               conf.env.Replace(LIBS = oldLibs) 

      def CheckHeader(name, path):
         foundH = conf.CheckCXXHeader(name)
         
         if foundH:
            return True;
          
         for dir in path:
            oldCPPPATH = conf.env['CPPPATH'] + []
            conf.env.Append(CPPPATH = [dir])
            foundH = conf.CheckCXXHeader(name)
            if foundH:
               break;
            else:
               conf.env['CPPPATH'] = oldCPPPATH

         
         return foundH;

      if env['OS'] == 'windows' :
         CheckLinkGroup([ 'opengl', 'User32', 'Advapi32', 'Rpcrt4',  'Winmm', 'Gdi32', 'opengl', 'winsock', 'shell32', 'ole32' ], 'OS', True)
      elif env['OS'] == 'darwin' :
         CheckLinkGroup([ 'opengl', 'AGL', 'CoreFoundation', 'IOKit', 'Carbon', 'ncurses', 'uuid' ], 'OS', True)
      elif env['OS'] == 'linux' :
         CheckLinkGroup([ 'opengl', 'Xxf86vm', 'uuid', 'ncurses' ], 'OS', True)

      CheckLinkGroup(['ode'], 'ode', True, False)

      CheckLinkGroup(['js', 'ul'], 'plib', False, False)
      CheckLinkGroup(['cppunit'], 'cppunit', False, False)
      CheckLinkGroup(['python'], 'python', False, False)
      CheckLinkGroup(['openal', 'alut'], 'openal', True, False)
      #if env['ENV'].has_key('QTDIR'):
      CheckLinkGroup(['QtCore', 'QtGui', 'QtOpenGL'], 'qt', False, False)
      
      CheckLinkGroup(['fltk'], 'fltk', False, False)
      CheckLinkGroup(['isense'], 'isense', False, False) 
   
      CheckLinkGroup([
         'OpenThreads',
         'opengl',
         'xerces-c',
         'gdal',
         'gne',
         'HawkNL',
         'osg',
         'osgDB',
         'osgUtil',
         'osgText',
         'osgSim',
         'osgFX',
         'osgParticle',
         'osgGA',
         'osgViewer'], 'osg', True)
               
      CheckLinkGroup(['cal3d','rvrutils','rcfgscript','rbody'], 'rbody', True)
      CheckLinkGroup(['DIS'], 'DIS', True)
      CheckLinkGroup([ 'CEGUIBase', 'CEGUIOpenGLRenderer' ], 'CEGUI', False, False)

      if env.has_key('rtiLibs'):
         for lib in env['rtiLibs'] :
            extLibs[lib] = lib
            foundLibs[lib] = 'sharedLib'
      
      if env.has_key('additionalLibsOrder'):
         additionalLibsOrder = env['additionalLibsOrder']
         additionalOrderedToCheck = []
         for libName in additionalLibsOrder:
            if not foundLibs.has_key(libName):
               additionalOrderedToCheck += [ libName ]
      
         CheckLinkGroup(additionalOrderedToCheck, 'AdditionalOrdered', True, False)
         
      additionalToCheck = [];
      for libName in extLibs.keys():
         if not foundLibs.has_key(libName):
            additionalToCheck += [ libName ]

      CheckLinkGroup(additionalToCheck, 'Additional', True, False)
         

      
      if not env.GetOption('clean') :
         
         foundnlH = CheckHeader('nl.h', ['/usr/include/hawknl','#ext/inc', '#ext/inc/NL', '#ext/inc/nl', '/usr/local/include/hawknl'])

         if not foundnlH: 
            print "nl.h was not found, aborting."
            env.Exit(1)

         foundGdalH = CheckHeader('gdal.h', ['/usr/include/gdal','#ext/inc/gdal', '/Library/Frameworks/gdal.framework/Headers'])
         
         if not foundGdalH: 
            print "gdal.h was not found, aborting."
            env.Exit(1)

         foundCEGUIH = CheckHeader('CEGUIBase.h', ['/usr/include/CEGUI','#ext/inc/CEGUI', '/Library/Frameworks/CEGUI.framework/Headers'])
         
         if not foundCEGUIH: 
            print "CEGUIBase.h was not found, aborting."
            env.Exit(1)
         
      
      #this actually SEARCHES, not good
      #foundLibs[ os.path.join('boost','python.hpp') ] = conf.CheckHeader( os.path.join('boost','python.hpp'), language='C++')
      
      env = conf.Finish()

      # check if we found all the libraries
      # TODO: it's ok if we are missing boost_python, but make this explicit!
      #if len( foundLibs.keys() ) < len( extLibs.keys() ) - 2  : 
      #   print 'Build Failed: Missing required libraries'
      #   errorLog.write('Build Failed: Missing required libraries\n\n')
      #   errorLog.close()

            
      #   env.Exit(-1)

      # add the rti libs to the external library dictionary, but only
      # after the configure check is complete 

      env['foundLibs'] = foundLibs


########################

   def SGlob(env, pattern):
      path = string.replace(env.GetBuildPath('SConscript'),  'SConscript', '')

      result = []
      for i in glob.glob(path + pattern):
         result.append(string.replace(i, path, ''))

      return result 


########################

   # This is not a regular Builder; it's a wrapper function.
   # So just make it available as a method of Environment.
   SConsEnvironment.BuildProgram = BuildProgram
   SConsEnvironment.BuildLib = BuildLib
   SConsEnvironment.SetupDepsAndPCH = SetupDepsAndPCH
   SConsEnvironment.CompilerConf = CompilerConf
   SConsEnvironment.SGlob = SGlob
   SConsEnvironment.CreateConf = CreateConf

########################

def _lang2suffix(lang):
   """
   Convert a language name to a suffix.
   When 'lang' is empty or None C is assumed.
   Returns a tuple (lang, suffix, None) when it works.
   For an unrecognized language returns (None, None, msg).
   Where:
      lang   = the unified language name
      suffix = the suffix, including the leading dot
      msg    = an error message
   """
   if not lang or lang in ["C", "c"]:
      return ("C", ".c", None)
   if lang in ["c++", "C++", "cpp", "CXX", "cxx"]:
      return ("C++", ".cpp", None)

   return None, None, "Unsupported language: %s" % lang

########################

def CheckFramework(context, frameworks, symbol = 'main', header = None, 
             extra_frameworks = None, extra_libs = None, 
             call = None, language = None, autoadd = 0):
   """
   Configure check for a C or C++ 'frameworks'.  Searches through
   the list of libraries, until one is found where the test succeeds.
   Tests if 'symbol' or 'call' exists in the library.  Note: if it exists
   in another library the test succeeds anyway!
   Optional 'header' can be defined to include a header file.  If not given a
   default prototype for 'symbol' is added.
   Optional 'extra_libs' is a list of library names to be added after
   'lib_name' in the build command.  To be used for libraries that 'lib_name'
   depends on.
   Optional 'call' replaces the call to 'symbol' in the test code.  It must
   consist of complete C statements, including a trailing ';'.
   There must either be a 'symbol' or a 'call' argument (or both).
   'language' should be 'C' or 'C++' and is used to select the compiler.
   Default is 'C'.
   Note that this uses the current value of compiler and linker flags, make
   sure $CFLAGS, $CPPFLAGS and $LIBS are set correctly.
   Returns an empty string for success, an error message for failure.
   """
   if not header:
      header = ""

   text = """
         %s """ % (header)

   # Add a function declaration if needed.
   if symbol and symbol != "main" and not header:
      text = text + """
            #ifdef __cplusplus
            extern "C"
            #endif
            char %s();""" % symbol

   # The actual test code.
   if not call:
      call = "%s();" % symbol
   text = text + """
         int
         main() {
         %s
         return 0;
         }
         \n\n""" % call

   i = string.find(call, "\n")
   if i > 0:
      calltext = call[:i] + ".."
   elif call[-1] == ';':
      calltext = call[:-1]
   else:
      calltext = call

   if not SCons.Util.is_List(frameworks):
      frameworks = [frameworks]


   oldFrameworks = context.env.get('FRAMEWORKS', []) + []
   oldLIBS = context.env.get('LIBS', []) + []

   for framework_name in frameworks:

      lang, suffix, msg = _lang2suffix(language)
      if msg:
         context.Message("Cannot check for framework %s: %s\n" % (framework_name, msg))
         return msg

      context.Message("Checking for %s in %s framework %s... "
                  % (calltext, lang, framework_name))
      
      if framework_name:
         l = [ '-framework',  framework_name ]
         
         if extra_libs:
            context.env.Append(LIBS=extra_libs)
            
         context.env.Append(FRAMEWORKS=[framework_name])
         if extra_frameworks != None:
            context.env.Append(FRAMEWORKS=extra_frameworks)

         sym = "HAVE_LIB" + framework_name
      else:
         sym = None

      ret = context.BuildProg(text, suffix)

      if ret or autoadd == 0:
         context.env.Replace(FRAMEWORKS = oldFrameworks)
      
      if oldLIBS != []:
         context.env.Replace(LIBS = oldLIBS)
     
   context.Result(not ret)
   return not ret

########################

def _Have(context, key, have):
   """
   Store result of a test in context.havedict and context.headerfilename.
   'key' is a 'HAVE_abc' name.  It is turned into all CAPITALS and ':./' are
   replaced by an underscore.
   The value of 'have' can be:
   1     - Feature is defined, add '#define key'.
   0     - Feature is not defined, add '/* #undef key */'.
          Adding 'undef' is what autoconf does.  Not useful for the
          compiler, but it shows that the test was done.
   number - Feature is defined to this number '#define key have'.
          Doesn't work for 0 or 1, use a string then.
   string - Feature is defined to this string '#define key have'.
          Give 'have' as is should appear in the header file, include quotes
          when desired and escape special characters!
   """
   key_up = string.upper(key)
   key_up = string.replace(key_up, ':', '_')
   key_up = string.replace(key_up, '.', '_')
   key_up = string.replace(key_up, '/', '_')
   key_up = string.replace(key_up, ' ', '_')
   context.havedict[key_up] = have
   if context.headerfilename:
      f = open(context.headerfilename, "a")
      if have == 1:
         f.write("#define %s\n" % key_up)
      elif have == 0:
         f.write("/* #undef %s */\n" % key_up)
      elif type(have) == IntType:
         f.write("#define %s %d\n" % (key_up, have))
      else:
         f.write("#define %s %s\n" % (key_up, str(have)))
      f.close()

########################

def _LogFailed(context, text, msg):
   """
   Write to the log about a failed program.
   Add line numbers, so that error messages can be understood.
   """
   context.Log("Failed program was:\n")
   lines = string.split(text, '\n')
   if len(lines) and lines[-1] == '':
      lines = lines[:-1]           # remove trailing empty line
   n = 1
   for line in lines:
      context.Log("%d: %s\n" % (n, line))
      n = n + 1
   context.Log("Error message: %s\n" % msg)

########################

def find_file( findThis, startDirs ) :
   for startDir in startDirs :
     for root, dirs, files in os.walk( startDir ) :
       if findThis in files :
         return os.path.join( root, findThis )
   return None

########################

def exit_with_error() :
   sys.stderr.write('Build failed.  See BuildLog.txt for details.\n')
   errorLog.write('Build Failed: Build ended: ' + CurrentTime() + '\n')
   errorLog.close()
   env.Exit(-1)

