#include "lifegame.h"

void    print_term_chunk(t_chunk *chunk)
{
    if (chunk->type == TYPE_EMPTY)
        printf("\033[48;5;96m");
    else if (chunk->type == TYPE_WALL)
        printf("\033[48;5;94m");
    else if (chunk->type == TYPE_DOT)
        printf("\033[48;5;98m"); 
    printf("[%-2ld:%-2ld]\033[0m", chunk->pos_x, chunk->pos_y);
}

void    print_term_map(t_data *data)
{
    for (size_t i = 0; data->map->chunk[i]; i++)
    {
        for (size_t y = 0; y < data->sync->size_y; y++)
            print_term_chunk(&data->map->chunk[i][y]);
        printf("\n");
    }
}

void    free_all(t_data *data)
{
    // print_term_map(data);
    if (data->map->collectible)
        free(data->map->collectible);
    free_images(data);
    free_dots(data);
    mlx_destroy_window(data->window->mlx_ptr, data->window->win_ptr);
    mlx_destroy_display(data->window->mlx_ptr);
    free(data->window->mlx_ptr);
    free(data->window);
    free_map(data);
    free(data->sync);
    free(data);
}

int key_event(int keycode, t_data *data) {
    // Optionnel: On peut aussi stocker le nom de la touche
    if (keycode == 65307)
        return (free_all(data), exit(EXIT_SUCCESS), 1);
    else if (keycode == 65438)
        res_map(data);
    else if (keycode == 65436)
        gen_map(data, 3);
    else if (keycode == 65451)
        add_dot(data);
    else if (keycode == 65453)
        del_dot(data);
    else
        printf("not in %d\n", keycode);
    refresh_map(data);
    return (0);
}

void    loop(t_data *data)
{
    set_bool(&data->map->mutex, &data->map->refresh, true);
    mlx_key_hook(data->window->win_ptr, key_event, data);
    mlx_loop(data->window->mlx_ptr);
}

int main(int argc, char **argv)
{
    t_data *data = NULL;

    if (argc < 3 || argc > 3)
        return (printf("argc pas ok\n"),1);
    else if (init_data(&data, argv))
        return (2);
    srand(time(NULL));
    refresh_map(data);
    loop(data);
    return (free_all(data), 0);
}
