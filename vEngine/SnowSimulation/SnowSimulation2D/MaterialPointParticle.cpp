//
//  MaterialPointParticle.cpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright 2015 Yuan Li. All rights reserved.
//

#include "MaterialPointParticle.hpp"
#include "Common/Header/Vector.h"
#include "Common/Header/ReturnCode.h"
#include "Common/Header/Math.h"

namespace vEngine
{
	namespace Physics
	{

		static const float
			PARTICLE_DIAM = .0072f,		//Diameter of each particle; smaller = higher resolution
			FRAMERATE = 1 / 60.0f,			//Frames per second
			CFL = .04f,					//Adaptive timestep adjustment
			MAX_TIMESTEP = 5e-4f,		//Upper timestep limit
			FLIP_PERCENT = .95f,			//Weight to give FLIP update over PIC (.95)
			CRIT_COMPRESS = 1 - 1.9e-2f,	//Fracture threshold for compression (1-2.5e-2)
			CRIT_STRETCH = 1 + 7.5e-3f,	//Fracture threshold for stretching (1+7.5e-3)
			HARDENING = 5.0f,			//How much plastic deformation strengthens material (10)
			DENSITY = 100,				//Density of snow in kg/m^2 (400 for 3d)
			YOUNGS_MODULUS = 1.5e5f,		//Young's modulus (springiness) (1.4e5)
			POISSONS_RATIO = .2f,		//Poisson's ratio (transverse/axial strain ratio) (.2)
			IMPLICIT_RATIO = 0,			//Percentage that should be implicit vs explicit for velocity update
			MAX_IMPLICIT_ITERS = 30,	//Maximum iterations for the conjugate residual
			MAX_IMPLICIT_ERR = 1e4f,		//Maximum allowed error for conjugate residual
			MIN_IMPLICIT_ERR = 1e-4f,	//Minimum allowed error for conjugate residual
			STICKY = .9f,				//Collision stickiness (lower = stickier)
			GRAVITY = -9.8f;


		static const float MU = YOUNGS_MODULUS / (2 + 2 * POISSONS_RATIO);
		static const float LAMBDA = YOUNGS_MODULUS*POISSONS_RATIO / ((1 + POISSONS_RATIO)*(1 - 2 * POISSONS_RATIO));
		static const float EPSILON = HARDENING;
		

		MaterialPointParticle::MaterialPointParticle()
			:Particle()
		{
			Math::Identity(this->Fe);
			Math::Identity(this->Fp);
			Math::Identity(this->R);
			Math::Identity(this->S);
			Math::Identity(this->s);
			Math::Identity(this->v);
			Math::Identity(this->d);
		};
		MaterialPointParticle::~MaterialPointParticle()
		{

		};

		void MaterialPointParticle::Update()
		{
			//this->restriction_instance_.Apply(*this); 
			
			float Delta = 1/60.0f;
			if (Delta > 0)
			{
				this->last_frame_info_ = this->current_frame_info_;
				/*this->current_frame_info_.location = this->current_frame_info_.location + //S0
					((this->current_frame_info_.velocity * Delta) +//V0*t 
					(this->current_frame_info_.acceleration * 0.5 * Delta * Delta)); //0.5*a*t*t
				this->current_frame_info_.velocity = this->current_frame_info_.velocity + this->current_frame_info_.acceleration * Delta;*/

				float4x4 m = this->render_element_->GetLocalMatrix();
				Math::Translate(m, this->current_frame_info_.location.x(), this->current_frame_info_.location.y(), this->current_frame_info_.location.z());
				this->render_element_->SetModelMatrix(m);

				this->current_frame_info_.acceleration = float3(0, 0, 0);
			}
		}

		void MaterialPointParticle::Reset()
		{
		}

		void MaterialPointParticle::ComputeEnergyDensity()
		{
			float Je = Math::determinant(this->Fe);
			float Jp = Math::determinant(this->Fp);

			float muFp = MU * Math::Pow(Math::E, EPSILON*(1 - Jp));
			float D_lambdaFp = LAMBDA * Jp * (1 - Jp);

			float2x2 U;
			float2 D;
			float2x2 Vt;
			Math::GetSVD2D(Fe, U, D, Vt);			

			float2x2 R = Math::Multiply(U, Vt);

			float2x2 temp = (Fe - R) * 2 * muFp * Math::Transpose(Fe);
			
			float2x2 lambda_mat;
			lambda_mat[0][0] = D_lambdaFp;
			lambda_mat[1][1] = D_lambdaFp;
			
			temp = temp + lambda_mat;

			this->force_ = temp * (-this->volume_);

			//equation:
			//P(F) = Y(F)/dF = ( 2 * mu * (Fe - R ) * FeT + (lambda * (Jp - 1) * Jp) ) * -V
		}

		void MaterialPointParticle::PrintInfo()
		{
			PRINT("Mass " << this->GetMass());
			PRINT("Volume " << this->volume_<< " Desity" << this->density_);
			PRINT("Velocity " << this->GetVelocity().x() << " " << this->GetVelocity().y());
		}

	}
	

}

