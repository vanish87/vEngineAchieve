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

			Math::Identity(this->B);
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
					(this->current_frame_info_.acceleration * 0.5f * Delta * Delta)); //0.5*a*t*t
				this->current_frame_info_.velocity = this->current_frame_info_.velocity + this->current_frame_info_.acceleration * Delta;

				if (this->render_element_ != nullptr)
				{
					float4x4 m = this->render_element_->GetLocalMatrix();
					Math::Translate(m, this->current_frame_info_.location.x(), this->current_frame_info_.location.y(), this->current_frame_info_.location.z());
					this->render_element_->SetModelMatrix(m);
				}
				this->current_frame_info_.acceleration = float3(0, 0, 0);
			}
		}

		void MaterialPointParticle::Reset()
		{
			CHECK_ASSERT(false);
		}

		void MaterialPointParticle::ComputeEnergyDensity()
		{
			//------------------------

			/*float Je = Math::determinant(this->Fe);
			float Jp = Math::determinant(this->Fp);

			float harden = Math::Pow(Math::E, EPSILON*(1 - Jp));

			float2x2 U;
			float2 D;
			float2x2 Vt;
			Math::GetSVD2D(Fe, U, D, Vt);

			float2x2 temp = (this->Fe - (U * Math::Transpose(Vt))) * 2 * MU * Math::Transpose(this->Fe);


			float D_lambdaFp = LAMBDA * Jp * (1 - Jp);
			float2x2 lambda_mat;
			lambda_mat[0][0] = D_lambdaFp;
			lambda_mat[1][1] = D_lambdaFp;

			temp = temp + lambda_mat;

			this->force_ = temp * -volume_ * harden;
*/

			//-----------------------------------------
			float Je = Math::determinant(this->Fe);
			float Jp = Math::determinant(this->Fp);

			//CHECK_ASSERT(Jp != 0);

// 			if (Jp == 0) Jp = 1; 
// 			if (Math::IsINF(Jp))
// 			{
// 				Math::Identity(this->Fp);
// 				Jp = 1;
// 			}

			float muFp = MU * Math::Pow(Math::E, EPSILON*(1 - Jp));
			float lambdaFp = LAMBDA * Math::Pow(Math::E, EPSILON*(1 - Jp));


			if (Math::IsINF(muFp)) muFp = std::numeric_limits<float>::max();
			if (Math::IsINF(lambdaFp)) lambdaFp = std::numeric_limits<float>::max();

			CHECK_ASSERT(Math::IsINF(muFp) == false);
			CHECK_ASSERT(Math::IsINF(lambdaFp) == false);

			/*float2x2 U;
			float2 D;
			float2x2 Vt;
			Math::GetSVD2D(Fe, U, D, Vt);

			CHECK_ASSERT(Math::Abs(Math::determinant(D) - Je) < 1e-5f);*/
			
			float2x2 R;// = Math::Multiply(U, Vt);
			float2x2 S;
			Math::GetPolarDecomposition2D(Fe, R, S);
			//PRINT_VAR(R);

			float2x2 temp = (Fe - R) * 2 * muFp * Math::Transpose(Fe);
			//PRINT_VAR(temp);
			
			float2x2 lambda_mat;
			lambda_mat[0][0] = lambdaFp * (Je - 1) * Je;
			lambda_mat[1][1] = lambdaFp * (Je - 1) * Je;
			//PRINT_VAR(lambda_mat);
			
			temp = temp + lambda_mat;

			this->force_ = temp * (-this->volume_);
			//PRINT_VAR(force_);


			CHECK_ASSERT(Math::IsNAN(force_[0][0]) == false);
			CHECK_ASSERT(Math::IsNAN(force_[0][1]) == false);
			CHECK_ASSERT(Math::IsNAN(force_[1][0]) == false);
			CHECK_ASSERT(Math::IsNAN(force_[1][1]) == false);
			//equation:
			//P(F) = Y(F)/dF = ( 2 * mu * (Fe - R ) * FeT + (lambda * (Je - 1) * Je) ) * -V
		}

		void MaterialPointParticle::PrintInfo()
		{
			PRINT("Mass " << this->GetMass());
			PRINT("Volume " << this->volume_<< " Desity" << this->density_);
			PRINT("Velocity " << this->GetVelocity().x() << " " << this->GetVelocity().y());
		}

	}
	

}

