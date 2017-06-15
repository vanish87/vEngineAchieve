#include "CommonHeader.fx"

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; //it will be only float
	float  Shininess;

	int diffuse_tex_index;
	int normalmap_tex_index;
	int pom_tex_index;
};

struct Light
{
    float3 position;	
	int type;
	float3 direction;
	float range;
    float4 color;
	float4 falloff;
	float2 inner_outer;
};
//StructuredBuffer<PointLight> gLight;

cbuffer cbPerFrame
{
	float4x4 main_camera_inv_view;
	float4x4 main_camera_inv_proj;
	Light light;
};

float _RefractiveIndex = 0.3;
float PI = 3.141592653f;

//reflection R
float SchlickFresnelWithN(float n, float3 halfVec, float3 viewDir/*or LightDir*/)
{
	float FresnelFactor = pow((1 - n) / (1 + n), 2);
	float  VdotH = dot(halfVec, viewDir);
	return FresnelFactor + ((1 - FresnelFactor) * pow(1 - VdotH, 5));
}

//refraction T
float FresnelT(float n, float3 normal, float3 In)
{
	return 1 - SchlickFresnelWithN(n, normal, In);
}

float CookTorranceGeometry(float3 normal, float3 halfVec, float3 viewDir, float3 lightDir)
{
	float NdotH = dot(normal, halfVec);
	float VdotH = dot(viewDir, halfVec);
	float NdotMin = min(dot(normal, viewDir), dot(normal, lightDir));
	return min(1.0f, 2.0f * NdotH * NdotMin / VdotH);
}

//The power p is the "roughness parameter" of the Phong NDF; high values represent smooth
//surfaces and low values represent rough ones.
float BlinnPhongDistribution(float3 normal, float3 halfVec, float alpha)
{
	float normalizeTerm = (alpha + 2) / (2 * PI);
	return normalizeTerm * pow(max(0, dot(normal, halfVec)), alpha);
}

//http://simonstechblog.blogspot.jp/2011/12/microfacet-brdf.html
//http://blog.selfshadow.com/publications/s2012-shading-course/hoffman/s2012_pbs_physics_math_notes.pdf
float CalBlinnPhongBRDF_Specular(float3 normal, float3 viewDir, float3 lightDir, bool withFresnal, float power)
{
	float3 halfVec = normalize(viewDir + lightDir);
	float f = 1;
	if (withFresnal)
	{
		f = SchlickFresnelWithN(_RefractiveIndex, halfVec, lightDir);
	}

	float g = CookTorranceGeometry(normal, halfVec, viewDir, lightDir);
	//BlinnPhong distribution
	float d = BlinnPhongDistribution(normal, halfVec, power);

	float NdotL = dot(normal, lightDir);
	float NdotV = dot(normal, viewDir);
	return f * g * d / (4 * NdotL * NdotV);
}

float4 CalulateLighting(in float3 normal,
						in float3 position, //world space pos
						in float  specularPower,
						in float  shadow,
						in float4 occlusion)
{
	float3 pos_eye = normalize(g_eye_pos - position);//V == pos->eye   vector
	normal = normalize(normal);
	// Start with a sum of zero. 
	// Default ambeint color = (0.1, 0.1 0.1)
	float4 litColor = float4(0.0f, 0.0f, 0.0f, 0.0f);


	//for(uint i = 0; i < lights_size; i++)
	{
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float  spec = 0.0f;
		float4 light_color = light.color;
		float3 light_position = light.position;

		// The vector from the surface to the light.
		float3 pos_light = light_position - position;//Lc    == pos->light vector

		float d = length(pos_light);
		if (d > light.falloff.w)
		{
			return litColor;
		}

		float3 light_dir = light.direction;
		int type = light.type;

		//TODO : not good one, use different fx file to handle different light type
		[branch]
		switch (type)
		{
			//Point Light
		case 0:
		{
			pos_light = normalize(pos_light);

			float diffuse_angle = dot(pos_light, normal);//N * Lc (light vector = L)
			[flatten]
			if (diffuse_angle > 0.0f)
			{
				float3 H = normalize(pos_light + pos_eye);

				float spec_factor = pow(max(dot(normal, H), 0.0f), specularPower);

				float att = 1.0f / (light.falloff.x + light.falloff.y *d + light.falloff.z * d * d);
				//Clight * (N * Lc)
				diffuse = light_color * diffuse_angle * att;
				//pow(N*H, alpha) * Clight * (N * Lc)
				//spec = spec_factor * light_color.r * diffuse_angle* att;//only one value(specular intensity) for spec
				spec = light_color.r * CalBlinnPhongBRDF_Specular(normal, pos_eye, pos_light, true, spec_factor);
			}

			float4 acc_color = float4(diffuse.rgb, spec);
			//acc_color = float4(float3(0,0,0), spec);
			litColor = litColor + acc_color;
			break;
		}
		//Spot Light
		case 1:
		{
			pos_light = normalize(pos_light);

			float diffuse_angle = dot(pos_light, normal);//N * Lc (light vector = L)
			[flatten]
			if (diffuse_angle > 0.0f)
			{
				float3 H = normalize(pos_light + pos_eye);

				float spec_factor = pow(max(dot(normal, H), 0.0f), specularPower);

				float att = 1.0f / (light.falloff.x + light.falloff.y *d + light.falloff.z * d * d);
				//Clight * (N * Lc)
				diffuse = light_color * diffuse_angle * att;
				//pow(N*H, alpha) * Clight * (N * Lc)
				//spec = spec_factor * light_color.r * diffuse_angle* att;//only one value(specular intensity) for spec
				spec = light_color.r * CalBlinnPhongBRDF_Specular(normal, pos_eye, pos_light, true, spec_factor);
			}

			float inner = light.inner_outer.x;
			float outer = light.inner_outer.y;

			float cosDirection = dot(-pos_light, normalize(light_dir));

			//Hermite interpolation
			float spot = smoothstep(outer, inner, cosDirection);
			//float d = length(pos_light);
			//spot/= d*d;


			float light_occlusion = 1 - saturate(dot(float4(-normalize(pos_eye), 1), occlusion));

			diffuse = diffuse * spot * shadow * light_occlusion;
			spec = spec * spot * shadow;

			float4 acc_color = float4(diffuse.rgb, spec);
			//acc_color = float4(float3(0,0,0), spec);
			litColor = litColor + acc_color;

			break;
		}
		case 2:
			break;
		default:
			break;
		}

	}
	return litColor;
}

float4 CalLighting( in float3 normal, 
					 in float3 position, //view_pos
					 in float4 diffuseAlbedo,
					 in float3 specularAlbedo,
					 in float specularPower )
{
	float3 pos_eye = normalize(-position);
	
	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 litColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//uint lights_size, dummy;
    //gLight.GetDimensions(lights_size, dummy);

	//for(uint i = 0; i < lights_size; i++)
	{
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 light_color = light.color;
		float3 light_position = light.position;//view_pos

		float3 light_dir = light.direction;
		int type = light.type;
		// The vector from the surface to the light.
		float3 pos_light = light_position - position;
		pos_light = normalize(pos_light);

		float diffuse_angle = dot(pos_light, normal);//N * Lc
		[flatten]
		if( diffuse_angle > 0.0f )
		{
			float3 refect_vec = reflect(-pos_light, normal);

			float spec_factor = pow(max(dot(refect_vec, pos_eye), 0.0f), specularPower);

			//Cdiff * Clight * (N * Lc)
			diffuse = diffuseAlbedo * light_color * diffuse_angle;
			//pow(R*V, alpha) * Cspec * Clight * (N * Lc)
			spec    = spec_factor * float4(specularAlbedo, 1.0f) * light_color * diffuse_angle;
		}
		
		float4 acc_color = (ambient + diffuse + spec);
		litColor = litColor + acc_color;
	}

	litColor.a = diffuseAlbedo.w;
    return litColor;
}

float4 CalPreLighting(	 in float3 normal, 
						 in float3 position, //view pos
						 in float  specularPower,
						 in float shadow,
						 in float4 occlusion)
{
	float3 pos_eye = normalize(-position);//V
	
	// Start with a sum of zero. 
	// Default ambeint color = (0.2, 0.2 0.2)
	float4 litColor = float4(0.0f, 0.0f, 0.0f, 0.0f);


	//for(uint i = 0; i < lights_size; i++)
	{
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float  spec    = 0.0f;
		float4 light_color = light.color;
		float3 light_position = light.position;//view_pos

		// The vector from the surface to the light.
		float3 pos_light = light_position - position;//Lc
		float d = length(pos_light);
		if (d > light.falloff.w)
		{
			return litColor;
		}

		float3 light_dir = light.direction;				
		int type = light.type;

		//TODO : not good one, use different fx file to handle different light type
		[branch] 
		switch(type)
			{
				//Point Light
				case 0:	
					{						
						pos_light = normalize(pos_light);

						float diffuse_angle = dot(pos_light, normal);//N * Lc (light vector = L)
						[flatten]
						if( diffuse_angle > 0.0f )
						{
							float3 H = normalize(pos_light + pos_eye);

							float spec_factor = pow(max(dot(normal, H), 0.0f), specularPower);
							
							float att = 1.0f / (light.falloff.x + light.falloff.y *d + light.falloff.z * d * d );
							//Clight * (N * Lc)
							diffuse = light_color * diffuse_angle * att;
							//pow(N*H, alpha) * Clight * (N * Lc)
							spec    = spec_factor * light_color.r * diffuse_angle* att;//only one value(specular intensity) for spec
						}
						
						float4 acc_color = float4(diffuse.rgb , spec);
						litColor = litColor + acc_color;
						break;
					}
				//Spot Light
				case 1:
					{
						pos_light = normalize(pos_light);

						float diffuse_angle = dot(pos_light, normal);//N * Lc (light vector = L)
						[flatten]
						if( diffuse_angle > 0.0f )
						{
							float3 H = normalize(pos_light + pos_eye);

							float spec_factor = pow(max(dot(normal, H), 0.0f), specularPower);
							
							float att = 1.0f / (light.falloff.x + light.falloff.y *d + light.falloff.z * d * d );
							//Clight * (N * Lc)
							diffuse = light_color * diffuse_angle * att;
							//pow(N*H, alpha) * Clight * (N * Lc)
							spec = spec_factor * light_color.r * diffuse_angle* att;//only one value(specular intensity) for spec
							//spec = light_color.r * CalBlinnPhongBRDF_Specular(normal, pos_eye, pos_light, true, spec_factor);
						}

						float inner = light.inner_outer.x;
						float outer = light.inner_outer.y;
  
						float cosDirection = dot(-pos_light, normalize(light_dir)); 

						//Hermite interpolation
						float spot = smoothstep(outer, inner, cosDirection);
						//float d = length(pos_light);
						//spot/= d*d;

						
						float light_occlusion = 1-saturate(dot(float4(-normalize(pos_eye),1), occlusion));

						diffuse = diffuse * spot * shadow * light_occlusion;
						spec = spec * spot * shadow;

						float4 acc_color = float4(diffuse.rgb , spec);
						litColor = litColor + acc_color;
						
						break;
					}
				case 2:
					break; 
				default:
					break; 
			}
		
	}	
    return litColor;
}

