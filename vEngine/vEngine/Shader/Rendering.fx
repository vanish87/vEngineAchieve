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
	float3 g_eye_pos;
	Light light;
};

float4 CalLighting( in float3 normal, 
					 in float3 position, //view_pos
					 in float4 diffuseAlbedo,
					 in float3 specularAlbedo,
					 in float specularPower )
{
	float3 pos_eye = normalize(-position);
	
	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 litColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
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
			diffuse =  diffuseAlbedo * light_color * diffuse_angle;
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
	float4 litColor = float4(0.1f, 0.1f, 0.1f, 0.0f);

	//float4 litColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//uint lights_size, dummy;
    //gLight.GetDimensions(lights_size, dummy);

	//for(uint i = 0; i < lights_size; i++)
	{
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float  spec    = 0.0f;
		float4 light_color = light.color;
		float3 light_position = light.position;//view_pos

		// The vector from the surface to the light.
		float3 pos_light = light_position - position;//Lc
		float d = length(pos_light);
		if(d > light.falloff.w) 
			return float4(0.0f, 0.0f, 0.0f, 0.0f);

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
							spec    = spec_factor * light_color.r * diffuse_angle* att;//only one value(specular intensity) for spec
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
