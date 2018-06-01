#include "Rendering.fx"

//g-buffer
Texture2D diffuse_tex;
Texture2D normal_tex;
Texture2D depth_tex;

Texture2D position_tex;

//Shadow map
Texture2D shadow_map_tex;

//POM
Texture2D normal_map_tex;

//SSDO
Texture2D blur_occlusion_tex;

//for lighting buffer
Texture2D lighting_tex;

//mesh diffuse texture
Texture2D mesh_diffuse;

SamplerState MeshTextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState ShadowMapSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

cbuffer cbPerObject
{
	float4x4 g_m_inv_transpose;

	Material gMaterial;

	float4x4 g_light_view_proj;

	bool g_normal_map;
	bool g_mesh_diffuse;
};


struct GbufferVSInput
{
	float3 position			: POSITION;
    float3 normal			: NORMAL;	
	float2 tex				: TEXCOORD0;	
	float3 tangent			: TANGENT;
};

struct GbufferVSOutput
{
	float4 position			 : SV_POSITION;
	float3 posWS             : POSITION;
	float2 tex_cood			 : TEXCOORD0;

	float3 normalWS			 : NORMAL;
	float3 tangentWS		 : TANGENT;	
};

//set value to normalize color value
float encodeToColorSpace(float value)
{
	return (value + 1)* 0.5;
}
float decodeFromColorSpace(float value)
{
	return (value - 0.5) * 2;
}

//set value to normalize color value
float4 encodeToColorSpace(float4 value)
{
	return (value + 1)* 0.5;
}
float4 decodeFromColorSpace(float4 value)
{
	return (value - 0.5) * 2;
}

//set value to normalize color value
float3 encodeToColorSpace(float3 value)
{
	return (value + 1)* 0.5;
}
float3 decodeFromColorSpace(float3 value)
{
	return (value - 0.5) * 2;
}

GbufferVSOutput GbufferVS(GbufferVSInput vin)
{
	GbufferVSOutput vout;
	
	vout.position = mul(float4(vin.position, 1.0f), g_model_matrix);
	vout.position = mul(vout.position			  , g_view_proj_matrix);
	vout.tex_cood = vin.tex;
	vout.posWS = mul(float4(vin.position, 1.0f), g_model_matrix).xyz;

	vout.normalWS = mul(vin.normal, (float3x3)g_m_inv_transpose);
	vout.tangentWS = mul(vin.tangent, (float3x3)g_m_inv_transpose);

    return vout;
}


struct GbufferPSOutput
{	
	float4 Normal			: SV_Target0;
	float4 Diffuse			: SV_Target1;
	float4 PosWS			: SV_Target2;
};
GbufferPSOutput GbufferPS(GbufferVSOutput pin)
{
	GbufferPSOutput output;
	float3 normalTS;
	float3 normalWS;
	float3 mat_diffuse;
	
	//normal map	
	if (g_normal_map)
	{
		float3 N = normalize(pin.normalWS);
		float3 T = normalize(pin.tangentWS - dot(pin.tangentWS, N) * N);
		float3 B = cross(N, T);
		float3x3 TtoW = float3x3(T, B, N);
		normalTS = normal_map_tex.Sample(MeshTextureSampler, pin.tex_cood).rgb;
		normalTS = decodeFromColorSpace(normalTS);
		normalWS = mul(normalTS, TtoW);
	}
	else
	{
		normalWS = pin.normalWS;
	}


	if (g_mesh_diffuse)
		mat_diffuse = mesh_diffuse.Sample(MeshTextureSampler, pin.tex_cood).rgb;
	else
		mat_diffuse = float4(1, 1, 1, 1);

	//normalWS.x = encodeToColorSpace(normalWS.x);
	//normalWS.y = encodeToColorSpace(normalWS.y);
	//normalWS.z = encodeToColorSpace(normalWS.z);

	//wolrd space normal + mat.Shininess
	output.Normal = float4(normalWS, gMaterial.Shininess);
	//combines Mat with Tex color
	output.Diffuse  = float4( mat_diffuse* gMaterial.Diffuse.rgb, gMaterial.Specular.x);	
	output.PosWS = float4(pin.posWS,1.0f);
	
	return output;
}
struct LightingVin
{
	float3 position : POSITION;
};

struct LightingVout
{
	float4 pos		   : SV_POSITION;//screen coordinates
	float3 posVS	   : TEXCOORD0;
	float3 posCS	   : TEXCOORD1;
};

LightingVout LightingVS(in LightingVin vin)
{
	LightingVout vout;
	vout.pos = float4(vin.position, 1.0f);
	float4 positionVS = mul( float4(vin.position.xy, 1.0f, 1.0f), main_camera_inv_proj);
	vout.posVS = positionVS.xyz / positionVS.w;
	vout.posCS = float3(vin.position.xy, 1.0f);
	return vout;
}
float linstep(float min, float max, float v)
{
	return saturate((v - min) / (max - min));
}

float4 LightingPS( in LightingVout pin): SV_Target
{		
	int3 samplelndices = int3( pin.pos.xy, 0 );
	//this one depends on View Space coord,
	//so make sure xyz are in the same space
	float depth = depth_tex.Load(samplelndices).r / pin.posVS.z;
	float3 positionVS = pin.posVS;
	positionVS *= depth;

	//this one depends on Clip Space coord,
	//so make sure xyz are in the same space
	depth = depth_tex.Load(samplelndices).r;
	float4 newPositionVS = mul(float4(pin.posCS.xy, depth, 1.0f), main_camera_inv_proj);
	positionVS = newPositionVS.xyz / newPositionVS.w;

	//shadowing
	float4 world_pos = mul(float4(positionVS, 1.0f) , main_camera_inv_view);
	float4 world_pos1 = position_tex.Load(samplelndices).rgba;

	if (abs(world_pos1.x - world_pos.x)> 0.01)
	{
		//return float4(1,0,0, 1);
		//world_pos.x = 0;
	}
	else if (abs(world_pos1.y - world_pos.y)> 0.01)
	{
		//return float4(0, 1, 0, 1);
	}
	else if (abs(world_pos1.z - world_pos.z)> 0.01)
	{
		//return float4(0, 0, 1, 1);
	}
	//world_pos = world_pos1;
	//float4 world_pos = mul(float4(positionVS, 1.0f), main_camera_inv_view);


	//world_pos.y = 0;
	float4 pos_light = mul(world_pos, g_light_view_proj);
	pos_light /= pos_light.w;
	pos_light.x = pos_light.x / 2 + 0.5f;
	pos_light.y = -pos_light.y / 2 + 0.5f;
	
	float zf = 1000.0f;
	float zn = 1.0f;
	float q = zf/ (zf-zn);
	float pos_depth = pos_light.z;
	pos_depth = zn * q / (q - pos_depth);

	pos_depth = mul(world_pos, g_light_view_proj).w;

	//pos_depth = length(mul(light.position.xyz,g_inv_view_matrix)  - world_pos);

	float min_variance = 0.3;
	float bleeding_reduce = 0.75;

	//float2 moments = float2(shadow_depth, shadow_depth*shadow_depth);
	float2 moments = shadow_map_tex.Sample(ShadowMapSampler, pos_light.xy).rg;
	//float len = length(light.position.xyz - positionVS);
	float p = (pos_depth <= moments.x );
	// Variance shadow mapping
	float variance = moments.y - moments.x * moments.x;
	variance = max(variance, min_variance);
	float m_d = moments.x - pos_depth;
	float p_max = variance / (variance + m_d * m_d);
	p_max = linstep(bleeding_reduce, 1, p_max);

	float shadow = max(p, p_max);
	//no shadow for point light
	if(light.type == 0)
		shadow = 1;

	//Get Infor from g-buffer
	//ws normal
	float4 normal_t = normal_tex.Load( samplelndices );

	float3 normal = normal_t.xyz;
	//set for those mesh that do not want to do lighting
	if(normal.x ==0 && normal.y ==0&& normal.z ==0)
		return float4(0,0,0,1);

	float shininess = normal_t.w;

	float4 occlusion = blur_occlusion_tex.Load( samplelndices );
	//cal lighting
	//return float4(1,1,1,1);
	return CalulateLighting( normal, world_pos.xyz, shininess, shadow, occlusion);
	
}

struct FinalVin
{
	float4 Position : POSITION;		
};

struct FinalVout
{
	float4 pos		: SV_POSITION;
};

FinalVout FinalVS(in FinalVin vin)
{
	FinalVout vout;
	vout.pos = vin.Position;
	return vout;
}

float4 FinalPS( in FinalVout pin): SV_Target
{
	if(0)//for debugging
	{
	int3 samplelndices = int3( pin.pos.xy, 0 );
	float4 world_pos = lighting_tex.Load( samplelndices );
	return float4(world_pos.xyz,1.0f);
	}
	else
	{
		
	//Get Infor from g-buffer
	int3 samplelndices = int3( pin.pos.xy, 0 );
	float4 lighting = lighting_tex.Load( samplelndices );
	float4 material = diffuse_tex.Load( samplelndices );
	float3 DiffuseAlbedo = material.rgb;

	if(0) return float4(DiffuseAlbedo,1.0f);
	
	float3 diffuse = lighting.xyz* DiffuseAlbedo;
	float3 specular = lighting.w *  float3(material.w,material.w,material.w);
	//if(lighting.w == 1.0f)
	{
		//diffuse = DiffuseAlbedo;
		//specular = float3(0,0,0);
	}
	//float4 DiffuseAlbedo = gMaterial.Diffuse;
		
	//cal lighting
	//return float4(lighting.w>0.3?float3(1,1,1):0, 1.0f);
	return float4(diffuse + specular , 1.0f);
	//return float4(DiffuseAlbedo , 1.0f);

	}

	
}

BlendState lighing_acc
{
	BlendEnable[0] = TRUE;
	SrcBlend = One;
	DestBlend = One;
	BlendOp[0] = ADD;
	SrcBlendAlpha = One;
	DestBlendAlpha = Zero;
	BlendOpAlpha[0] = ADD;
	RenderTargetWriteMask[0] = 0x0f;

};

BlendState final
{
	BlendEnable[0] = FALSE;
	SrcBlend = One;
	DestBlend = Zero;
	BlendOp[0] = ADD;
	SrcBlendAlpha = One;
	DestBlendAlpha = Zero;
	BlendOpAlpha[0] = ADD;
	RenderTargetWriteMask[0] = 0x0f;

};


technique11 GbufferTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, GbufferVS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, GbufferPS() ) );		
		SetBlendState(final, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
    }

	pass P1
	{
		SetVertexShader( CompileShader( vs_5_0, LightingVS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, LightingPS() ) );
		SetBlendState(lighing_acc, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	}

	pass P2
	{
		SetVertexShader( CompileShader( vs_5_0, FinalVS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, FinalPS() ) );		
		SetBlendState(final, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	}
};

//shadowing tech
struct ShadowVSOut
{
	float4 position_h : SV_POSITION;
};

ShadowVSOut ShadowingVS(GbufferVSInput vin)
{
	ShadowVSOut vout;

	float4x4 world_matrix = g_model_matrix;
	float4x4 mvp_matrix = mul(g_model_matrix, g_view_proj_matrix);
	vout.position_h = mul(float4(vin.position, 1.0f), mvp_matrix);

	return vout;
}

technique11 Shadowing
{
	pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, ShadowingVS() ) );
		SetGeometryShader( NULL );
        SetPixelShader(NULL);		
    }
};

