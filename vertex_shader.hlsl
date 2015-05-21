float4x4 worldViewProj;

struct VS_INPUT
{
	float3 position  : POSITION;
	float4 color0    : COLOR0;
	float2 texcoord0 : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 hposition : POSITION;
	float4 color0    : COLOR0;
	float2 texcoord0 : TEXCOORD0;
	float4 pos : TEXCOORD1;
};

VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT;

	float4 v = float4( IN.position.x,
		               IN.position.y,
					   IN.position.z,
					   1.0f );

    OUT.hposition = mul( v, worldViewProj );
    OUT.color0    = IN.color0;
    OUT.texcoord0 = IN.texcoord0;
	OUT.pos = OUT.hposition;

    return OUT;
}
