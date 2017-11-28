//
//  Particle.cpp
//  AppFrame
//
//  Created by Yuan Li on 2015/09/29.
//  Copyright 2015 Yuan Li. All rights reserved.
//

#include "Engine/Header/Physics/Particle.hpp"
#include "Common/Header/ReturnCode.h"
#include "Engine/Header/RenderTools.h"
#include "Common/Header/Math.h"
#include "Engine/Header/Model.h"


namespace vEngine
{
	namespace Physics
	{
		Particle::Particle()
			:SceneObject::SceneObject()
		{
		};
		ReturnCode Particle::Create()
		{
			return this->Create(float4(1, 1, 1, 1));
		}
		ReturnCode Particle::Create(float4 color)
		{
			//CHECK_ASSERT(false);

			Mesh* mesh = RenderTools::GetInstance().Make2DCricleMesh();

			Model* model = new Model();
			model->AddMesh(mesh);
			Material* meshMat = new Material();
			meshMat->diffuse = color;
			meshMat->specular = float4(0, 0, 0, 0);
			meshMat->shininess = 10;

			model->AddMaterial(meshMat);
			model->LoadShaderFile("DeferredLighting");

			this->render_element_ = (RenderElement*)model;
			return RCSuccess();
		};
		Particle::~Particle()
		{

		};

		void Particle::Update()
		{
			float Delta = MS_PER_UPDATE;
			if (Delta > 0)
			{
				this->last_frame_info_ = this->current_frame_info_;
				this->current_frame_info_.location = this->current_frame_info_.location + //S0
					((this->current_frame_info_.velocity * Delta) +//V0*t 
						(this->current_frame_info_.acceleration * 0.5f * Delta * Delta)); //0.5*a*t*t
				this->current_frame_info_.velocity = this->current_frame_info_.velocity + this->current_frame_info_.acceleration * Delta;

 				float4x4 m = this->render_element_->GetLocalMatrix();
 				Math::Translate(m, this->current_frame_info_.location.x(), this->current_frame_info_.location.y(), this->current_frame_info_.location.z());
 				this->render_element_->SetModelMatrix(m);

				this->current_frame_info_.acceleration = float3(0, 0, 0);
			}
		};
		void Particle::Render()
		{

		};

		void Particle::Reset()
		{
			this->current_frame_info_.location = float3(0, 0, 0);
			this->current_frame_info_.velocity = float3(0, 0, 0);
			this->current_frame_info_.acceleration = float3(0, 0, 0);
			this->current_frame_info_.mass = 1;
			this->current_frame_info_.radius = 1;
		};


		void Particle::ApplyForce(const float3& force)
		{
			float3 NewAcc = force / this->current_frame_info_.mass;
			this->current_frame_info_.acceleration = this->current_frame_info_.acceleration + NewAcc;
		}


		void Particle::SetLocation(const float3& location)
		{
			CHECK_ASSERT(Math::IsNAN(location.x()) == false);
			CHECK_ASSERT(Math::IsNAN(location.y()) == false);
			CHECK_ASSERT(Math::IsNAN(location.z()) == false);
			this->current_frame_info_.location = location;
		}
		const float3 Particle::GetPosition() const
		{
			return this->current_frame_info_.location;
		}

		void Particle::SetScale(const float3& scale)
		{
			//this->RenderElement->SetScale(NewScale);
		}
		const float Particle::GetMass() const
		{
			return this->current_frame_info_.mass;
		}
		void Particle::SetMass(const float & mass)
		{
			if (this->current_frame_info_.mass > 0)
			{
				this->current_frame_info_.mass = mass;
			}
			else
			{
				//do nothing
			}
		}
		const float3 Particle::GetVelocity() const
		{
			return this->current_frame_info_.velocity;
		}
		void Particle::SetVelocity(const float3& velocity)
		{
			CHECK_ASSERT(Math::IsNAN(velocity.x()) == false);
			CHECK_ASSERT(Math::IsNAN(velocity.y()) == false);
			CHECK_ASSERT(Math::IsNAN(velocity.z()) == false);
			this->current_frame_info_.velocity = velocity;
		};

		const float Particle::GetRadius() const
		{
			return this->current_frame_info_.radius;
		}

		void Particle::ApplyCollisionCorrection(const float NormalizedDeltaTimeFromLastFrame)
		{
			this->current_frame_info_.velocity = this->last_frame_info_.velocity + this->last_frame_info_.acceleration * NormalizedDeltaTimeFromLastFrame * MS_PER_UPDATE;
			this->current_frame_info_.location = this->last_frame_info_.location + this->last_frame_info_.velocity;

			//CHECK_ASSERT(false);
			//this->RenderElement->SetLocation(this->current_frame_info_.location);

			this->current_frame_info_.acceleration = float3(0, 0, 0);
			this->last_frame_info_ = this->current_frame_info_;
			//CHECK_ASSERT(false);
			//this->Update((1 - NormalizedDeltaTimeFromLastFrame) * MS_PER_UPDATE);
			this->Update();
		}


	}
}
