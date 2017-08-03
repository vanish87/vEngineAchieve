#include "CommonHeader.fx"

cbuffer cbPerObject
{
	float4x4 g_world_matrix;
	float4x4 g_view_matrix;
	float4x4 g_mwv_inv_transpose;
	float4x4 g_inv_proj_matrix;
	float4x4 g_inv_view_matrix;
};

Texture2D input_tex_0;
Texture2D input_tex_1;
Texture2D input_tex_2;

cbuffer cbPerFrame
{
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

SamplerState ShadowMapSampler
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
	float zf = 1000.0f;
	float zn = 1.0f;
	float q = zf/ (zf-zn);
	PSOutput output;
	float dis = input_tex_0.Sample(ShadowMapSampler,pin.tex).x;
	dis = zn * q / (q - dis);
	float2 dxdy = float2(ddx(dis), ddy(dis));
	// G chanel for vsm
	output.color = float4(dis, dis * dis + 0.25f * dot(dxdy, dxdy), 0, 1);
	return output;
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
