#include "lifegame.h"
#include <stdlib.h>

void    gen_map(t_data *data, size_t ratio)
{
    res_map(data);
    free_dots(data);
    for (size_t x = 0; x < data->sync->size_x; x++)
    {
        for (size_t y = 0; y < data->sync->size_y; y++)
            if (rand() % ratio == 0)
                data->map->chunk[x][y].type = TYPE_WALL;
    }
}

void    res_map(t_data *data)
{
    for (size_t x = 0; x < data->sync->size_x; x++)
        for (size_t y = 0; y < data->sync->size_y; y++)
            data->map->chunk[x][y].type = TYPE_EMPTY;
}

void free_map(t_data *data)
{
    if (data->map && data->map->chunk)
    {
        for (size_t x = 0; x < data->sync->size_x; x++)
            if (data->map->chunk[x])
                free(data->map->chunk[x]);
        free(data->map->chunk); 
    }
    free(data->map);
    data->map = NULL; 
}