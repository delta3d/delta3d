//______________________________________________________________
// Framework interface declaration file
// 10/08/2003

#ifndef DELTA_SOARX_FRAMEWORK
#define DELTA_SOARX_FRAMEWORK

#include <typeinfo>
#include "soarx/soarx_definitions.h"

namespace dtSOARX
{
   //______________________________________________________________
   // class declarations:

   class Callback;
   class ISystem;
   class IOpenGL;
   class IOpenAL;
   class IImage;
   class IFont2D;
   class IFont3D;

   //______________________________________________________________
   // The system interface is globally accessible to every moudule
   // and provides basic functionality.

   class ISystem
   {
   protected:
	   virtual ~ISystem() = 0;
   public:
	   virtual void Log(const char* format, ... ) = 0;
	   virtual void OverlayNote(const char* format, ... ) = 0;
	   virtual void OverlayWrite(int row, const char* format, ... ) = 0;
	   virtual float GetFPS() = 0;
	   virtual void* GetInterface(const char* path) = 0;
	   virtual int SetBasePath(const char* path) = 0;
	   virtual char* Load(const char* path) = 0;
	   virtual void GetMouseMovement(double& x, double& y) = 0;
	   virtual int GetEvent(const char* key) = 0;
	   virtual int Subscribe(const char* key, Callback callback) = 0;
	   virtual void Unsubscribe(const char* key, Callback callback) = 0;
	   virtual void UnsubscribeAll(const char* key) = 0;
	   virtual int CallSubscribers(int event) = 0;
	   virtual int CallSubscribers(int event, const char* format, ... ) = 0;
	   virtual int ExecuteFile(const char* path) = 0;
	   virtual int ExecuteString(const char* format, ...) = 0;
	   virtual double GetGlobalNumber(const char* key) = 0;
	   virtual const char* GetGlobalString(const char* key) = 0;
	   virtual void GetGlobals(const char* keys, const char* format, ... ) = 0;
   };

   //______________________________________________________________
   // The IOpenGL interface reflects the OpenGL API, and enables
   // easy access to the extensions. It also provides various
   // utility functions.

   class IOpenGL
   {
   protected:
	   virtual ~IOpenGL() = 0;

   public:
	   // basic
	   virtual GLvoid Enable(GLenum cap) = 0;
	   virtual GLvoid EnableClientState(GLenum array) = 0;
	   virtual GLvoid Begin(GLenum mode) = 0;
	   virtual GLvoid End(void) = 0;
	   virtual GLvoid BlendFunc(GLenum sfactor, GLenum dfactor) = 0;
	   virtual GLvoid Clear(GLbitfield mask) = 0;
	   virtual GLvoid ClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = 0;
	   virtual GLvoid ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) = 0;
	   virtual GLvoid ClearDepth(GLclampd depth) = 0;
	   virtual GLvoid ClearIndex(GLfloat c) = 0;
	   virtual GLvoid ClearStencil(GLint s) = 0;
	   virtual GLvoid Color4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha) = 0;
	   virtual GLvoid Color4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha) = 0;
	   virtual GLvoid Disable(GLenum cap) = 0;
	   virtual GLvoid DisableClientState(GLenum array) = 0;
	   virtual GLvoid DepthFunc(GLenum func) = 0;
	   virtual GLvoid DepthMask(GLboolean flag) = 0;
	   virtual GLvoid DepthRange(GLclampd zNear, GLclampd zFar) = 0;
	   virtual GLvoid Fogf(GLenum pname, GLfloat param) = 0;
	   virtual GLvoid Fogfv(GLenum pname, const GLfloat *params) = 0;
	   virtual GLvoid Fogi(GLenum pname, GLint param) = 0;
	   virtual GLvoid Fogiv(GLenum pname, const GLint *params) = 0;
	   virtual GLvoid FrontFace(GLenum mode) = 0;
	   virtual GLvoid PolygonMode(GLenum face, GLenum mode) = 0;
	   virtual GLvoid PolygonOffset(GLfloat factor, GLfloat units) = 0;
	   virtual GLvoid PolygonStipple(const GLubyte *mask) = 0;

	   virtual GLvoid Color4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = 0;
	   virtual GLvoid Color4fv(const GLfloat *v) = 0;
	   virtual GLvoid CullFace(GLenum mode) = 0;
	   virtual GLvoid CopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border) = 0;
	   virtual GLvoid CopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) = 0;
	   virtual GLvoid CopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) = 0;
	   virtual GLvoid CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) = 0;
	   virtual GLvoid Vertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) = 0;
	   virtual GLvoid Vertex4fv(const GLfloat *v) = 0;
	   virtual GLvoid Vertex4dv(const GLdouble *v) = 0;
	   virtual GLvoid TexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q) = 0;

	   virtual GLvoid MatrixMode(GLenum mode) = 0;
	   virtual GLvoid PopMatrix(void) = 0;
	   virtual GLvoid PushMatrix(void) = 0;
	   virtual GLvoid VertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) = 0;
	   virtual GLvoid LoadIdentity(void) = 0;


	   // transpose matrix
	   virtual void LoadTransposeMatrixf(const float* m) = 0;
	   virtual void LoadTransposeMatrixd(const double* m) = 0;
	   virtual void MultTransposeMatrixf(const float* m) = 0;
	   virtual void MultTransposeMatrixd(const double* m) = 0;

	   // texturing
	   virtual void ActiveTexture (GLenum texture) = 0;
	   virtual GLvoid GenTextures (GLsizei n, GLuint *textures) = 0;
	   virtual GLvoid DeleteTextures (GLsizei n, const GLuint *textures) = 0;
	   virtual GLvoid BindTexture (GLenum target, GLuint texture) = 0;
	   virtual GLvoid TexEnvf (GLenum target, GLenum pname, GLfloat param) = 0;
	   virtual GLvoid TexEnvfv (GLenum target, GLenum pname, const GLfloat *params) = 0;
	   virtual GLvoid TexEnvi (GLenum target, GLenum pname, GLint param) = 0;
	   virtual GLvoid TexEnviv (GLenum target, GLenum pname, const GLint *params) = 0;
	   virtual GLvoid TexGend (GLenum coord, GLenum pname, GLdouble param) = 0;
	   virtual GLvoid TexGendv (GLenum coord, GLenum pname, const GLdouble *params) = 0;
	   virtual GLvoid TexGenf (GLenum coord, GLenum pname, GLfloat param) = 0;
	   virtual GLvoid TexGenfv (GLenum coord, GLenum pname, const GLfloat *params) = 0;
	   virtual GLvoid TexGeni (GLenum coord, GLenum pname, GLint param) = 0;
	   virtual GLvoid TexGeniv (GLenum coord, GLenum pname, const GLint *params) = 0;
	   virtual GLvoid TexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels) = 0;
	   virtual GLvoid TexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) = 0;
	   virtual GLvoid TexParameterf (GLenum target, GLenum pname, GLfloat param) = 0;
	   virtual GLvoid TexParameterfv (GLenum target, GLenum pname, const GLfloat *params) = 0;
	   virtual GLvoid TexParameteri (GLenum target, GLenum pname, GLint param) = 0;
	   virtual GLvoid TexParameteriv (GLenum target, GLenum pname, const GLint *params) = 0;
	   virtual GLvoid TexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels) = 0;
	   virtual GLvoid TexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) = 0;

	   // draw range elements
	   virtual void DrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices) = 0;

	   // vertex buffer object
	   virtual GLvoid BindBuffer(GLenum target, GLuint buffer) = 0;
	   virtual GLvoid DeleteBuffers(GLsizei n, const GLuint *buffers) = 0;
	   virtual GLvoid GenBuffers(GLsizei n, GLuint *buffers) = 0;
	   virtual GLboolean IsBuffer(GLuint buffer) = 0;
	   virtual GLvoid BufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) = 0;
	   virtual GLvoid BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data) = 0;
	   virtual GLvoid GetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data) = 0;
	   virtual GLvoid *MapBuffer(GLenum target, GLenum access) = 0;
	   virtual GLboolean UnmapBuffer(GLenum target) = 0;
	   virtual GLvoid GetBufferParameteriv(GLenum target, GLenum pname, GLint *params) = 0;
	   virtual GLvoid GetBufferPoGLinterv(GLenum target, GLenum pname, GLvoid **params) = 0;

   public:
	   // Utility functions:
	   virtual int ResetTextureUnits() = 0;
	   virtual int LoadTexture(const char* path) = 0;
	   virtual int SaveScreen() = 0;
	   virtual bool IsExtensionSupported(const char *string) = 0;
   };

   //______________________________________________________________
   // The IOpenAL interface provides basic audio services.

   class IOpenAL
   {
   protected:
	   virtual ~IOpenAL() = 0;
   public:
	   virtual int Play() = 0;
	   virtual int LoadMP3(const char* path, int secs) = 0;
   };

   //______________________________________________________________
   // The IImage interface allows reading, saving and processing
   // various image formats.

   class IImage
   {
   public:
	   struct Dimensions
	   {
		   int width;
		   int height;
		   int depth;
		   int channels;
		   int bpc;
	   };

	   typedef int (*ImageProc)(int ch, int cx, int cy, int cz, int value, void* ptr);
	   virtual ~IImage() = 0;
	   virtual int Create(IImage::Dimensions d) = 0;
	   virtual int ProcessImage(ImageProc fp, void* ptr) = 0;
	   virtual int ProcessLine(int line, ImageProc fp, void* ptr) = 0;
	   virtual	int CreateGradient(IImage* src_image, float scale) = 0;
	   virtual int Load(const char* path) = 0;
	   virtual int LoadInfo(const char* path) = 0;
	   virtual int Save(const char* path) = 0;
	   virtual char* GetData() = 0;
	   virtual IImage::Dimensions GetDimensions() = 0;
   };

   //______________________________________________________________
   // 2D texture font

   class IFont2D
   {
   public:
	   virtual ~IFont2D() = 0;
	   virtual int Create(const char* face, int font_height = 24, int texture_width = 256, int texture_height = 256) = 0;
	   virtual int SetLineFeed(float linefeed = -1) = 0;
	   virtual int SetColor(float r, float g, float b, float a) = 0;
	   virtual int Print(const char* s, ...) = 0;
	   virtual int Test() const = 0;
   };

   //______________________________________________________________
   // 3D font

   class IFont3D
   {
   public:
	   virtual ~IFont3D() = 0;
	   virtual int Create(const char* face) = 0;
	   virtual int Print(const char* s) = 0;
   };

   //______________________________________________________________
   // Every module has to implement this factory function!

   typedef void* (TFactory)(ISystem*);
   extern TFactory Factory;


   //______________________________________________________________
   // The Callback class is responsible for storing important data
   // about registered functions like signature, entry point and
   // object pointer.

   class Callback
   {
   public:
	   void* m_object_ptr;
	   void* m_function_ptr;
	   char* m_function_sig;
	   //const type_info* m_function_type;
           const std::type_info* m_function_type;

   public:
	   template<typename T>
	   Callback(void* i_object, T i_function)
	   {
                   #ifdef _WIN32
		   __asm {
                           mov eax, [i_object]
			   mov [ecx], eax
			   mov eax, [i_function]
			   mov [ecx+4], eax
			   mov DWORD PTR [ecx+8], 0
		   }
                   #else
                   asm("movw (i_object),%eax\n\tmovw %eax,(%ecx)\n\tmovw (i_function), %eax\n\tmovw %eax,4(%ecx)\n\tmovl $0,8(%ecx)");
                   #endif
		   m_function_type = &typeid(T);
	   }

	   bool operator == (const Callback& c) const
	   {
		   return (m_object_ptr == c.m_object_ptr) && (m_function_ptr == c.m_function_ptr);
	   }
   };
};

//______________________________________________________________
// This convenience macro makes function registering easier:

#define REGISTER(event, function) sys->Subscribe(#event, Callback(this, function));

//______________________________________________________________

#endif // DELTA_SOARX_FRAMEWORK
