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
    AddressU = Wrap;
    AddressV = Wrap;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
    vout.pos = float4(vin.pos.xyz, 1.0f);
	vout.tex = float2(vin.pos.x * 0.5 + 0.5, -vin.pos.y * 0.5 + 0.5);
    return vout;
}


float BoxFilterStart( float fWidth )  //Assumes filter is odd
{
    return ( ( fWidth - 1.0f ) / 2.0f );
}
PSOutput PS(VertexOut pin) 
{
	float blurSize = 1.0f/1024.0f;
	PSOutput output;
	
	float4 sum =0.0;
	float2 ScaleU = float2(1,1);
	bool box = false;

	if(box)
	{
    //====================================
	float fFilterWidth = 7;
	bool bVertical = false;
	float fStepSize = 1.0f;
	float fTextureWidth = 1280.0f;
	float fStartOffset = BoxFilterStart( fFilterWidth );
	//float box_filter[7] = {0.006,0.061,0.242,0.383,0.242,0.061,0.006};
	float box_filter[7] = {1,1,1,1,1,1,1};
	float box_w = 7;
	float2 fTexelOffset = float2( bVertical * ( fStepSize / fTextureWidth ), !bVertical * ( fStepSize / fTextureWidth ) );

    
    float2 fTexStart = pin.tex - ( fStartOffset * fTexelOffset );
    
    for( int i = 0; i < fFilterWidth; ++i )
        sum += input_tex_0.Sample( ShadowMapSampler, float2( fTexStart + fTexelOffset * i) ) * box_filter[i];
    
    output.color =  sum/ box_w;
	return output;
	}
	//pin.tex = float2(pin.pos.x * 0.5 + 0.5, -pin.pos.y * 0.5 + 0.5);
	//make weight become true gaussian 
		sum =0.0;
   // sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x - 3.0*blurSize, pin.tex.y)) * 0.0284324;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x - 2.0*blurSize, pin.tex.y)) * 0.0545;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x - blurSize, pin.tex.y)) * 0.2442;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y)) * 0.4026;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x + blurSize, pin.tex.y)) * 0.2442;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x + 2.0*blurSize, pin.tex.y)) * 0.0545;
  //  sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x + 3.0*blurSize, pin.tex.y)) * 0.0284324;

// 	sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x - 3.0*blurSize, pin.tex.y)) * 0.00038771;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x - 2.0*blurSize, pin.tex.y)) * 0.01330373;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x - blurSize, pin.tex.y)) * 0.11098164;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y)) * 0.22508352;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x + blurSize, pin.tex.y)) * 0.11098164;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x + 2.0*blurSize, pin.tex.y)) * 0.01330373;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x + 3.0*blurSize, pin.tex.y)) * 0.00038771;


	//float filter[9] = {0.0162162162, 0.0540540541, 0.1216216216, 0.1945945946, 0.2270270270,
	//					0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162};

	float filter[7] = {0.030078323, 0.104983664, 0.222250419, 0.285375187, 0.222250419,
		0.104983664, 0.030078323};
	//sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y), int2(-4, 0)) * filter[0];
	sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y), int2(-3, 0)) * filter[0];
    sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y), int2(-2, 0)) * filter[1];
    sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y), int2(-1, 0)) * filter[2];
    sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y), int2( 0, 0)) * filter[3];
    sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y), int2( 1, 0)) * filter[2];
    sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y), int2( 2, 0)) * filter[1];
    sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y), int2( 3, 0)) * filter[0];
	//sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y), int2( 4, 0)) * filter[0];

	//5 sample
// 	   sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x - 2.0*blurSize, pin.tex.y)) * 0.0097576615;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x - blurSize, pin.tex.y)) * 0.2058489191;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x, pin.tex.y)) * 0.5687868388;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x + blurSize, pin.tex.y)) * 0.2058489191;
//     sum += input_tex_0.Sample(ShadowMapSampler, float2(pin.tex.x + 2.0*blurSize, pin.tex.y)) * 0.0097576615;

	
/*  //two demension sample 
	sum += input_tex_0.Sample( ShadowMapSampler, pin.tex + float2( -3.0*ScaleU.x, -3.0*ScaleU.y ) ) * 0.015625;
	sum += input_tex_0.Sample( ShadowMapSampler, pin.tex + float2( -2.0*ScaleU.x, -2.0*ScaleU.y ) )*0.09375;
	sum += input_tex_0.Sample( ShadowMapSampler, pin.tex + float2( -1.0*ScaleU.x, -1.0*ScaleU.y ) )*0.234375;
	sum += input_tex_0.Sample( ShadowMapSampler, pin.tex + float2( 0.0 , 0.0) )*0.3125;
	sum += input_tex_0.Sample( ShadowMapSampler, pin.tex + float2( 1.0*ScaleU.x,  1.0*ScaleU.y ) )*0.234375;
	sum += input_tex_0.Sample( ShadowMapSampler, pin.tex + float2( 2.0*ScaleU.x,  2.0*ScaleU.y ) )*0.09375;
	sum += input_tex_0.Sample( ShadowMapSampler, pin.tex + float2( 3.0*ScaleU.x, -3.0*ScaleU.y ) ) * 0.015625;*/
 
 
    output.color = sum;
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
