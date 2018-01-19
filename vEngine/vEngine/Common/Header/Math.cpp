#include "Common/Header/Math.h"
#include "Common/Header/Vector.h"
#include "Common/Header/Ray.h"
#include "Common/Header/AABBox.h"
#include "Common/Header/Matrix.h"

namespace vEngine
{
	namespace Math
	{
		float Sqrt(float x)
		{
			return std::sqrt(x);
		}

		float Sin(float x)
		{
			return std::sin(x);
		}

		float Cos(float x)
		{
			return std::cos(x);
		}

		float ArcCos(float x)
		{
			return std::acos(x);
		}

		float Tan(float x)
		{
			return std::tan(x);
		}
		float ArcTan(float x)
		{
			return std::atan(x);
		}
		float Cot(float x)
		{
			return 1.0f / Tan(x);
		}
		float Ln(float x)
		{
			return std::log(x);
		}
		float Pow(float base, float exp)
		{
			return pow(base, exp);
		}


		float InvSqrt(float number)
		{
			if (Math::IsFloatEqual(number, 0)) return 0;

			float xhalf = 0.5f*number;

			int i = *(int*)&number; // get bits for floating value
			i = 0x5f3759df - (i >> 1); // gives initial guess y0
			number = *(float*)&i; // convert bits back to float
			number = number*(1.5f - xhalf*number*number); // Newton step, repeating increases accuracy
			number = number*(1.5f - xhalf*number*number); // 2nd Newton step, repeating increases accuracy

			return number;
		}

		bool IntersectRayAABB(Ray* ray, AABBox* aabb)
		{
			//from http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
			//http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-7-intersecting-simple-shapes/ray-box-intersection/

			/*// r.dir is unit direction vector of ray
			float3 dirfrac, lb, rt;
			float t;
			lb = aabb->Min();
			rt = aabb->Max();
			float3 dir = ray->Direction();
			float3 org = ray->Origin();
			dirfrac.x() = 1.0f / dir.x();
			dirfrac.y() = 1.0f / dir.y();
			dirfrac.z() = 1.0f / dir.z();
			// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
			// r.org is origin of ray
			float t1 = (lb.x() - org.x())*dirfrac.x();
			float t2 = (rt.x() - org.x())*dirfrac.x();
			float t3 = (lb.y() - org.y())*dirfrac.y();
			float t4 = (rt.y() - org.y())*dirfrac.y();
			float t5 = (lb.z() - org.z())*dirfrac.z();
			float t6 = (rt.z() - org.z())*dirfrac.z();

			float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
			float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

			// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
			if (tmax < 0)
			{
				t = tmax;
				return false;
			}

			// if tmin > tmax, ray doesn't intersect AABB
			if (tmin > tmax)
			{
				t = tmax;
				return false;
			}

			t = tmin;
			return true;*/

			const int NUMDIM = 3;
			double coord[NUMDIM];

			float minB[NUMDIM], maxB[NUMDIM];
			minB[0] = aabb->Min()[0];
			minB[1] = aabb->Min()[1];
			minB[2] = aabb->Min()[2];

			maxB[0] = aabb->Max()[0];
			maxB[1] = aabb->Max()[1];
			maxB[2] = aabb->Max()[2];
			float origin[NUMDIM], dir[NUMDIM];
			origin[0] = ray->Origin().x();
			origin[1] = ray->Origin().y();
			origin[2] = ray->Origin().z();

			dir[0] = ray->Direction().x();
			dir[1] = ray->Direction().y();
			dir[2] = ray->Direction().z();

			bool inside = true;
			int quadrant[NUMDIM];
			//left = 0; right =1; mid =2;
			int whichPlane;
			double maxT[NUMDIM];
			double candidatePlane[NUMDIM];

			for (int i = 0; i < NUMDIM; i++)
				if (origin[i] < minB[i]) {
					quadrant[i] = 0;
					candidatePlane[i] = minB[i];
					inside = false;
				}
				else if (origin[i] > maxB[i]) {
					quadrant[i] = 1;
					candidatePlane[i] = maxB[i];
					inside = false;
				}
				else {
					quadrant[i] = 2;
				}

				/* Ray origin inside bounding box */
				if (inside) {
					//coord = origin;
					return true;
				}


				/* Calculate T distances to candidate planes */
				for (int i = 0; i < NUMDIM; i++)
					if (quadrant[i] != 2 && dir[i] != 0.0)
					{
						maxT[i] = (candidatePlane[i] - origin[i]) / dir[i];
					}
					else
					{
						maxT[i] = -1.;
					}

				/* Get largest of the maxT's for final choice of intersection */
				whichPlane = 0;
				for (int i = 1; i < NUMDIM; i++)
					if (maxT[whichPlane] < maxT[i])
						whichPlane = i;

				/* Check final candidate actually inside box */
				if (maxT[whichPlane] < 0.) return false;
				for (int i = 0; i < NUMDIM; i++)
					if (whichPlane != i) {
						coord[i] = origin[i] + maxT[whichPlane] * dir[i];
						if (coord[i] < minB[i] || coord[i] > maxB[i])
							return false;
					}
					else {
						coord[i] = candidatePlane[i];
					}
					return true;				/* ray hits box */
		}

		bool IntersectRayTriangle(Ray* ray, float3 a, float3 b, float3 c, float & t)
		{
			/*//http://www.cs.washington.edu/education/courses/cse457/07sp/lectures/triangle_intersection.pdf
			float3 ab = b-a;
			float3 ac = c-a;
			float3 n = Math::Cross(ab, ac);
			if(Math::Dot(n, ray->Direction()) == 0) return false;
			float D = Math::Dot(n , a);
			float t = (D - Math::Dot(n, ray->Origin())) / Math::Dot(n, ray->Direction());

			if(t > 0)
			{
				float3 q = ray->Origin() + ray->Direction() * t;
				float3 bc = c - b;
				float3 ca = a - c;
				float3 aq = q - a;
				float3 bq = q - b;
				float3 cq = q - c;

				if(Math::Dot(Math::Cross(ab,aq), n) >= 0 && Math::Dot(Math::Cross(bc,bq), n) >= 0 && Math::Dot(Math::Cross(ca,cq), n) >= 0)
					return true;
				return false;
			}
			return false;*/


			//from http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
			//http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/raytri/raytri.c
			float3 edge1, edge2, tvec, pvec, qvec;
			float  u, v;
			float det, inv_det;
			float epsilon = 0.000001f;

			t = std::numeric_limits<float>::max();

			edge1 = b - a;
			edge2 = c - a;

			pvec = Math::Cross(ray->Direction(), edge2);

			det = Math::Dot(edge1, pvec);

			// cull test on
			if (det < 0)
				return false;
			tvec = ray->Origin() - a;

			u = Math::Dot(tvec, pvec);
			if (u< 0.0f || u > det)
				return false;

			qvec = Math::Cross(tvec, edge1);
			v = Math::Dot(ray->Direction(), qvec);
			if (v < 0.0f || u + v >det)
				return false;

			t = Math::Dot(edge2, qvec);
			inv_det = 1.0f / det;
			t *= inv_det;
			u *= inv_det;
			v *= inv_det;
			//end cull test

/*			//non-culling
			if(det > -std::numeric_limits<float>::epsilon() && det < std::numeric_limits<float>::epsilon())
				return false;
			inv_det = 1.0f / det;

			tvec = ray->Origin() - a;
			u = Math::Dot(tvec, pvec) * inv_det;
			if(u < 0.0f || u >1.0f)
				return false;

			qvec = Math::Cross(tvec, edge1);

			v = Math::Dot(ray->Direction(), qvec) * inv_det;
			if(v < 0.0f || u + v > 1.0f)
				return false;

			t = Math::Dot(edge2, qvec) * inv_det;
			*/
			return true;
		}

		float Abs(float num)
		{
			return std::fabs(num);
		}

		static const int FLOAT_MAX_ULP = 4;
		bool CompareFloat(const float lhs, const float rhs)
		{
			union CompareUnion
			{
				CompareUnion(float FIn = 1.0)
					:F(FIn)
				{
				};

				float F;
				int64_t FInt;
			};
			CompareUnion LhsFloat(lhs);
			CompareUnion RhsFloat(rhs);

			//checking if NAN
			if (IsNAN(lhs) || IsNAN(rhs)) return false;
			if (IsINF(lhs) || IsINF(rhs)) return false;
			//checking sign
			if ((std::signbit)(lhs) != (std::signbit)(rhs))
			{
				//for +0.0 and -0.0 error
				if (lhs == rhs)	return true;
				return false;
			}

			int64_t Ulps = (std::abs)(LhsFloat.FInt - RhsFloat.FInt);
			if (Ulps <= FLOAT_MAX_ULP)
				return true;
			return false;
		}

		bool IsFloatEqual(float& lhs, float& rhs)
		{
			return CompareFloat(lhs, rhs);
		}

		bool IsFloatEqual(float& lhs, float& rhs, float epsilon)
		{
			epsilon = Math::Abs(epsilon);
			CHECK_ASSERT(epsilon != 0);
			return Math::Abs(lhs - rhs) < epsilon;
		}

		bool IsFloatEqual(const float& lhs, const float& rhs)
		{
			return CompareFloat(lhs, rhs);
		}

		bool GetQuadraticRoot(float2& Roots, const float a, const float b, const float c)
		{
			if (IsFloatEqual(a, 0.0f))
			{
				Roots.x() = Roots.y() = (b != 0.0f) ? -c / b : 0;
				return b == 0 ? false : true;
			}
			float Delta = (b * b) - (4 * a * c);
			if (Delta == 0.0f)
			{
				Roots.x() = Roots.y() = -b / (2 * a);
				return true;
			}
			if (Delta < 0)
			{
				Roots.x() = Roots.y() = 0;
				return false;
			}

			Roots.x() = (-b - Delta) / (2 * a);
			Roots.y() = (-b + Delta) / (2 * a);
			return true;
		}

		inline float GetBSplineHelper(const float value)
		{
			float val = Math::Abs(value);
			float ret = 0;
			if (val < 1)
			{
				ret = (0.5f * val * val * val) - (val * val) + (2 / 3.0f);
			}
			else
			if (val < 2)
			{
				ret = (-1 / 6.0f) * (val * val * val) + (val *val) - (2 * val) + (4 / 3.0f);
			}
			else
			{
				return 0;
			}

			//if (ret < 1e-4f) 
			//	return 0;

			return ret;
		}

		inline float GetBSplineDerivativeHelper(const float value)
		{
			float val = Math::Abs(value);
			if (val < 1)
			{
				return (1.5f * val * value) - 2 * value;
			}
			else
			if (value < 2)
			{
				return (-0.5f) * (val * value) + 2 * value - 2* value/val;
			}
			else
			{
				return 0;
			}
		}

		

		float3 GetBSpline(const float3 value)
		{
			return float3(GetBSplineHelper(value.x()), GetBSplineHelper(value.y()), GetBSplineHelper(value.z()));
		}


		float3 GetBSplineDerivative(const float3 value)
		{
			return float3(GetBSplineDerivativeHelper(value.x()), GetBSplineDerivativeHelper(value.y()), GetBSplineDerivativeHelper(value.z()));
		}


		//-------------------------------------------------------------------------------------------------------------------------
		//a extends of matrix functions to get SVD of a matrix
		//TODO: write my own version

		//from http://www.math.ucla.edu/~fuchuyuan/svd/paper.pdf

		float2x2 G2(float c, float s)
		{
			return float2x2(c, s, -s, c);
		}
		void GetPolarDecomposition2D(float2x2 A, float2x2& R, float2x2& S)
		{
			float x = A[0][0] + A[1][1];
			float y = A[1][0] - A[0][1];

			float d = Math::Sqrt(x*x + y*y);

			float c = 1;
			float s = 0;

			R = G2(c, s);

			float Epsilon = 1e-6f;
			float Zero = 0;
			if (Math::IsFloatEqual(d, Zero, Epsilon) == false)
			{
				d = 1.0f / d;
				R = G2(x * d, -y * d);
			}

			S = Math::Transpose(R) * A;
			PRINT_VAR(R);
			PRINT_VAR(S);
			float2x2 AToVerify = R * S;
			PRINT_VAR(AToVerify);
			/*
			PRINT_WARNING("Start GetPolarDecomposition2D");
			PRINT_VAR(A);
			PRINT("To verify:");
			PRINT_VAR(R);
			PRINT_VAR(S);
			float2x2 AToVerify = R * S;
			PRINT_VAR(AToVerify);
			PRINT_WARNING("End GetPolarDecomposition2D");

			const float std::numeric_limits<float>::epsilon() = 1e-4f;
			CHECK_ASSERT(Math::IsFloatEqual(A[0][0], AToVerify[0][0]));
			CHECK_ASSERT(Math::IsFloatEqual(A[0][1], AToVerify[0][1]));
			CHECK_ASSERT(Math::IsFloatEqual(A[1][0], AToVerify[1][0]));
			CHECK_ASSERT(Math::IsFloatEqual(A[1][1], AToVerify[1][1]));*/
		}

		void GetSVD2D(float2x2 A, float2x2& U, float2& D, float2x2& V) 
		{
			float2x2 R;
			float2x2 S;

			GetPolarDecomposition2D(A, R, S);

			float c = 1;
			float s = 0;

			if (Math::IsFloatEqual(S[0][1], 0))
			{
				D[0] = S[0][0];
				D[1] = S[1][1];
			}
			else
			{
				float taw = 0.5f * (S[0][0] - S[1][1]);
				float w = Math::Sqrt(taw * taw + S[0][1] * S[0][1]);
				float t = taw > 0 ? S[0][1] / (taw + w) : S[0][1] / (taw - w);

				c = Math::InvSqrt(t*t + 1);
				s = -t * c;

				D[0] = c*c *S[0][0] - 2 * c*s*S[0][1] + s*s*S[1][1];
				D[1] = s*s *S[0][0] + 2 * c*s*S[0][1] + c*c*S[1][1];

			}

			if (D[0] < D[1])
			{
				float temp = D[0];
				D[0] = D[1];
				D[1] = temp;

				V = G2(-s, c);
			}
			else
			{
				V = G2(c, s);
			}

			U = R*V;

			//we will transpose it outside the function
			//before this variable Vt stores V
			//Vt = Math::Transpose(Vt);
			/*
			PRINT_WARNING("Start GetSVD2D");
			PRINT_VAR(A);
			PRINT_VAR(U);
			PRINT_VAR(D);
			PRINT_VAR(Vt);

			PRINT("To verify:");
			float2x2 DMatrix;
			DMatrix[0][0] = D[0];
			DMatrix[1][1] = D[1];
			float2x2 AToVerify = U * DMatrix * Vt;
			PRINT_VAR(AToVerify);
			PRINT_WARNING("End GetSVD2D");

			const float std::numeric_limits<float>::epsilon() = 1e-4f;
			CHECK_ASSERT(Math::IsFloatEqual(A[0][0], AToVerify[0][0]));
			CHECK_ASSERT(Math::IsFloatEqual(A[0][1], AToVerify[0][1]));
			CHECK_ASSERT(Math::IsFloatEqual(A[1][0], AToVerify[1][0]));
			CHECK_ASSERT(Math::IsFloatEqual(A[1][1], AToVerify[1][1]));*/
		}
		
		/*void GetSVD2D(float2x2 A, float2x2& U, float2& D, float2x2& Vt) {
			/ * Probably not the fastest, but I can't find any simple algorithms
			Got most of the derivation from:
			http://www.ualberta.ca/~mlipsett/ENGM541/Readings/svd_ellis.pdf
			www.imm.dtu.dk/pubdb/views/edoc_download.php/3274/pdf/imm3274.pdf
			https://github.com/victorliu/Cgeom/blob/master/geom_la.c (geom_matsvd2d method)
			* /

			const float MATRIX_EPSILON = 1e-6f;
			//If it is diagonal, SVD is trivial
			if (fabs(A[0][1] - A[1][0]) < MATRIX_EPSILON && fabs(A[0][1]) < MATRIX_EPSILON) {
				U= float2x2(A[0][0] < 0 ? -1.f : 1.f, 0, 0, A[1][1] < 0 ? -1 : 1.f);
				D = float2(fabs(A[0][0]), fabs(A[1][1]));
				Math::Identity(Vt);
			}
			//Otherwise, we need to compute A^T*A
			else {
				float j = A[0][0] * A[0][0] + A[0][1] * A[0][1],
					k = A[1][0] * A[1][0] + A[1][1] * A[1][1],
					v_c = A[0][0] * A[1][0] + A[0][1] * A[1][1];
				//Check to see if A^T*A is diagonal
				if (fabs(v_c) < MATRIX_EPSILON) {
					float s1 = sqrt(j),
						s2 = fabs(j - k) < MATRIX_EPSILON ? s1 : sqrt(k);
					D = float2(s1, s2);
					Math::Identity(Vt);
					U= float2x2(
						A[0][0] / s1, A[1][0] / s2,
						A[0][1] / s1, A[1][1] / s2
					);
				}
				//Otherwise, solve quadratic for eigenvalues
				else {
					float jmk = j - k,
						jpk = j + k,
						root = sqrt(jmk*jmk + 4 * v_c*v_c),
						eig = (jpk + root) / 2,
						s1 = sqrt(eig),
						s2 = fabs(root) < MATRIX_EPSILON ? s1 : sqrt((jpk - root) / 2);
					D = float2(s1, s2);
					//Use eigenvectors of A^T*A as V
					float v_s = eig - j,
						len = sqrt(v_s*v_s + v_c*v_c);
					v_c /= len;
					v_s /= len;
					Vt = float2x2(v_c, -v_s, v_s, v_c);
					//Compute Umatrix as Av/s
					U= float2x2(
						(A[0][0] * v_c + A[1][0] * v_s) / s1,
						(A[1][0] * v_c - A[0][0] * v_s) / s2,
						(A[0][1] * v_c + A[1][1] * v_s) / s1,
						(A[1][1] * v_c - A[0][1] * v_s) / s2
					);
				}
			}

			PRINT_VAR(A);
			PRINT_VAR(U);
			PRINT_VAR(D);
			PRINT_VAR(Vt);

			PRINT("To verify:");
			float2x2 DMatrix;
			DMatrix[0][0] = D[0];
			DMatrix[1][1] = D[1];
			float2x2 AToVerify = U * DMatrix * Vt;
			PRINT_VAR(AToVerify);

		}*/



		float2 GetGivensConventionalCS(float a, float b)
		{
			float d = a * a + b * b;
			float c = 1;
			float s = 0;
			if (Math::Abs(d) > 0)
			{
				float t = Math::InvSqrt(d);
				c = a * t;
				s = -b * t;
			}

			return float2(c, s);
		}

		float2 GetGivensUnConventionalCS(float a, float b)
		{

			float d = a * a + b * b;
			float c = 1;
			float s = 0;
			if (Math::Abs(d) > 0)
			{
				float t = Math::InvSqrt(d);
				c = a * t;
				s = b * t;
			}

			return float2(c, s);
		}

		float3x3 G3_12(float c, float s, bool use_conventional = true)
		{
			float2 cs = use_conventional ? GetGivensConventionalCS(c, s) : GetGivensUnConventionalCS(c, s);
			c = cs.x();
			s = cs.y();
			return float3x3(c, s, 0,
				-s, c, 0,
				0, 0, 1);
		}


		float3x3 G3_23(float c, float s, bool use_conventional = true)
		{
			float2 cs = use_conventional ? GetGivensConventionalCS(c, s) : GetGivensUnConventionalCS(c, s);
			c = cs.x();
			s = cs.y();
			return float3x3(1, 0, 0,
				0, c, s,
				0, -s, c);
		}


		float3x3 G3_13(float c, float s, bool use_conventional = true)
		{
			float2 cs = use_conventional ? GetGivensConventionalCS(c, s) : GetGivensUnConventionalCS(c, s);
			c = cs.x();
			s = cs.y();
			return float3x3(c, 0, s,
				0, 1, 0,
				-s, 0, c);
		}		
		

		void CodeZerochasing( float3x3& U,  float3x3& A,  float3x3& V)
		{
			float3x3 G = G3_12(A[0][0], A[1][0]);
			A = Math::Multiply(Math::Transpose(G), A);
			U = Math::Multiply(U, G);
			//U = MyMul(U, G); same as mul
			//checked

			float c = A[0][1];
			float s = A[0][2];
			if (Math::Abs(A[1][0]) > std::numeric_limits<float>::epsilon())
			{
				c = A[0][0] * A[0][1] + A[1][0] * A[1][1];
				s = A[0][0] * A[0][2] + A[1][0] * A[1][2];
			}

			G = G3_23(c, s);
			A = Math::Multiply(A, G);
			V = Math::Multiply(V, G);
			//checked;

			G = G3_23(A[1][1], A[2][1]);
			A = Math::Multiply(Math::Transpose(G), A);
			U = Math::Multiply(U, G);
			//checked
		}

		void Zerochasing( float3x3& U,  float3x3& A,  float3x3& V)
		{
			float3x3 G = G3_23(A[0][1], A[0][2]);
			A = Math::Multiply(A, G);
			U = Math::Multiply(Math::Transpose(G), U);

			G = G3_23(A[0][1], A[0][2]);
			A = Math::Multiply(Math::Transpose(G), A);
			V = Math::Multiply(Math::Transpose(G), V);

			G = G3_23(A[1][1], A[2][1]);
			A = Math::Multiply(Math::Transpose(G), A);
			U = Math::Multiply(U, G);
		}

		void Bidiagonalize( float3x3& U,  float3x3& A,  float3x3& V)
		{
			float3x3 G = G3_23(A[1][0], A[2][0]);
			A = Math::Multiply(Math::Transpose(G), A);
			U = Math::Multiply(U, G);
			//checked

			CodeZerochasing(U, A, V);
		}

		float FrobeniusNorm(float3x3 input)
		{
			float ret = 0;
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					ret += input[i][j] * input[i][j];
				}
			}

			return sqrt(ret);
		}

		void FlipSign(int index,  float3x3& mat, float3& sigma)
		{
			mat[0][index] = -mat[0][index];
			mat[1][index] = -mat[1][index];
			mat[2][index] = -mat[2][index];
			sigma[index] = -sigma[index];
		}

		void FlipSignColumn( float3x3& mat, int col)
		{
			mat[0][col] = -mat[0][col];
			mat[1][col] = -mat[1][col];
			mat[2][col] = -mat[2][col];
		}

		inline void Swap(float& a, float& b)
		{
			float temp = a;
			a = b;
			b = temp;
		}


		inline void Swap(float3& a, float3& b)
		{
			float3 temp = a;
			a = b;
			b = temp;
		}

		inline void SwapColumn( float3x3& a, int col_a,  float3x3& b, int col_b)
		{
			float3 temp = float3(a[0][col_a], a[1][col_a], a[2][col_a]);
			a[0][col_a] = b[0][col_b];
			a[1][col_a] = b[1][col_b];
			a[2][col_a] = b[2][col_b];

			b[0][col_b] = temp[0];
			b[1][col_b] = temp[1];
			b[2][col_b] = temp[2];
		}

		void SortWithTopLeftSub( float3x3& U, float3& sigma,  float3x3& V)
		{
			if (Math::Abs(sigma[1]) >= Math::Abs(sigma[2]))
			{
				if (sigma[1] < 0)
				{
					FlipSign(1, U, sigma);
					FlipSign(2, U, sigma);
				}
				return;
			}
			if (sigma[2] < 0)
			{
				FlipSign(1, U, sigma);
				FlipSign(2, U, sigma);
			}
			Swap(sigma[1], sigma[2]);
			SwapColumn(U, 1, U, 2);
			SwapColumn(V, 1, V, 2);

			if (sigma[1] > sigma[0])
			{
				Swap(sigma[0], sigma[1]);
				SwapColumn(U, 0, U, 1);
				SwapColumn(V, 0, V, 1);
			}
			else
			{
				FlipSignColumn(U, 2);
				FlipSignColumn(V, 2);
			}
		}

		void SortWithBotRightSub( float3x3& U, float3& sigma,  float3x3& V)
		{
			if (Math::Abs(sigma[0]) >= Math::Abs(sigma[1]))
			{
				if (sigma[0] < 0)
				{
					FlipSign(0, U, sigma);
					FlipSign(2, U, sigma);
				}
				return;
			}
			Swap(sigma[0], sigma[1]);
			SwapColumn(U, 0, U, 1);
			SwapColumn(V, 0, V, 1);

			if (Math::Abs(sigma[1]) < Math::Abs(sigma[2]))
			{
				Swap(sigma[1], sigma[2]);;
				SwapColumn(U, 1, U, 2);
				SwapColumn(V, 1, V, 2);
			}
			else
			{
				FlipSignColumn(U, 2);
				FlipSignColumn(V, 2);
			}

			if (sigma[1] < 0)
			{
				FlipSign(1, U, sigma);
				FlipSign(2, U, sigma);
			}
		}

		void SolveReducedTopLeft( float3x3& B,  float3x3& U, float3& sigma,  float3x3& V)
		{
			float s3 = B[2][2];
			//float2x2 u = G2(1, 0);
			//float2x2 v = G2(1, 0);

			float2x2 top_left = float2x2(B[0][0], B[0][1], B[1][0], B[1][1]);

			float2x2 A2 = top_left;
			float2x2 U2;
			float2 D2;
			float2x2 V2;
			GetSVD2D(A2, U2, D2, V2);

			float3x3 u3 = G3_12(U2[0][0], U2[0][1], false);
			float3x3 v3 = G3_12(V2[0][0], V2[0][1], false);

			U = Math::Multiply(U, u3);
			V = Math::Multiply(V, v3);

			sigma = float3(D2.x(), D2.y(), s3);
		}


		void SolveReducedBotRight( float3x3& B,  float3x3& U, float3& sigma,  float3x3& V)
		{
			float s1 = B[0][0];
			//float2x2 u = G2(1, 0);
			//float2x2 v = G2(1, 0);

			float2x2 bot_right = float2x2(B[1][1], B[1][2], B[2][1], B[2][2]);

			float2x2 A2 = bot_right;
			float2x2 U2;
			float2 D2;
			float2x2 V2;
			GetSVD2D(A2, U2, D2, V2);

			float3x3 u3 = G3_12(U2[0][0], U2[0][1], false);
			float3x3 v3 = G3_12(V2[0][0], V2[0][1], false);

			U = Math::Multiply(U, u3);
			V = Math::Multiply(V, v3);
			sigma = float3(s1, D2.x(), D2.y());
		}

		void PostProcess(float3x3 B,  float3x3& U,  float3x3& V, float3 alpha, float2 beta, float3& sigma, float tao)
		{
			if (Math::Abs(beta[1]) <= tao)
			{
				SolveReducedTopLeft(B, U, sigma, V);
				SortWithTopLeftSub(U, sigma, V);
			}
			else if (Math::Abs(beta[0]) <= tao)
			{
				SolveReducedBotRight(B, U, sigma, V);
				SortWithBotRightSub(U, sigma, V);
			}
			else if (Math::Abs(alpha[1]) <= tao)
			{
				//UnConventional G here
				float3x3 G = G3_23(B[1][2], B[2][2], false);
				B = Math::Multiply(Math::Transpose(G), B);
				U = Math::Multiply(U, G);

				SolveReducedTopLeft(B, U, sigma, V);
				SortWithTopLeftSub(U, sigma, V);
			}
			else if (Math::Abs(alpha[2]) <= tao)
			{
				float3x3 G = G3_23(B[1][1], B[1][2]);
				B = Math::Multiply(B, G);
				V = Math::Multiply(V, G);

				G = G3_13(B[0][0], B[0][2]);
				B = Math::Multiply(B, G);
				V = Math::Multiply(V, G);

				//checked
				SolveReducedTopLeft(B, U, sigma, V);
				//checked
				SortWithTopLeftSub(U, sigma, V);
				//checked
			}
			else if (Math::Abs(alpha[0]) <= tao)
			{
				//UnConventional G here
				float3x3 G = G3_12(B[0][1], B[1][1], false);
				B = Math::Multiply(Math::Transpose(G), B);
				U = Math::Multiply(U, G);

				//UnConventional G here
				G = G3_13(B[0][2], B[2][2], false);
				B = Math::Multiply(Math::Transpose(G), B);
				U = Math::Multiply(U, G);

				SolveReducedBotRight(B, U, sigma, V);
				SortWithBotRightSub(U, sigma, V);
			}
			else
			{
				//sigma = float3(111, 222, 333);
			}
		}


		void GetSVD3D(float3x3 A,  float3x3& U,  float3& D,  float3x3& V)
		{
			float3x3 B = A;
			Math::Identity(U);
			D = float3(0, 0, 0);
			Math::Identity(V);
			Bidiagonalize(U, B, V);
			//chekced


			float3 alpha = float3(B[0][0], B[1][1], B[2][2]);
			float2 beta = float2(B[0][1], B[1][2]);
			float2 gamma = float2(alpha[0] * beta[0], alpha[1] * beta[1]);

			float tol = 128 * std::numeric_limits<float>::epsilon();
			float tao = tol * Math::Max(0.5f * FrobeniusNorm(B), 1.0f);


			while (Math::Abs(alpha[0]) > tao && Math::Abs(alpha[1]) > tao && Math::Abs(alpha[2]) > tao &&
				Math::Abs(beta[0])  > tao && Math::Abs(beta[1]) > tao)
			{
				float a1 = alpha[1] * alpha[1] + beta[0] * beta[0];
				float a2 = alpha[2] * alpha[2] + beta[1] * beta[1];
				float b1 = gamma[1];


				float d = (a1 - a2) * 0.5f;
				float mu = (b1 * b1) / (Math::Abs(d) + sqrt(d*d + b1*b1));
				//copy sign from d to mu
				//float d_sign = sign(d);
				//mu = d_sign * Math::Abs(mu);

				mu = std::copysign(mu, d);

				//code not in the paper
				//mu = a2 - mu;
				//----------------

				float3x3 G = G3_12(alpha[0] * alpha[0] - mu, gamma[0]);
				B = Math::Multiply(B, G);
				V = Math::Multiply(V, G);

				CodeZerochasing(U, B, V);

				alpha = float3(B[0][0], B[1][1], B[2][2]);
				beta = float2(B[0][1], B[1][2]);
				gamma = float2(alpha[0] * beta[0], alpha[1] * beta[1]);
			}

			PostProcess(B, U, V, alpha, beta, D, tao);

			PRINT_WARNING("Start GetSVD3D");
			PRINT_VAR(A);
			PRINT("To verify:");
			PRINT_VAR(U);
			PRINT_VAR(D);
			PRINT_VAR(V);

			float3x3 D_mat;
			D_mat[0][0] = D[0];
			D_mat[1][1] = D[1];
			D_mat[2][2] = D[2];
			float3x3 AToVerify = U * D_mat * Math::Transpose(V);
			PRINT_VAR(AToVerify);
			PRINT_WARNING("End GetSVD3D");


			CHECK_ASSERT(Math::IsFloatEqual(A[0][0], AToVerify[0][0]));
			CHECK_ASSERT(Math::IsFloatEqual(A[0][1], AToVerify[0][1]));
			CHECK_ASSERT(Math::IsFloatEqual(A[0][2], AToVerify[0][2]));

			CHECK_ASSERT(Math::IsFloatEqual(A[1][0], AToVerify[1][0]));
			CHECK_ASSERT(Math::IsFloatEqual(A[1][1], AToVerify[1][1]));
			CHECK_ASSERT(Math::IsFloatEqual(A[1][2], AToVerify[1][2]));

			CHECK_ASSERT(Math::IsFloatEqual(A[2][0], AToVerify[2][0]));
			CHECK_ASSERT(Math::IsFloatEqual(A[2][1], AToVerify[2][1]));
			CHECK_ASSERT(Math::IsFloatEqual(A[2][2], AToVerify[2][2]));
		}
	}

}