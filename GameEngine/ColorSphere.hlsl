cbuffer global
{
	float4x4 g_matWVP;//ワールド、ビュー、プロジェクション
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float3 color : COLOR0;
};

VS_OUT VS( float4 pos : POSITION )// : SV_POSITION
{
	VS_OUT outData;
	outData.position = mul(pos, g_matWVP);
	outData.color = pos;
	return outData;
}

float4 PS(VS_OUT inData):SV_Target
{
	return float4(inData.color,1);
}