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

#include "SnowConstants.h"

namespace vEngine
{
	namespace Physics
	{		

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
				this->current_frame_info_.location = this->current_frame_info_.location + //S0
					((this->current_frame_info_.velocity * Delta) +//V0*t 
					(this->current_frame_info_.acceleration * 0.5 * Delta * Delta)); //0.5*a*t*t
				this->current_frame_info_.velocity = this->current_frame_info_.velocity + this->current_frame_info_.acceleration * Delta;

				float4x4 m = this->render_element_->GetLocalMatrix();
				Math::Translate(m, this->current_frame_info_.location.x(), this->current_frame_info_.location.y(), this->current_frame_info_.location.z());
				this->render_element_->SetModelMatrix(m);

				this->current_frame_info_.acceleration = float3(0, 0, 0);
			}
		}

		void MaterialPointParticle::Reset()
		{
			CHECK_ASSERT(false);
		}

		void MaterialPointParticle::ComputeEnergyDensity()
		{
			float Je = Math::determinant(this->Fe);
			float Jp = Math::determinant(this->Fp);

			float muFp = MU * Math::Pow(Math::E, EPSILON*(1 - Jp));
			float D_lambdaFp = LAMBDA * Jp * (1 - Jp);

			/*float2x2 U;
			float2 D;
			float2x2 Vt;
			Math::GetSVD2D(Fe, U, D, Vt);		*/	
			
			float2x2 R;// = Math::Multiply(U, Vt);
			float2x2 S;
			Math::GetPolarDecomposition2D(Fe, R, S);
			//PRINT_VAR(R);

			float2x2 temp = (Fe - R) * 2 * muFp * Math::Transpose(Fe);
			//PRINT_VAR(temp);
			
			float2x2 lambda_mat;
			lambda_mat[0][0] = D_lambdaFp;
			lambda_mat[1][1] = D_lambdaFp;
			//PRINT_VAR(lambda_mat);
			
			temp = temp + lambda_mat;

			this->force_ = temp * (-this->volume_);
			//PRINT_VAR(force_);

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

