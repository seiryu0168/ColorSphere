cbuffer global
{
	float4x4 g_matWVP;//ワールド、ビュー、プロジェクション
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float3 color : COLOR0;
};

VS_OUT VS( float3 pos : POSITION )// : SV_POSITION
{
	VS_OUT outData;
	float4 pos4=float4(pos, 0)*100;
	outData.position = mul(pos4, g_matWVP);
	outData.color = pos;
	return outData;
}

float4 PS(VS_OUT inData):SV_Target
{
	return float4(inData.color,1);
}