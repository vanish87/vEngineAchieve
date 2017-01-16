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

//position
Texture2D input_tex_0;
//normal
Texture2D input_tex_1;
//noise
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
	float2 tex			: TEXCOORD0;	
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
	int3 samplelndices = int3( pin.pos.xy, 0 );
	float3 world_pos =  input_tex_0.Load( samplelndices ).rgb;

	float3 points[] =
	{
		float3(-0.134, 0.044, -0.825),
		float3(0.045, -0.431, -0.529),
		float3(-0.537, 0.195, -0.371),
		float3(0.525, -0.397, 0.713),
		float3(0.895, 0.302, 0.139),
		float3(-0.613, -0.408, -0.141),
		float3(0.307, 0.822, 0.169),
		float3(-0.819, 0.037, -0.388),
		float3(0.376, 0.009, 0.193),
		float3(-0.006, -0.103, -0.035),
		float3(0.098, 0.393, 0.019),
		float3(0.542, -0.218, -0.593),
		float3(0.526, -0.183, 0.424),
		float3(-0.529, -0.178, 0.684),
		float3(0.066, -0.657, -0.570),
		float3(-0.214, 0.288, 0.188),
		float3(-0.689, -0.222, -0.192),
		float3(-0.008, -0.212, -0.721),
		float3(0.053, -0.863, 0.054),
		float3(0.639, -0.558, 0.289),
		float3(-0.255, 0.958, 0.099),
		float3(-0.488, 0.473, -0.381),
		float3(-0.592, -0.332, 0.137),
		float3(0.080, 0.756, -0.494),
		float3(-0.638, 0.319, 0.686),
		float3(-0.663, 0.230, -0.634),
		float3(0.235, -0.547, 0.664),
		float3(0.164, -0.710, 0.086),
		float3(-0.009, 0.493, -0.038),
		float3(-0.322, 0.147, -0.105),
		float3(-0.554, -0.725, 0.289),
		float3(0.534, 0.157, -0.250),
	};

	const int num_samples = 32;
	float g_occlusion_radius = 0.26;
	float g_occlusion_max_distance = 0.68;
	float2 g_resolution = float2(1280,800);
	float2 noise_texture_size = float2(4,4);
	float3 center_pos = mul(float4(world_pos.xyz,1.0), g_view_matrix).xyz;
	//float3 eye_pos = g_inv_view_matrix[3].xyz;

	float  center_depth  = distance(float3(0,0,0), center_pos);

	float radius = g_occlusion_radius / center_depth;
	float max_distance_inv = 1.f / g_occlusion_max_distance;
	float attenuation_angle_threshold = 0.1;

	float3 noise = input_tex_2.Sample(ShadowMapSampler, pin.tex*g_resolution.xy/noise_texture_size).xyz*2-1;

	float4 normal_t = input_tex_1.Load( samplelndices );
	float4 normalVS = normal_t;
	float3 center_normal = normalVS.xyz;

	float4 occlusion_sh2 = 0;

	const float fudge_factor_l0 = 2.0;
	const float fudge_factor_l1 = 10.0;

	const float sh2_weight_l0 = fudge_factor_l0 * 0.28209; //0.5*sqrt(1.0/pi);
	const float3 sh2_weight_l1 = fudge_factor_l1 * 0.48860; //0.5*sqrt(3.0/pi);

	const float4 sh2_weight = float4(sh2_weight_l1, sh2_weight_l0) / num_samples;
	
	float2 dx, dy;
	dx = ddx( pin.tex );
	dy = ddy( pin.tex );
	[unroll] // compiler wants to branch here by default and this makes it run nearly 2x slower on PC and 1.5x slower on 360!
	for( int i=0; i<num_samples; ++i )
	{
	    float2 textureOffset = reflect( points[ i ].xy, noise.xy ).xy * radius;
		float2 sample_tex = pin.tex + textureOffset;
		float4 world_pos = input_tex_0.SampleGrad( ShadowMapSampler, sample_tex, dx, dy ).rgba;		
		float3 sample_pos = mul(world_pos, g_view_matrix).xyz;
		float3 center_to_sample = sample_pos - center_pos;
		float dist = length(center_to_sample);
		float3 center_to_sample_normalized = center_to_sample / dist;
		float attenuation = 1-saturate(dist * max_distance_inv);
		float dp = dot(center_normal, center_to_sample_normalized);

		attenuation = attenuation*attenuation * step(attenuation_angle_threshold, dp);

		occlusion_sh2 += attenuation * sh2_weight*float4(center_to_sample_normalized,1);
	}

	PSOutput output;
	output.color =  occlusion_sh2;
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
