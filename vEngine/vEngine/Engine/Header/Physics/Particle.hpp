//
//  Particle.hpp
//  AppFrame
//
//  Created by Yuan Li on 2015/09/29.
//  Copyright 2015 Yuan Li. All rights reserved.
//

#ifndef Particle_hpp
#define Particle_hpp

#include "Common/Header/CommonPreDec.h"
#include "Common/Header/Vector.h"
#include "Engine/Header/SceneObject.h"

namespace vEngine
{
	namespace Physics {

		class Particle : public SceneObject
		{
		public:
			Particle();
			ReturnCode Create();
			ReturnCode Create(float4 color);
			virtual ~Particle();

			virtual void Update();
			virtual void Render();
			virtual void Reset();

			void ApplyForce(const float3& force);

			void SetLocation(const float3& location);
			const float3 GetPosition() const;
			void SetScale(const float3& scale);

			const float GetMass() const;
			void  SetMass(const float& mass);

			const float3 GetVelocity() const;
			void SetVelocity(const float3& velocity);


			const float GetRadius() const;

			void ApplyCollisionCorrection(const float DeltaTimeFromLastFrame);
		protected:
			struct SpatialInfomation
			{
				float3 location;//physics location that differs from render element's
				float3 velocity;
				float3 acceleration;
				SpatialInfomation()
					:location(float3(0, 0, 0)), velocity(float3(0, 0, 0)), acceleration(float3(0, 0, 0)), mass(1), radius(1)
				{}

				float mass;
				float radius;
			};

			SpatialInfomation last_frame_info_;
			SpatialInfomation current_frame_info_;
		};
	}
}



#endif /* Particle_hpp */
