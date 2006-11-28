import glob
import os 
import shutil
import re
import sys
import time
import smtplib
import string
import SCons.Util
import delta3d
import macosx
import gch
import platform
from SCons.Script.SConscript import SConsEnvironment

from email.MIMEText import MIMEText

# TODO:
# install headers, libs, to prefix
# fix dtCore/dtUtil auto-install
# doxygen
# make packaged release
# CVS support
   
########################
# Build error emailing #
########################

# set this to true to enable build emailing
email = 'false'

# fill this in with interested parties
peopleToEmail = []

# replcae with your local outgoing SMTP server
smtpServer = 'mysmtp@server.com'

SetOption('implicit_cache', 1)

# this is who the build email will come from
fromAddress = 'myaddress@company.com'

############################

# Function to calculate current time for logging
def CurrentTime() :
   now = time.localtime()
   display = time.strftime('%A, %B %d %Y, %X', now)
   return display

########################

# Function to email the result of the build
def EmailResults() :
	file = open('BuildLog.txt', 'r')
	buffer = MIMEText(file.read())
	buffer['Subject'] = 'Editor Build Log'
	file.close()

	s = smtplib.SMTP()
	s.connect(smtpServer) 
	s.sendmail(fromAddress, peopleToEmail, buffer.as_string())
	s.close()

########################

def CheckForBoost() :

   boostHeader = os.path.join('boost','python.hpp')
   
   if OS == 'windows' :
      boostLib = 'boost_python-vc71-mt.lib'
   elif OS == 'darwin' : 
      # This only has a hard-coded version since Boost.Python does not output
      # with the same file naming convention as other platform (as of 1.33.1)
      boostLib = 'libboost_python-1_33_1.dylib'
   else :
      boostLib = 'libboost_python-gcc-mt.so'
      
   foundLib = 0
   for dir in env['LIBPATH'] :
      if os.path.isfile( os.path.join( dir, boostLib ) ) :
         foundLib = 1
         break

   foundHeader = 0
   for dir in env['CPPPATH'] :
      if os.path.isfile( os.path.join( dir, boostHeader ) ) :
         foundHeader = 1
         break

   # If we don't find the lib name for Boost.Python 1.33 onward,
   # try the old library name instead...
   #if OS == 'linux' and foundLib == 0 :
   #   boostLib = 'libboost_python.so'

   #   for dir in env['LIBPATH'] :
   #      if os.path.isfile( os.path.join( dir, boostLib ) ) :
   #         foundLib = 1
   #         break

   return foundHeader != 0 and foundLib != 0

###########
# Options #
###########

# Open the error log
errorLog = open('BuildLog.txt', 'w')

print 'Build started: ' + CurrentTime() + ''
errorLog.write('Build started: ' + CurrentTime() + '\n\n')

print 'Platform: ' + sys.platform 

EnsureSConsVersion(0,96)

# detemine the OS
if sys.platform == 'win32' :
   OS = 'windows'	
elif sys.platform == 'linux2' or sys.platform == 'linux-i386' :
   OS = 'linux'
elif sys.platform == 'darwin' :
   OS = 'darwin'
else :
   print 'Build Failed: Unsupported platform'
   errorLog.write('Build Failed: Unsupported platform: ' + sys.platform)
   errorLog.close()
   env.Exit(-1) 

tools = ["default", "gch"]

if OS == 'darwin':
   tools.append(macosx.TOOL_BUNDLE)

tools.append(delta3d.TOOL_BUNDLE)

# some basic command-line options
optCache = 'options.cache'
opts = Options(optCache)
opts.AddOptions(
   EnumOption( 'mode', 'Build as either debug or release', 'release',
            allowed_values = ( 'debug', 'release' ),
            map = {}, ignorecase = 1 ),
   BoolOption( 'usePrefixHeaders', 'use a prefix/precompiled header', False ),
   BoolOption( 'buildPythonBindings', 'compile the python binding libraries', True ),
   PackageOption( 'boost', 'Boost installation directory', 'no' ),
   PackageOption( 'rti', 'RTI installation directory', 'no' )
)

def CommaConverter( value ) :
   return value.split(',')

opts.Add( 'cpppath', 'Additional include directories (comma delimited)', converter=CommaConverter  )
opts.Add( 'libpath', 'Additional library directories (comma delimited)', converter=CommaConverter  )

if OS == 'windows' :
   opts.Add( PathOption( 'prefix', 'Directory to install under', os.getcwd() ) )
elif OS == 'linux':

   defaultLibDir = 'lib'
   if platform.architecture()[0] == '64bit' :
      defaultLibDir = 'lib64'
   
   opts.Add( PathOption( 'prefix', 'Directory to install under', '/usr/local' ) )
   opts.Add( PathOption( 'libdir', 'Directory to install libraries under (if set, this will override the normal prefix option)', '/usr/local/'+defaultLibDir ) )
   opts.Add( PathOption( 'bindir', 'Directory to install executables under (if set, this will override the normal prefix option)', '/usr/local/bin' ) )
   opts.Add( PathOption( 'includedir', 'Directory to install headers under (if set, this will override the normal prefix option)', '/usr/local/include' ) )
else :
   opts.Add( PathOption( 'prefix', 'Directory to install under', '/usr/local' ) )

env = Environment( options = opts, tools = tools, toolpath = "." )

env['OS'] = OS

env.CompilerConf(errorLog)

Help( opts.GenerateHelpText( env ) )
opts.Save( 'delta3d.conf', env )


if env.get('mode') == 'debug':
   mode = 'debug'
elif env.get('mode') == 'release':
   mode = 'release'


#################
# Build Delta3D #
#################

buildProductsDir='#build'

Export( 'env', 'mode', 'CheckForBoost', 'errorLog', 'buildProductsDir' )

env.SConsignFile()

Default( [ 'lib', 'bin', 'tests' ] )
if OS == 'darwin':
   Default(['Applications'])

env.Alias( 'python', [ os.path.join(os.getcwd(), 'src', 'python'),
                  os.path.join(os.getcwd(), 'examples', 'testPython')] )

env.Alias( 'hla', [ os.path.join(os.getcwd(), 'src', 'dtHLA'),
                os.path.join(os.getcwd(), 'examples', 'testHLA'),
                os.path.join(os.getcwd(), 'utilities', 'hlaStealthViewer') ] )

env.Alias( 'install', '$prefix' )

SConscript( dirs=['doc'] )
SConscript( dirs=['src'] )
SConscript( dirs=['examples'] )
SConscript( dirs=['demos'] )
SConscript( dirs=['utilities'] )

# TODO: this should defintely be using the env['TOOLS'] list and extracting the 'default' cxx tool,
#       problem is that on win32 the c_compiler and cxx_compiler are the same, so there's one
#       less element in the list, so we can't use an index to find the proper tool. instead we
#       need to query what scons chooses from the list and actually uses...
if OS == 'windows' :
   tool = 'msvc'
elif OS == 'linux' or OS == 'darwin' :
   tool = 'g++'

SConscript( dirs=['tests'], build_dir = os.path.join( buildProductsDir, 'tests', OS+'-'+tool+'-'+mode ), duplicate = 0 )

#if libs == '[]' :
#   errorLog.write('Failed to create the libraries or executable file')
#   errorLog.write('Build Failed: Build ended: ' + CurrentTime() + '\n')
#   errorLog.close()
#   env.Exit(-1)
#else :
#   # Close the error log and print the results
#   print 'Build Succeeded: Build ended: ' + CurrentTime()
#   errorLog.write('Build Succeeded: Build ended: ' + CurrentTime() + '\n')
#   errorLog.close()
#
#   # email the results
#   if email == 'true' :
#     EmailResults() 
