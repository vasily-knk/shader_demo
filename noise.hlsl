typedef float2 noise_coord_t;

float shadertoy_noise2d_hash(float n)
{ 
	return frac(sin(n)*43758.5453123); 
}

float shadertoy_noise(in noise_coord_t x)
{
    float2 p = floor(x.xy);
    float2 f = frac(x.xy);
    f = f*f*(3.0-2.0*f);
	
    float n = p.x + p.y*157.0;
    float r = lerp(
        lerp(shadertoy_noise2d_hash(n +   0.0), shadertoy_noise2d_hash(n +   1.0), f.x),
        lerp(shadertoy_noise2d_hash(n + 157.0), shadertoy_noise2d_hash(n + 158.0), f.x),
        f.y);
    
	return r;
}

struct noise_tuning_t
{
    float min, max, edge1, edge2;    
};

noise_tuning_t make_noise_tuning(in float min, in float max, in float edge1, in float edge2)
{
    noise_tuning_t tuning;
    
    tuning.min = min;
    tuning.max = max;
    tuning.edge1 = edge1;
    tuning.edge2 = edge2;
    
    return tuning;
}

struct multi_noise_t
{
	noise_coord_t coord;
	noise_coord_t offset;
    float m;

	noise_coord_t mult_coord;
	noise_coord_t mult_offset;
    float mult_m;

    noise_tuning_t tuning;
    int num_octaves;
};

float tune_noise(in float val, in noise_tuning_t tuning)
{
    val = smoothstep(tuning.edge1, tuning.edge2, val);
    val = clamp(val, 0., 1.);
    val = lerp(tuning.min, tuning.max, val);
    return val;
}

float apply_multi_noise(in multi_noise_t params)
{
    float f = 0.;
    
    noise_coord_t coord = params.coord;
    noise_coord_t offset = params.offset;
    float m = params.m;
    
    for (int i = 0; i < params.num_octaves; ++i)
    {
        float n = shadertoy_noise(coord + offset);
        
        f += m * n;
        coord *= params.mult_coord;
        offset *= params.mult_offset;
        m *= params.mult_m;
    }

    f = tune_noise(f, params.tuning);
	
    return f;
}
