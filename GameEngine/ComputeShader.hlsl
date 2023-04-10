int threadX = 16;
int threadY = 16;
int threadZ = 1;

//////////////////////コンピュートシェーダー/////////////////////////////////
struct output
{
	float3 color;
};
Texture2D<float4> g_Texture : register(t0);
SamplerState g_Sampler : register(s0);
//output
RWStructuredBuffer<output>outBuffer : register(u0);

[numthreads(16,16,1)]
void CS( uint2 DTid : SV_DispatchThreadID )
{
	float2 index;
	index.x = DTid.x;
	index.y = DTid.y;
	float3 pixelColor = g_Texture[DTid].rgb;// float3(index.x, index.y, 10);
	outBuffer[index.x].color = pixelColor;
	//outBuffer[index].r = pixelColor.x;
	//outBuffer[index].g = pixelColor.y;
	//outBuffer[index].b = pixelColor.z;
	//return pixelColor;
}
///////////////////////////////////////////////////////////////////////////////

////////////////////////////バーテックスシェーダー/////////////////////////////
//cbuffer global
//{
//	float4x4 g_matWVP;
//	float4x4 g_matNormal;
//};
//
//struct VS_OUT
//{
//	float4 position : SV_POSITION;
//	float3 color : COLOR0;
//};

//VS_OUT VS(float4 pos : POSITION)
//{
//	VS_OUT outData;
//	outData.position = mul(pos, g_matWVP);
//	outData.color = pos.xyz;
//}
///////////////////////////////////////////////////////////////////////////////
//struct GS_OUT
//{
//	float4 position;
//	float3 color;
//};

//GS_OUT GS(VS_OUT inData)
//{
//	GS_OUT outData;
//	outData.position = mul(inData.position, g_matWVP);
//	outData.color = inData.color;
//}
////////////////////////////ピクセルシェーダー/////////////////////////////////
//float4 PS(VS_OUT inData) : SA_Target
//{
//	return float4(inData.color,1);
//}