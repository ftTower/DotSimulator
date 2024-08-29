#include "lifegame.h"
#include <stdlib.h>

void    gen_map(t_data *data, size_t ratio)
{
    res_map(data);
    for (size_t x = 0; x < data->sync->size_x; x++)
    {
        for (size_t y = 0; y < data->sync->size_y; y++)
            if (rand() % ratio == 0)
                data->map->chunk[x][y].type = TYPE_WALL;
    }
    gen_collectible(data);
    set_bool(&data->map->mutex, &data->map->refresh, true);
}

void    gen_collectible(t_data *data)
{
    if (data->map->collectible)
    {
        pthread_mutex_destroy(&data->map->collectible->mutex);
        free(data->map->collectible);
        data->map->collectible = NULL;
    }
    data->map->collectible = malloc(sizeof(t_collectible));
    if (!data->map->collectible)
        return ;
    pthread_mutex_init(&data->map->collectible->mutex, NULL);
    data->map->collectible->pos_x = rand() % data->sync->size_x;
    data->map->collectible->pos_y = rand() % data->sync->size_y;
    if (data->map->chunk[data->map->collectible->pos_x][data->map->collectible->pos_y].type == TYPE_EMPTY)
        data->map->chunk[data->map->collectible->pos_x][data->map->collectible->pos_y].type = TYPE_COLLECTIBLE;
    else
        gen_collectible(data);
}

void    res_map(t_data *data)
{
    for (size_t x = 0; x < data->sync->size_x; x++)
        for (size_t y = 0; y < data->sync->size_y; y++)
            data->map->chunk[x][y].type = TYPE_EMPTY;
    free_dots(data);
    bool is_collectible = false;
    if (data->map->collectible)
        is_collectible = true;
    if (get_bool(&data->map->mutex, &is_collectible))
    {
        free(data->map->collectible);
        data->map->collectible = NULL;
    }
    set_bool(&data->map->mutex, &data->map->refresh, true);
    // refresh_map(data);
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