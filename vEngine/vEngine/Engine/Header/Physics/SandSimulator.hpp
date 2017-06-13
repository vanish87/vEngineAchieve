//
//  SandSimulator.hpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright 2015 Yuan Li. All rights reserved.
//

#ifndef SandSimulator_hpp
#define SandSimulator_hpp

#include "Common\Header\CommonPreDec.h"
#include "Common\Header\ReturnCode.h"
#include <unordered_map>
#include <list>
#include "Engine\Header\Physics\SandParticle.hpp"
#include "Common\Header\Thread.h"

namespace vEngine
{
	class SimulatorThread: public Thread
	{
	public:
		virtual ReturnCode Main(void* para) override;
	};
	class SandSimulator
	{
	public:
		SandSimulator();
		virtual ~SandSimulator();

		void Start();

		//Do main context init staffs
		ReturnCode Init();
		//Deinit staffs
		ReturnCode Deinit();
		//User update
		ReturnCode Update();

		ReturnCode Reset();
		ReturnCode SetConntactParameter(float Mass, float Alpha, float Beta, float NormalRestitution);

		static const uint32_t	NUMBER_OF_PARTICLES = 500;
		static const float3		GRAVITY_CONSTANT;
		static const float3		FRICTION_CONSTANT;

		static float3 GetContactForce(const float3 x1, const float3 x2,
			const float  m1 , const float  m2,
			const float3 v1 , const float3 v2,
			const float  r1 , const float  r2);

	private:
		ReturnCode UpdateSpatialHash();
		ReturnCode CheckDection(SandParticle& PaticleIn, std::list<SandParticle*>& Cadidates);
		ReturnCode HandleCollisionWith(SandParticle & Target1, SandParticle & Target2);

		static float GetKd(float MassEff = 0, float TimeContact = 1);
		static float GetKr(float MassEff = 0, float TimeContact = 1);
		static float GetKd(float3 T1, float3 T2);
		static float GetKr(float3 T1, float3 T2);

		static float Alpha;
		static float Beta;
		static float NormalRestitution;

		std::array<SandParticle, SandSimulator::NUMBER_OF_PARTICLES> ParticlePool;

		static const uint32_t	VOXEL_CELL_SIZE;

		struct SpatialHashZ
		{
			std::unordered_map < int32_t, std::list<SandParticle*>> m;
		};
		struct SpatialHashY
		{
			std::unordered_map < int32_t, SpatialHashZ> m;
		};
		struct SpatialHash
		{
			std::unordered_map < int32_t, SpatialHashY> m;
		};


		SpatialHash SpatialHashInstance;

		SimulatorThread SimulationThread;
	};
}


#endif /* SandSimulator_hpp */
