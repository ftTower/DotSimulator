#include "lifegame.h"

bool    init_map(t_data *data)
{
    t_map *new;

    new = malloc(sizeof(t_map));
    if (!new)
        return (true);
    new->chunk = malloc(sizeof(t_chunk) * (data->sync->size_x + 1));
    new->chunk[data->sync->size_x] = NULL;
    for (size_t x = 0; x < data->sync->size_x; x++) {
        new->chunk[x] = malloc(sizeof(t_chunk) * (data->sync->size_y + 1));
        for(size_t y = 0; y < data->sync->size_y; y++)
        {
            new->chunk[x][y].pos_x = x;
            new->chunk[x][y].pos_y = y;
            new->chunk[x][y].type = TYPE_EMPTY;
            pthread_mutex_init(&new->chunk[x][y].mutex, NULL);
        }
    }
    data->map = new;
    data->map->sync = data->sync;
    return (false);
}

bool    init_dot(t_data *data)
{
    data->dot = NULL;
    return (false);
}

bool    init_sync(t_data *data, char **argv)
{
    data->sync = malloc(sizeof(t_sync));
    if (!argv[0] || !argv[1] || !argv[2])
        return (true);
    data->sync->size_x = atoi(argv[1]) / CHUNK_SIZE;
    data->sync->size_y = atoi(argv[2]) / CHUNK_SIZE;
    printf("\t[x:%zu y:%zu]\n\n", data->sync->size_x, data->sync->size_y);
    return (false);
}

bool    init_window(t_data *data)
{
    data->window = malloc(sizeof(t_win));
    data->window->image = NULL;
    data->window->mlx_ptr = mlx_init();
    data->window->win_ptr = mlx_new_window(data->window->mlx_ptr, data->sync->size_x * CHUNK_SIZE, data->sync->size_y * CHUNK_SIZE, "lifegame");

    return (false);
}

bool    init_data(t_data **data, char **argv)
{
    *data = malloc(sizeof(t_data));
    if (!*data)
        return (true);
    if (init_dot(*data) || init_sync(*data, argv) || init_map(*data) || init_window(*data))
        return (printf("\n\t[init pas ok]\n\n"),free_all(*data),true);
    return (printf("\n\t[init ok]\n\n"),false);
}