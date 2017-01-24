#include "Rendering.fx"

//g-buffer
Texture2D diffuse_tex;
Texture2D normal_tex;
Texture2D depth_tex;

//Shadow map
Texture2D shadow_map_tex;

//POM
Texture2D normal_map_tex;

//SSDO
Texture2D blur_occlusion_tex;

//for lighting buffer
Texture2D lighting_tex;

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
	//model matrix
	float4x4 g_world_matrix;
	//mesh_matrix
	float4x4 g_model_matrix;
	float4x4 g_view_matrix;
	float4x4 g_view_proj_matrix;
	float4x4 g_mwv_inv_transpose;
	float4x4 g_inv_proj_matrix;
	float4x4 g_inv_view_matrix;
	Material gMaterial;

	//not use
	//float4x4 g_shadow_transform; 

	float4x4 g_light_view_proj; 

	bool g_pom_enable;
	bool g_normal_map;
};


struct VertexIn
{
	float3 pos				: POSITION;
    float3 normal			: NORMAL;	
	float2 tex_cood			: TEXCOORD;	
	float3 tangent_cood		: TANGENT;
	float3 binormal			: BINORMAL;
};

struct VertexOut
{
	float4 pos				 : SV_POSITION;
   // float3 normalVS			 : NORMAL;     //view space
	float3 posWS             : Position;
	float2 tex_cood			 : TEXCOORD0;

	float3 normalWS			 : NORMAL;
	float3 tangentWS		 : TANGENT;

    float3 vViewTS           : TEXCOORD1;   // view vector in tangent space, denormalized
    float3 vNormalTS         : TEXCOORD2;   // Normal vector in world space

};

VertexOut GbufferVS(VertexIn vin)
{
	float g_fHeightMapScale = 1.0f;
	VertexOut vout;
	
	float4x4 world_matrix = mul(g_model_matrix, g_world_matrix);
	float4x4 mvp_matrix = mul(world_matrix ,g_view_proj_matrix);
	vout.pos = mul(float4(vin.pos, 1.0f), mvp_matrix);
// 	vout.normalVS = normalize(mul(vin.normal, (float3x3)g_mwv_inv_transpose));
// 	vout.tangentVS = normalize(mul(vin.tangent_cood, (float3x3)g_mwv_inv_transpose));
// 	//trust model input come with orthorch
// 	vout.binormalVS = normalize(mul(vin.binormal, (float3x3)g_mwv_inv_transpose));
	vout.tex_cood = vin.tex_cood;    

	float3 normalWS = mul(vin.normal, (float3x3)world_matrix);
	float3 tangentWS = mul(vin.tangent_cood, (float3x3)world_matrix);
	float3 binormalWS =  mul(vin.binormal, (float3x3)world_matrix);

	float4 positionWS =  mul(float4(vin.pos, 1.0f), world_matrix);
	float3 viewWS = g_eye_pos - positionWS.xyz;

	float3x3 mTtoW = float3x3( tangentWS, binormalWS, normalWS );
	vout.vViewTS  = mul(mTtoW, viewWS);//== vViewWS * invese(mTtoW) == vViewWS * mWtoT
	vout.vNormalTS = mul(mTtoW, vin.normal);

	vout.normalWS = normalWS;
	vout.tangentWS = tangentWS;
	
	vout.posWS = positionWS.xyz;

    return vout;
}


struct GbufferPSOutput
{
	
	float4 Normal			: SV_Target0;
	float4 Diffuse			: SV_Target1;
	float4 PositionWS       : SV_Target2;
};
GbufferPSOutput GbufferPS(VertexOut pin)
{
	GbufferPSOutput output;
	float3 normalTS;
	float3 normalWS;
	float3 mat_diffuse;
	if(g_pom_enable)
	{
		int g_nMaxSamples = 100;
		int g_nMinSamples = 12;
		float fHeightMapScale = 0.08;
		float fParallaxLimit = length((pin.vViewTS.xy) / pin.vViewTS.z);
		fParallaxLimit *= fHeightMapScale;

		float2 vOffset = normalize( float2(-pin.vViewTS.x, pin.vViewTS.y) );
		vOffset = vOffset * fParallaxLimit;

		int nNumSamples = (int) lerp( g_nMinSamples, g_nMaxSamples,dot(  pin.vViewTS,  pin.vNormalTS ));
		float fStepSize = 1.0f / nNumSamples;

		float2 dx, dy;
		dx = ddx( pin.tex_cood );
		dy = ddy( pin.tex_cood );

		float2 vOffsetStep = fStepSize * vOffset;
		float2 vCurrOffset = 0.0f;
		float2 vLastOffset = 0.0f;
		float2 vFinalOffset = 0.0f;

		float4 vCurrSample;
		float4  vLastSample;

		float stepHeight = 1.0;
		int nCurrSample = 0;

		while( nCurrSample < nNumSamples )
		{
			vCurrSample = normal_map_tex.SampleGrad( MeshTextureSampler, pin.tex_cood + vCurrOffset, dx, dy );

			if ( vCurrSample.a > stepHeight )
		   {
			  float Ua = (vLastSample.a - (stepHeight+fStepSize))  / ( fStepSize + (vCurrSample.a - vLastSample.a));
			  vFinalOffset = vLastOffset + Ua * vOffsetStep;

			  vCurrSample = normal_map_tex.SampleGrad( MeshTextureSampler, pin.tex_cood  + vFinalOffset, dx, dy );
			  nCurrSample = nNumSamples + 1;
		   }
			else
		   {
			  nCurrSample++;
			  stepHeight -= fStepSize;
			  vLastOffset = vCurrOffset;
			  vCurrOffset += vOffsetStep;
			  vLastSample = vCurrSample;
		   }
		}
		float3 N = normalize(pin.normalWS);
		float3 T = normalize(pin.tangentWS - dot(pin.tangentWS, N) * N);
		float3 B = cross(N,T);
		float3x3 TtoW = float3x3(T, B, N);
		//TS normal + height
		normalTS = vCurrSample.rgb;
		normalWS = mul( normalTS, TtoW );
		mat_diffuse = mesh_diffuse.Sample(MeshTextureSampler, pin.tex_cood  + vFinalOffset).rgb;
	}
	//not pom
	else
	{
		//normal map	
		if(g_normal_map)
		{
			float3 N = normalize(pin.normalWS);
			float3 T = normalize(pin.tangentWS - dot(pin.tangentWS, N) * N);
			float3 B = cross(N,T);
			float3x3 TtoW = float3x3(T, B, N);
			normalTS = normal_map_tex.Sample( MeshTextureSampler, pin.tex_cood).rgb;
			normalWS = mul( normalTS, TtoW );
		}
		else
			normalWS = pin.normalWS;

		mat_diffuse = mesh_diffuse.Sample(MeshTextureSampler, pin.tex_cood ).rgb;

	}
	
	float3 normalVS = mul(normalWS, (float3x3)g_view_matrix);

	//view space normal + mat.Shininess
	output.Normal = float4(normalVS, gMaterial.Shininess);	
	//combines Mat with Tex color
	output.Diffuse  = float4( mat_diffuse* gMaterial.Diffuse.rgb, gMaterial.Specular.x);	

	//only for ssdo
	output.PositionWS = float4(pin.posWS,1.0f);

	return output;
}
struct LightingVin
{
	float4 Position : POSITION;
};

struct LightingVout
{
	float4 pos		: SV_POSITION;
	float3 view_ray    : VIEWRAY;
};

LightingVout LightingVS(in LightingVin vin)
{
	LightingVout vout;
	vout.pos = vin.Position;
	float4 pos = mul(vin.Position, g_inv_proj_matrix);

	float3 positionVS = mul( vin.Position, g_inv_proj_matrix ).xyz;
	vout.view_ray = float3( positionVS.xy / positionVS.z, 1.0f );
	return vout;
}
float linstep(float min, float max, float v)
{
	return saturate((v - min) / (max - min));
}

float4 LightingPS( in LightingVout pin): SV_Target
{
	if(0)//for debugging
	{

	int3 samplelndices = int3( pin.pos.xy, 0 );
	float3 world_pos = normal_tex.Load( samplelndices ).xyz;
	return float4(world_pos.xyz/10000,1.0f);
	}
	else{
		
	
	int3 samplelndices = int3( pin.pos.xy, 0 );
	float3 view_ray_vec = pin.view_ray;
	float depth = depth_tex.Load( samplelndices ).r;

	float3 positionVS = view_ray_vec * depth;

	//shadowing
	float4 world_pos = mul(float4(positionVS, 1.0f) , g_inv_view_matrix);
	//world_pos /= world_pos.w;
	//float2 shadow_tex_cood = mul(world_pos , g_shadow_transform);
	//float shadow_depth = shadow_map_tex.Sample(ShadowMapSampler, shadow_tex_cood).r;
	//shadow_depth = zn * q / (q - shadow_depth);

	//
	float4 pos_light = mul(world_pos, g_light_view_proj);
	pos_light /= pos_light.w;
	pos_light.x = pos_light.x / 2 + 0.5f;
	pos_light.y = -pos_light.y / 2 + 0.5f;
	
	float zf = 1000.0f;
	float zn = 1.0f;
	float q = zf/ (zf-zn);
	float pos_depth = pos_light.z;
	pos_depth = zn * q / (q - pos_depth);

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
		shadow = 0;
	if(0)
	{
		return float4(moments.x/1000 ,moments.x/1000,moments.x/1000 ,1);
		return world_pos;
		//return float4(shadow_map_tex.Load( samplelndices ).rrr/1000.0f, 1.0f);
		//return float4(shadow_depth,shadow_depth,shadow_depth,1.0f);
	}

	//Get Infor from g-buffer
	//vs normal
	float4 normal_t = normal_tex.Load( samplelndices );
	float3 normal = normal_t.xyz;
	//set for those mesh that do not want to do lighting
	if(normal.x ==0 && normal.y ==0&& normal.z ==0)
		return float4(0,0,0,1);

	//normal = mul(normal, (float3x3)g_view_matrix);
	float shininess = normal_t.w;

	float4 occlusion = blur_occlusion_tex.Load( samplelndices );
	//occlusion = float4(0,0,0,0);
	if(0)
		return occlusion;
	//float4 pre_color = lighting_tex.Load( samplelndices );

	//cal lighting
	return CalPreLighting( normal, positionVS, shininess, shadow, occlusion);
	}
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
	if(lighting.w == 1.0f)
	{
		diffuse = DiffuseAlbedo;
		specular = float3(0,0,0);
	}
	//float4 DiffuseAlbedo = gMaterial.Diffuse;
		
	//cal lighting
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
	float2 tex  : TEXCOORD;
};

ShadowVSOut ShadowingVS(VertexIn vin)
{
	ShadowVSOut vout;

	float4x4 world_matrix = mul(g_model_matrix, g_world_matrix);
	float4x4 mvp_matrix = mul(world_matrix ,g_view_proj_matrix);
	vout.position_h = mul(float4(vin.pos, 1.0f), mvp_matrix);
	vout.tex  = vin.tex_cood;

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

