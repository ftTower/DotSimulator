#include "lifegame.h"

void    free_images(t_data *data)
{
    t_image *tmp;
    t_image *current = data->window->image;
    while(current)
    {
        tmp = current->next;
        mlx_destroy_image(data->window->mlx_ptr, current->img);
        free(current);
        current = tmp;
    }
}

t_image *create_image(void *mlx, int width, int height) {
    t_image *img = malloc(sizeof(t_image));
    if (img == NULL) {
        fprintf(stderr, "Erreur lors de l'allocation de l'image\n");
        exit(1);
    }

    img->img = mlx_new_image(mlx, width, height);
    if (img->img == NULL) {
        fprintf(stderr, "Erreur lors de la création de l'image\n");
        free(img);
        exit(1);
    }
    img->next = NULL;
    img->addr = mlx_get_data_addr(img->img, &img->bits_per_pixel, &img->line_length, &img->endian);
    return img;
}

void    add_image(t_data *data, t_image *image)
{
    if (!data->window->image)
        return (data->window->image = image, (void)0);
    t_image *current = data->window->image;
    while(current->next)
        current = current->next;
    current->next = image;
}

void display_image(void *mlx, void *window, t_image *img, int x, int y) {
    mlx_put_image_to_window(mlx, window, img->img, x, y);
}

void put_pixel(t_image *img, int x, int y, int color) {
    char *pixel = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
    for (int i = 0; i < (img->bits_per_pixel / 8); i++) {
        pixel[i] = (color >> (i * 8)) & 0xFF;
    }
}

void put_chunk(t_image *img, size_t x_pos, size_t y_pos, int color) {
    for (size_t x = x_pos; x < x_pos + CHUNK_SIZE; x++)
        for (size_t y = y_pos; y < y_pos + CHUNK_SIZE; y++)
            put_pixel(img, x, y, color);
}

void refresh_map(t_data *data)
{
    bool refresh;
    pthread_mutex_lock(&data->map->mutex);
    refresh = data->map->refresh;
    pthread_mutex_unlock(&data->map->mutex);
    if (!refresh)
        return ;
    set_bool(&data->map->mutex, &data->map->refresh, false);
    add_image(data, create_image(data->window->mlx_ptr, data->sync->size_x * CHUNK_SIZE, data->sync->size_y * CHUNK_SIZE));
    
    for(size_t x = 0; x < data->sync->size_x; x++)
        for(size_t y = 0; y < data->sync->size_y ;y++ )
        {
            if (data->map->chunk[x][y].type == TYPE_EMPTY)
                put_chunk(data->window->image, x * CHUNK_SIZE, y * CHUNK_SIZE, 0xFFFFFF);
            else if (data->map->chunk[x][y].type == TYPE_WALL)
                put_chunk(data->window->image, x * CHUNK_SIZE, y * CHUNK_SIZE, 0x000000);
            else if (data->map->chunk[x][y].type == TYPE_DOT)
                put_chunk(data->window->image, x * CHUNK_SIZE, y * CHUNK_SIZE, 0xBB0A1E);
            else if (data->map->chunk[x][y].type == TYPE_COLLECTIBLE)
                put_chunk(data->window->image, x * CHUNK_SIZE, y * CHUNK_SIZE, 0x01ff05);
        }
    display_image(data->window->mlx_ptr, data->window->win_ptr, data->window->image, 0, 0);
}