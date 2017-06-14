

//this is a varible list shared by all shaders
cbuffer cbPerShader
{
	//model matrix: it can be a model * world or only model matrix
	float4x4 g_model_matrix;
	float4x4 g_view_proj_matrix;
	float3   g_eye_pos;
}