
float time = 0;

struct VS_OUTPUT
{
	float4 color0    : COLOR0;
	float2 texcoord0 : TEXCOORD0;
	float4 pos : TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 color : COLOR;
};

sampler testTexture;

struct args_t
{
    float2 xy;
    sampler s;
};

float4 get_color(args_t args)
{
    return tex2Dlod(args.s, float4(args.xy * 32.f, 0.f, 6.f));
}

PS_OUTPUT main( VS_OUTPUT IN )
{
    PS_OUTPUT o;

    args_t args;
    args.xy = IN.texcoord0;
    args.s = testTexture;
    
    o.color = get_color(args);

    return o;
}
