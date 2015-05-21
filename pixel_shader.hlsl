#include "noise.hlsl"

float time = 0;

struct VS_OUTPUT
{
	float4 hposition : TEXCOORD0;
	float4 color0    : COLOR0;
	float2 texcoord0 : TEXCOORD0;
	float4 pos : TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 color : COLOR;
};

sampler testTexture;


PS_OUTPUT main( VS_OUTPUT IN )
{
    PS_OUTPUT o;

    float2 screen_coord = IN.texcoord0;
    float2 screen_velocity = float2(0.25, 0.);

    float noise_scale = float2(10., 10.);

    multi_noise_t params;

	params.coord = screen_coord * noise_scale;
	params.offset = screen_velocity * noise_scale * time;
    params.m = 0.5;
    params.mult_coord = float2(2., 2.);
    params.mult_offset = float2(1., 1.);
    params.mult_m = 0.5;
    params.num_octaves = 8;
    params.tuning = make_noise_tuning(0., 1., 0.2, 0.5);



	float n = apply_multi_noise(params);

	o.color = float4(n, n, n, 1.);
    return o;
}
