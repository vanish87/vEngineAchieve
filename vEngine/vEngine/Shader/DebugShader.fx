cbuffer cbPerObject
{
	float4x4 g_world_matrix;
	float4x4 g_model_matrix;
	float4x4 g_view_matrix;
	float4x4 g_view_proj_matrix;
	float4x4 g_mwv_inv_transpose;
	float4x4 g_inv_proj_matrix;
	float4x4 g_inv_view_matrix;
};

Texture2D input_tex_0;
Texture2D input_tex_1;
Texture2D input_tex_2;

cbuffer cbPerFrame
{
	float3 g_eye_pos;
	float3 g_eye_z;
};

struct VertexIn
{
	float3 pos  : POSITION;
};

struct VertexOut
{
	float4 pos			: SV_POSITION;
	float2 tex			: TEXCOORD;
};
struct PSOutput
{
	float4 color		: SV_Target0;
};

SamplerState OutTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
    vout.pos = float4(vin.pos.xyz, 1.0f);
	vout.tex = float2(vin.pos.x * 0.5 + 0.5, -vin.pos.y * 0.5 + 0.5);
    return vout;
}

PSOutput PS(VertexOut pin) 
{
	PSOutput ret;
	ret.color = input_tex_0.Sample(OutTextureSampler, pin.tex).r/100;
	ret.color = ret.color.r > 0 && ret.color.r < 1 ? 1 : 0;
	return ret;
}

technique11 PPTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
