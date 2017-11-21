



#ifndef SNOW_CONSTANTS_H
#define SNOW_CONSTANTS_H

#pragma once


namespace vEngine
{
	extern const float MS_PER_UPDATE;
	namespace Physics
	{
		const static float
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

	}
}
#endif