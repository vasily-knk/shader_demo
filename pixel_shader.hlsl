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


noise_tuning_t big_noise_tuning(in noise_tuning_t original_tuning, in float big_noise_value)
{
    const float b = lerp(big_noise_value, 1., 0.5);
    
    noise_tuning_t tuning = original_tuning;
    tuning.edge1 = lerp(0.99, tuning.edge1, b);
    tuning.edge2 = lerp(1.  , tuning.edge2, b);
    return tuning;
}

multi_noise_t make_big_noise_params(float2 world_coord)
{
    float2 world_velocity = float2(1., 0.);
    float2 noise_scale = float2(0.2, 0.2);

    multi_noise_t params;

    params.coord = world_coord * noise_scale;
    params.offset = world_velocity * noise_scale * time;
    params.m = 0.5;
    params.mult_coord = float2(2., 2.);
    params.mult_offset = float2(2., 2.);
    params.mult_m = 0.5;
    params.num_octaves = 4;
    params.tuning = make_noise_tuning(0., 1., 0.3, 0.5);

    return params;
}

multi_noise_t make_noise_params(float2 world_coord, float big_noise_value)
{
    float2 world_velocity = float2(2., 0.);
    float2 noise_scale = float2(0.2, 1.);

    multi_noise_t params;

    params.coord = world_coord * noise_scale;
    params.offset = world_velocity * noise_scale * time;
    params.m = 0.5;
    params.mult_coord = float2(2., 2.);
    params.mult_offset = float2(2.2, 2.2);
    params.mult_m = 0.5;
    params.num_octaves = 8;
    params.tuning = big_noise_tuning(make_noise_tuning(0., 1., 0., 1.), big_noise_value);

    return params;
}

PS_OUTPUT main( VS_OUTPUT IN )
{
    PS_OUTPUT o;

    float2 world_scale = float2(10., 10.);
    float2 world_coord = IN.texcoord0 * world_scale;

    multi_noise_t big_noise_params = make_big_noise_params(world_coord);
    float big_noise_value = apply_multi_noise(big_noise_params);
    
    multi_noise_t params = make_noise_params(world_coord, big_noise_value);
    float n = apply_multi_noise(params);

	o.color = float4(n, n, n, 1.);
    return o;
}
