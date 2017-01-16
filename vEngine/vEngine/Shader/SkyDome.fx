TextureCube  background_tex;
cbuffer cbPerObject
{
	float4x4 g_world_matrix;
	float4x4 g_model_matrix;
	float4x4 g_view_matrix;
	float4x4 g_view_proj_matrix;
	float4x4 g_world_inv_transpose;
	float4x4 g_inv_proj_matrix;
	float4x4 g_inv_view_matrix;

	float3 g_eye_pos;
}
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


struct VertexIn
{
	float3 pos  : POSITION;
};

struct VertexOut
{
	float4 pos			: SV_POSITION;
	float3 posL 		: POSITION;
};
struct GbufferPSOutput
{
	float4 Normal			: SV_Target0;
	float4 Diffuse			: SV_Target1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	float4x4 world_matrix = mul(g_model_matrix, g_world_matrix);
	float4x4 mvp_matrix = mul(world_matrix ,g_view_proj_matrix);
	vout.pos =   mul(float4(vin.pos, 1.0f), mvp_matrix).xyww;

	//vout.pos =  float4(vin.pos.xyz, 1.0f);
	vout.posL = vin.pos;
    
    return vout;
}

GbufferPSOutput PS(VertexOut pin) 
{
	//return float4(1,1,0,1);
	GbufferPSOutput output;

	output.Normal = float4(0,0,0,0);	
	output.Diffuse = background_tex.Sample(SampleType, pin.posL);	

	return output;
}

DepthStencilState LessEqualDSS
{
	DepthFunc = LESS_EQUAL;
};
RasterizerState NoCull
{
	CullMode = None;
};

technique11 SkyDomeTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );

		SetRasterizerState(NoCull);
		SetDepthStencilState(LessEqualDSS, 0);
    }
}
//verbose tech for shadowing
technique11 GbufferTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );

		SetRasterizerState(NoCull);
		SetDepthStencilState(LessEqualDSS, 0);
    }
}
VertexOut ShadowingVS(VertexIn vin)
{
	VertexOut vout;
	vout.pos =   float4(0,0,0,0);
	vout.posL =  float3(0,0,0);
    
    return vout;
}
technique11 Shadowing
{
	pass P0
    {
        SetVertexShader(  CompileShader( vs_5_0, ShadowingVS() ) );
		SetGeometryShader( NULL );
        SetPixelShader(NULL);		
    }
};
