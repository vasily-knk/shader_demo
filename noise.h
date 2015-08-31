#pragma once

struct noise_tuning_t
{
    float min, max, edge1, edge2;    
};

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
