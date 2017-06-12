#include "Common\Header\Math.h"
#include "Common\Header\Vector.h"
#include "Common\Header\Ray.h"
#include "Common\Header\AABBox.h"

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

		float ArcCos( float x )
		{
			return std::acos(x);
		}

		float Tan(float x)
		{
			return std::tan(x);
		}
		float ArcTan( float x )
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
			float xhalf = 0.5f*number;

			int i = *(int*)&number; // get bits for floating value
			i = 0x5f3759df - (i>>1); // gives initial guess y0
			number = *(float*)&i; // convert bits back to float
			number = number*(1.5f-xhalf*number*number); // Newton step, repeating increases accuracy
			number = number*(1.5f-xhalf*number*number); // 2nd Newton step, repeating increases accuracy

			return number;
		}

		bool IntersectRayAABB( Ray* ray, AABBox* aabb )
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
			origin[0]= ray->Origin().x();
			origin[1]= ray->Origin().y();
			origin[2]= ray->Origin().z();

			dir[0]= ray->Direction().x();
			dir[1]= ray->Direction().y();
			dir[2]= ray->Direction().z();

			bool inside = true;
			int quadrant[NUMDIM];
			//left = 0; right =1; mid =2;
			int whichPlane;
			double maxT[NUMDIM];
			double candidatePlane[NUMDIM];

			for (int i=0; i<NUMDIM; i++)
				if(origin[i] < minB[i]) {
					quadrant[i] = 0;
					candidatePlane[i] = minB[i];
					inside = false;
				}else if (origin[i] > maxB[i]) {
					quadrant[i] = 1;
					candidatePlane[i] = maxB[i];
					inside = false;
				}else	{
					quadrant[i] = 2;
				}

				/* Ray origin inside bounding box */
				if(inside)	{
					//coord = origin;
					return true;
				}


				/* Calculate T distances to candidate planes */
				for (int i = 0; i < NUMDIM; i++)
					if (quadrant[i] != 2 && dir[i] !=0.0)
					{
						maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
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
						coord[i] = origin[i] + maxT[whichPlane] *dir[i];
						if (coord[i] < minB[i] || coord[i] > maxB[i])
							return false;
					} else {
						coord[i] = candidatePlane[i];
					}
					return true;				/* ray hits box */
		}

		bool IntersectRayTriangle( Ray* ray, float3 a, float3 b, float3 c , float & t)
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
			float EPSILON = 0.000001f;

			t =  std::numeric_limits<float>::max();

			edge1 = b-a;
			edge2 = c-a;

			pvec = Math::Cross(ray->Direction(), edge2);

			det = Math::Dot(edge1, pvec);

			// cull test on
				if(det < 0)
					return false;
				tvec = ray->Origin() - a;

				u = Math::Dot(tvec , pvec);
				if(u< 0.0f || u > det)
					return false;

				qvec = Math::Cross(tvec, edge1);
				v = Math::Dot(ray->Direction(), qvec);
				if(v < 0.0f || u+v >det)
					return false;

				t = Math::Dot(edge2, qvec);
				inv_det = 1.0f / det;
				t*= inv_det;
				u*= inv_det;
				v*= inv_det;
			//end cull test

/*			//non-culling
			if(det > -EPSILON && det < EPSILON)
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

		float Abs( float num )
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
				int32_t FInt;
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

			int32_t Ulps = (std::abs)(LhsFloat.FInt - RhsFloat.FInt);
			if (Ulps <= FLOAT_MAX_ULP)
				return true;
			return false;
		}

		bool IsFloatEqual(float& lhs, float& rhs)
		{
			return CompareFloat(lhs, rhs);
		}

		bool IsFloatEqual(const float& lhs, const float& rhs)
		{
			return CompareFloat(lhs, rhs);
		}

		bool GetQuadraticRoot(float2& Roots, const float a, const float b, const float c)
		{
			if (IsFloatEqual (a, 0.0f))
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

	}
}