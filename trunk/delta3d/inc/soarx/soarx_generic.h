//______________________________________________________________
// File: generic.h
// Modified: 10/09/2003

//______________________________________________________________
#ifndef DELTA_SOARX_GENERIC
#define DELTA_SOARX_GENERIC

#include <cmath>
#include <cstdio>
#include "soarx_configure.h"

//______________________________________________________________
// Compiler settings and definitions:

#define align(k) __declspec(align(k))
#undef max
#undef min

namespace dtSOARX
{
   //______________________________________________________________
   // Class declarations:

   class Random;
   class Curve;
   class Noise;
   template <class T> class Vector;
   template <class T> class Matrix;

   //______________________________________________________________
   // Typedefs:

   typedef signed char i8;
   typedef signed short int i16;
   typedef signed long int i32;
   #ifdef _WIN32
   typedef signed __int64 i64;
   #else
   typedef signed long i64;
   #endif
   typedef unsigned char u8;
   typedef unsigned short int u16;
   typedef unsigned long int u32;
   #ifdef _WIN32
   typedef unsigned __int64 u64;
   #else
   typedef unsigned long u64;
   #endif
   typedef float f32;
   typedef double f64;
   typedef long double f80;
   typedef Vector<double> v4d;
   typedef Vector<float> v4f;
   typedef Vector<int> v4i;
   typedef Vector<char> v4c;
   typedef Vector<unsigned char> v4b;
   typedef Matrix<double> m4d;
   typedef Matrix<float> m4f;

   //______________________________________________________________
   // Constants:

   const float PI = float(3.141592653589793238462643383279502884197169399375105820974944);
   const float DEG2RAD = float(3.141592653589793238462643383279502884197169399375105820974944 / 180.0);
   const float RAD2DEG = float(180.0 / 3.141592653589793238462643383279502884197169399375105820974944);
   const float INF = float(1.0e+10);
   const float EPS = float(1.0e-10);
   const float ZERO = float(0.0);
   const float ONE = float(1.0);

   //______________________________________________________________
   // Generic helper functions:

   template<class T> inline void release(T& t) { delete t; t = 0; }
   template<class T> inline void releasev(T& t) { delete [] t; t = 0; }
   template<class T> inline T deg2rad(T t) {return T(t * DEG2RAD);}
   template<class T> inline T rad2deg(T t) {return T(t * RAD2DEG);}
   template<class T> inline T rnd(T t) {return T(t*(rand()%2000-1000)/1000.0);}
   template<class T> inline T rcp(T t) {return T(1) / t;}
   template<class T> inline T lerp(T t, T a, T b) {return a + t * (b - a);}
   template<class T> inline T abs(T t) {return t>=0 ? t : -t;}
   template<class T> inline T sgn(T t) {return t>=0 ? 1 : -1;}
   template<class T> inline T max(T a, T b) {return a>b ? a : b;}
   template<class T> inline T min(T a, T b) {return a>b ? b : a;}
   template<class T> inline T clamp(T a, T b) {return (a>0) ? (a>b ? b:a) : (a<-b ? -b:a);}
   template<class T> inline T clamp(T a, T min, T max) {return (a>=min) ? (a<=max ? a:max) : min;}

   //______________________________________________________________
   // Random number generator, with adjustable distribution

   class Random
   {
	   Random(float szoras, float varhatoertek);
	   ~Random();
	   int SetSeed(int seed);
	   float GetLinear();
	   float GetNormal();
   };

   //______________________________________________________________
   // Curve fitting class: ??

   class Curve
   {
   };

   //______________________________________________________________
   // Basic fixed point math routines:

   inline int i2f(int a) {return a<<12;}
   inline int f2i(int a) {return a>>12;}
   inline float f2f(int a) {return a * (1.0f/4096.0f);}
   inline int imul(int a, int b) {return (a*b)>>12;}
   inline int ilerp(int t, int a, int b) {return a + imul(t,(b-a));}
   inline int fade(int t) {return imul(imul(imul(t, t), t), (imul(imul(i2f(6), t) - (i2f(15)), t)+(i2f(10))));}

   //______________________________________________________________
   // Fixed point implementation of Improved Perlin Noise generator:
   class Noise
   {
   private:

	   int p(int i)
	   {
         static int permutation[] = { 151,160,137,91,90,15,
			   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
			   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
			   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
			   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
			   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
			   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
			   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
			   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
			   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
			   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180, 151
		   };

		   return permutation[i];
	   }


	   int grad(int h, int x, int y, int z)
	   {
		   h &= 15;
		   int u = (h & 8) ? y : x;
		   int v = (h < 4) ? y : (((h & 14) == 12) ? x : z);
		   return ((h & 2) ? -u : u) + ((h & 1) ? -v : v);
	   }

	   int grad(int h, int x, int y)
	   {
		   h &= 15;
		   int u = (h & 8) ? y : x;
		   int v = (h < 4) ? y : (((h & 14) == 12) ? x : 0);
		   return ((h & 2) ? -u : u) + ((h & 1) ? -v : v);
	   }

	   int grad(int h, int x)
	   {
		   h &= 15;
		   int u = (h & 8) ? 0 : x;
		   int v = (h < 4) ? 0 : (((h & 14) == 12) ? x : 0);
		   return ((h & 2) ? -u : u) + ((h & 1) ? -v : v);
	   }

   public:

	   // 3D Noise:
	   int operator()(int x, int y, int z)
	   {
		   const int I = (1<<12);

		   unsigned char gx = x >> 12;
		   unsigned char gy = y >> 12;
		   unsigned char gz = z >> 12;

		   x &= I-1;
		   y &= I-1;
		   z &= I-1;

		   int fx = fade(x);
		   int fy = fade(y);
		   int fz = fade(z);

		   unsigned char p0 = p(gx+0)+gy;
		   unsigned char p1 = p(gx+1)+gy;

		   unsigned char p00 = p(p0+0)+gz;
		   unsigned char p01 = p(p0+1)+gz;
		   unsigned char p10 = p(p1+0)+gz;
		   unsigned char p11 = p(p1+1)+gz;

		   unsigned char p000 = p(p00+0);
		   unsigned char p001 = p(p00+1);
		   unsigned char p010 = p(p01+0);
		   unsigned char p011 = p(p01+1);
		   unsigned char p100 = p(p10+0);
		   unsigned char p101 = p(p10+1);
		   unsigned char p110 = p(p11+0);
		   unsigned char p111 = p(p11+1);

		   int noise = 
			   ilerp(fz,
				   ilerp(fy,
					   ilerp(fx, 
						   grad(p000, x, y, z),
						   grad(p100, x-I, y, z)
					   ),
					   ilerp(fx, 
						   grad(p010, x, y-I, z), 
						   grad(p110, x-I, y-I, z)
					   )
				   ),
				   ilerp(fy,
					   ilerp(fx, 
						   grad(p001, x, y, z-I),
						   grad(p101, x-I, y, z-I)
					   ),
					   ilerp(fx, 
						   grad(p011, x, y-I, z-I), 
						   grad(p111, x-I, y-I, z-I)
					   )
				   )
			   );
   		
		   return noise;
	   }

	   // 2D Noise:
	   int operator()(int x, int y)
	   {
		   const int I = (1<<12);

		   unsigned char gx = x >> 12;
		   unsigned char gy = y >> 12;

		   x &= I-1;
		   y &= I-1;

		   int fx = fade(x);
		   int fy = fade(y);

		   unsigned char p0 = p(gx+0)+gy;
		   unsigned char p1 = p(gx+1)+gy;

		   unsigned char p00 = p(p0+0);
		   unsigned char p01 = p(p0+1);
		   unsigned char p10 = p(p1+0);
		   unsigned char p11 = p(p1+1);

		   int noise = 
			   ilerp(fy,
				   ilerp(fx, 
					   grad(p00, x, y),
					   grad(p10, x-I, y)
				   ),
				   ilerp(fx, 
					   grad(p01, x, y-I), 
					   grad(p11, x-I, y-I)
				   )
			   );
   		
		   return noise;
	   }

	   // 1D Noise:
	   int operator()(int x)
	   {
		   const int I = (1<<12);

		   unsigned char gx = x >> 12;

		   x &= I-1;

		   int fx = fade(x);

		   unsigned char p0 = p(gx+0);
		   unsigned char p1 = p(gx+1);

		   int noise = 
			   ilerp(fx, 
				   grad(p0, x),
				   grad(p1, x-I)
			   );
   		
		   return noise;
	   }

   };

   //______________________________________________________________
   template <class T>
   class Vector
   {
   public:
	   union {
		   T v[4];
		   struct {
			   T x;
			   T y;
			   T z;
			   T w;
		   };
	   };

	   Vector(): x(0), y(0), z(0), w(1) {}
	   Vector(const T ix, const T iy = 0, const T iz = 0, const T iw = 1): x(ix), y(iy), z(iz), w(iw) {}
	   operator T* () {return v;}
	   void operator() (const Vector u) {x=u.x, y=u.y, z=u.z, w=u.w;}
	   void operator() (const T ix, const T iy = 0, const T iz = 0, const T iw = 1) {x=ix, y=iy, z=iz, w=iw;}
	   Vector operator& (const Vector u) const {return Vector(x&u.x, y&u.y, z&u.z, w&u.w);}
	   Vector operator| (const Vector u) const {return Vector(x|u.x, y|u.y, z|u.z, w|u.w);}
	   Vector operator^ (const Vector u) const {return Vector(x^u.x, y^u.y, z^u.z, w^u.w);}
	   Vector operator+ (const Vector u) const {return Vector(x+u.x, y+u.y, z+u.z, w+u.w);}
	   Vector operator- (const Vector u) const {return Vector(x-u.x, y-u.y, z-u.z, w-u.w);}
	   Vector operator* (const Vector u) const {return Vector(x*u.x, y*u.y, z*u.z, w*u.w);}
	   Vector operator* (const T f) const {return Vector(x*f, y*f, z*f, w*f);}
	   Vector operator- () {return Vector(-x, -y, -z, -w);}
	   void operator&= (const Vector u) {x&=u.x, y&=u.y, z&=u.z, w&=u.w;}
	   void operator|= (const Vector u) {x|=u.x, y|=u.y, z|=u.z, w|=u.w;}
	   void operator^= (const Vector u) {x^=u.x, y^=u.y, z^=u.z, w^=u.w;}
	   void operator+= (const Vector u) {x+=u.x, y+=u.y, z+=u.z, w+=u.w;}
	   void operator-= (const Vector u) {x-=u.x, y-=u.y, z-=u.z, w-=u.w;}
	   void operator*= (const Vector u) {x*=u.x, y*=u.y, z*=u.z, w*=u.w;}
	   void operator*= (const T f) {x*=f, y*=f, z*=f, w*=f;}

	   void translate(const Vector u) {x+=u.x, y+=u.y, z+=u.z, w+=u.w;}

	   void load(const T* u) {x=u[0], y=u[1], z=u[2], w=u[3];}
	   T dprod(const Vector u) const {return x*u.x + y*u.y + z*u.z + w*u.w;}
	   T length() const {return T(sqrtf(x*x+y*y+z*z));}

	   void abs() {x=::abs(x), y=::abs(y), z=::abs(z), w=::abs(w);}
	   void negate() {x = -x; y = -y; z = -z; w = -w;}
	   void clamp(T f) {x = ::clamp(x, f), y = ::clamp(y, f), z = ::clamp(z, f), w = ::clamp(w, f); }
	   void clamp(T min, T max) {x = ::clamp(x, min, max), y = ::clamp(y, min, max), z = ::clamp(z, min, max), w = ::clamp(w, min, max); }
	   void clamp(Vector& u) {x = ::clamp(x, u.x), y = ::clamp(y, u.y), z = ::clamp(z, u.z), w = ::clamp(w, u.w); }
	   void max(Vector& u) {x = ::max(x, u.x), y = ::max(y, u.y), z = ::max(z, u.z), w = ::max(w, u.w); }
	   void scale(T f) {x*=f, y*=f, z*=f, w*=f;}
	   void setlength(T f) {scale(f/length());}
	   //void minlength(T f) {T l=length(); (l<f ? scale(f/l): ); }
           void minlength(T f) {T l=length(); if(l<f){ scale(f/l); } }
	   //void maxlength(T f) {T l=length(); (l>f ? scale(f/l): ); }
           void maxlength(T f) {T l=length(); if(l>f){ scale(f/l); } }
	   void normalize() {setlength(1); }

	   void xprod(const Vector a, const Vector b) {x=a.y*b.z-a.z*b.y; y=a.z*b.x-a.x*b.z; z=a.x*b.y-a.y*b.x;}
	   void hdiv() {scale(1/w);}

	   void transform(T* m)
	   {
		   Vector<T> t;

		   t.x = x*m[ 0] + y*m[ 1] + z*m[ 2] + w*m[ 3];
		   t.y = x*m[ 4] + y*m[ 5] + z*m[ 6] + w*m[ 7];
		   t.z = x*m[ 8] + y*m[ 9] + z*m[10] + w*m[11];
		   t.w = x*m[12] + y*m[13] + z*m[14] + w*m[15];
		   (*this) = t;
	   }

	   void qidentity() {x=0, y=0, z=0, w=1;}
	   void rotate(const T angle, const Vector u)
	   {
		   T s = T(sinf( angle/2 ));
		   T w = T(cosf( angle/2 ));

		   T x = u.x * s;
		   T y = u.y * s;
		   T z = u.z * s;

		   T m[] = {
			   +w, -z, +y, +x,
			   +z, +w, -x, +y,
			   -y, +x, +w, +z,
			   -x, -y, -z, +w
		   };

		   transform(m);
	   }

	   void rotate(Vector u, const T time)
	   {
		   float angle = u.length()*time;
		   u.normalize();
		   rotate(angle, u);
	   }

	   void rotate_local(const T angle, Vector u)
	   {
		   Matrix<T> m;
		   m.identity();
		   m.rotate(*this);
		   u.transform(m);
		   rotate(angle, u);
	   }

	   void print(const char* s = "vector") { printf("%s = [%f %f %f %f]\n", s, x, y, z, w); }
   };

   //______________________________________________________________
   // 4x4 generic matrix class:

   template <class T>
   class Matrix
   {
   //private:
   public:
	   T d[16];
   public:
   //	Matrix() {}
   //	Matrix(Matrix &m) {for (int i=0; i<16; i++) d[i] = m[i];}
	   T&	cell(int i, int j) {return d[(i<<2)+j];}
	   T&	operator() (int i, int j) {return d[(i<<2)+j];}
   //	Vector<T> operator() (int i) { return Vector(&d[i<<2]); }
	   operator T* () {return d;}
	   Matrix&	operator() (T f) {for (int i=0; i<16; i++) d[i] = f; return *this;}
	   Matrix& operator() (T *m) {for (int i=0; i<16; i++) d[i] = m[i]; return *this;}
	   template <class M> Matrix& operator() (Matrix<M> &m) {for (int i=0; i<16; i++) d[i] = static_cast<T>(m[i]); return *this;}
	   Matrix&	identity() {
		   for (int i=0; i<4; i++) {
			   for (int j=0; j<4; j++) {
				   cell(i,j) = (i==j ? T(1) : T(0));
			   }
		   }

		   return *this;
	   }

	   Matrix& add(T f) {for (int i=0; i<16; i++) d[i] += f; return *this;}
	   Matrix& add(Matrix &m) {
		   for (int i=0; i<4; i++) {
			   for (int j=0; j<4; j++) {
				   cell(i,j) += m(i,j);
			   }
		   }

		   return *this;
	   }

	   Matrix& multiply(T f) {for (int i=0; i<16; i++) d[i] *= f; return *this;}
	   Matrix& multiply(Matrix &m) {
		   Matrix	t(*this);
		   for (int i=0; i<4; i++) {
			   for (int j=0; j<4; j++) {
				   cell(i,j) = 0;
				   for (int k=0; k<4; k++) {
					   cell(i,j) += t(i,k) * m(k,j);
				   }
			   }
		   }

		   return *this;
	   }

	   template <class V> Vector<V>& multiply(Vector<V>& u)
	   {
		   Vector<V> t;

		   for (int i=0; i<4; i++) {
			   t[i] = 0;
			   for (int k=0; k<4; k++) {
				   t[i] += V(cell(i,k) * u[k]);
			   }
		   }

		   u = t;

		   return u;
	   }

	   Matrix& add_row(int i1, int i2, T f)
	   {
		   const int n = 4;

		   T	*p1 = &d[i1*n];
		   T	*p2 = &d[i2*n];

		   for (int j=0; j<4; j++) {
			   *p1++ += *p2++ * f;
		   }

		   return *this;
	   }

	   Matrix& add_col(int j1, int j2, T f)
	   {
		   T	*p1 = &d[j1];
		   T	*p2 = &d[j2];

		   for (int i=0; i<4; i++) {
			   *p1 += *p2 * f;
			   p1 += 4;
			   p2 += 4;
		   }

		   return *this;
	   }

	   Matrix& swp_row(int i1, int i2)
	   {
		   const int n = 4;

		   T	*p1 = &d[i1*n];
		   T	*p2 = &d[i2*n];
		   T	t;

		   for (int j=0; j<4; j++) {
			   t = *p1;
			   *p1++ = *p2;
			   *p2++ = t;
		   }


		   return *this;
	   }

	   Matrix& swp_col(int j1, int j2)
	   {
		   T	*p1 = &d[j1];
		   T	*p2 = &d[j2];
		   T	t;

		   for (int i=0; i<4; i++) {
			   t = *p1;
			   *p1 = *p2;
			   *p2 = t;
			   p1 += 4;
			   p2 += 4;
		   }


		   return *this;
	   }

	   Matrix& transpose()
	   {
		   for (int i=1; i<4; i++) {
			   for (int j=0; j<i; j++) {
				   T t = cell(i,j);
				   cell(i,j) = cell(j,i);
				   cell(j,i) = t;
			   }
		   }
		   return *this;
	   }

	   Matrix& fastinvert()
	   {
		   for (int i=1; i<3; i++) {
			   for (int j=0; j<i; j++) {
				   T t = cell(i,j);
				   cell(i,j) = cell(j,i);
				   cell(j,i) = t;
			   }
		   }

		   T	t3, t7, t11;

		   t3  = - (d[3]*d[0] + d[7]*d[1] + d[11]*d[ 2]);
		   t7  = - (d[3]*d[4] + d[7]*d[5] + d[11]*d[ 6]);
		   t11 = - (d[3]*d[8] + d[7]*d[9] + d[11]*d[10]);

		   d[3] = t3;
		   d[7] = t7;
		   d[11] = t11;
   		
		   return *this;
	   }

	   Matrix& invert()
	   {
		   u32	i;
		   u32	k;
		   T	f;
		   u32 n = 4;
		   Matrix	m;

		   m.identity();

		   for (i=0; i<4; i++) {
			   for (k=i; k<n && fabs(cell(k,i)) < EPS; k++);
			   if (k == n) {
				   identity();
   //				console->write("ERROR (invert): singular matrix!\n");
				   return *this;
			   }

			   swp_row(i,k);
			   m.swp_row(i,k);

			   f = cell(i,i);

			   add_row(i, i, 1/f-1);
			   m.add_row(i, i, 1/f-1);

			   for (k=i+1; k<4; k++) {
				   f = cell(k,i);
				   add_row(k, i, -f);
				   m.add_row(k, i, -f);
			   }
		   }

		   for (i=n-1; i; i--) {
			   for (k=i-1; k<i; k--) {
				   f = cell(k,i);
				   add_row(k, i, -f);
				   m.add_row(k, i, -f);
			   }
		   }

		   (*this)(m);
   		
		   return *this;
	   }

	   template<class V> Matrix& scale(Vector<V>& u)
	   {
		   Matrix	t;

		   t[ 0] = u[0];	t[ 1] = 0;		t[ 2] = 0;		t[ 3] = 0;
		   t[ 4] = 0;		t[ 5] = u[1];	t[ 6] = 0;		t[ 7] = 0;
		   t[ 8] = 0;		t[ 9] = 0;		t[10] = u[2];	t[11] = 0;
		   t[12] = 0;		t[13] = 0;		t[14] = 0;		t[15] = u[3];

		   multiply(t);
		   return *this;
	   }

	   template<class V> Matrix& translate(Vector<V>& u)
	   {
		   Matrix	t;

		   t[ 0] = 1;	t[ 1] = 0;	t[ 2] = 0;	t[ 3] = u[0];
		   t[ 4] = 0;	t[ 5] = 1;	t[ 6] = 0;	t[ 7] = u[1];
		   t[ 8] = 0;	t[ 9] = 0;	t[10] = 1;	t[11] = u[2];
		   t[12] = 0;	t[13] = 0;	t[14] = 0;	t[15] = 1;

		   multiply(t);
		   return *this;
	   }

	   template<class V> Matrix& rotate(Vector<V>& q)
	   {
		   T xx = 2*q.x*q.x;
		   T xy = 2*q.x*q.y;
		   T xz = 2*q.x*q.z;
		   T xw = 2*q.x*q.w;

		   T yy = 2*q.y*q.y;
		   T yz = 2*q.y*q.z;
		   T yw = 2*q.y*q.w;
   		
		   T zz = 2*q.z*q.z;
		   T zw = 2*q.z*q.w;

		   Matrix	t;

		   t[ 0] = 1-yy-zz;	t[ 1] =   xy-zw;	t[ 2] =   xz+yw;	t[ 3] = 0;
		   t[ 4] =   xy+zw;	t[ 5] = 1-xx-zz;	t[ 6] =   yz-xw;	t[ 7] = 0;
		   t[ 8] =   xz-yw;	t[ 9] =   yz+xw;	t[10] = 1-xx-yy;	t[11] = 0;
		   t[12] =       0;	t[13] =       0;	t[14] =       0;	t[15] = 1;

		   multiply(t);

		   return *this;
	   }

	   Matrix& ortho(T left, T right, T bottom, T top, T n, T f)
	   {
		   Matrix	t;

		   t[ 0] = 2.0/(right-left);	t[ 1] = 0;					t[ 2] = 0;				t[ 3] = -(right+left)/(right-left);
		   t[ 4] = 0;					t[ 5] = 2.0/(top-bottom);	t[ 6] = 0;				t[ 7] = -(top+bottom)/(top-bottom);
		   t[ 8] = 0;					t[ 9] = 0;					t[10] = -2.0/(f-n);		t[11] = -(f+n)/(f-n);
		   t[12] = 0;					t[13] = 0;					t[14] = 0;				t[15] = 1;

		   multiply(t);
		   return *this;
	   }

	   Matrix& perspective(T left, T right, T bottom, T top, T n, T f)
	   {
		   Matrix	t;

		   t[ 0] = 2*n/(right-left);	t[ 1] = 0;					t[ 2] = (right+left)/(right-left);	t[ 3] = 0;
		   t[ 4] = 0;					t[ 5] = 2*n/(top-bottom);	t[ 6] = (top+bottom)/(top-bottom);	t[ 7] = 0;
		   t[ 8] = 0;					t[ 9] = 0;					t[10] = -(f+n)/(f-n);				t[11] = -2*f*n/(f-n);
		   t[12] = 0;					t[13] = 0;					t[14] = -1;							t[15] = 0;

		   multiply(t);
		   return *this;
	   }

	   Matrix&	perspective(T fovy, T aspect, T	n, T f)
	   {
		   T t = deg2rad(fovy/2.0f);
		   T right = (T)(n * tan(t) * aspect);
		   T top = (T)(n * tan(t));
		   return perspective(-right, right, -top, top, n, f);
	   }

   };
};


#endif // DELTA_SOARX_GENERIC

