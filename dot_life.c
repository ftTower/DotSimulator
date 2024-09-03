#include "lifegame.h"

typedef enum e_direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
} t_direction;

void set_chunk_type(t_map *map, size_t pos_x, size_t pos_y, t_type to_set)
{
    pthread_mutex_lock(&map->chunk[pos_x][pos_y].mutex);
    map->chunk[pos_x][pos_y].type = to_set;
    pthread_mutex_unlock(&map->chunk[pos_x][pos_y].mutex);
}

bool is_valid_position(t_map *map, size_t x, size_t y)
{
    if ( x >= map->sync->size_x || y >= map->sync->size_y)
        return false;
    if (map->chunk[y][x].type != TYPE_EMPTY) 
        return false;
    return true; 
}

void move_dot(t_dot *dot, int direction)
{
    write(1, "Attempting to move dot...\n", 27);
    pthread_mutex_lock(&dot->map_ptr->mutex);

    int new_x = dot->pos_x;
    int new_y = dot->pos_y;

    switch (direction)
    {
        case UP:    new_y -= 1; break;
        case DOWN:  new_y += 1; break;
        case LEFT:  new_x -= 1; break;
        case RIGHT: new_x += 1; break;
        default:    break;
    }

    if (is_valid_position(dot->map_ptr, new_x, new_y))
    {
        dot->pos_x = new_x;
        dot->pos_y = new_y;
        write(1, "dot moved\n", 10);
    } else {
        write(1, "dot invalid move\n", 18);
    }
    pthread_mutex_unlock(&dot->map_ptr->mutex);
}

void loop_collectible(t_dot *dot, t_data *data)
{
    while (true)
    {
        pthread_mutex_lock(&dot->map_ptr->mutex);
        bool is_collectible = dot->map_ptr->collectible;
        pthread_mutex_unlock(&dot->map_ptr->mutex);
        if (!is_collectible)
            break;
        pthread_mutex_lock(&data->dots_mutex);

        move_dot(dot, UP);
        move_dot(dot, RIGHT);
        move_dot(dot, DOWN);


        pthread_mutex_lock(&dot->mutex);
        bool is_alive = dot->alive;
        pthread_mutex_unlock(&dot->mutex);
        if (!is_alive)
            break;

        usleep(10000);
        pthread_mutex_unlock(&data->dots_mutex);
    }
}

t_dot* get_dot(t_data* data, size_t id) {
    if (data == NULL || id >= data->nb_dot) {
        return NULL;
    }

    return &(data->dot[id]);
}

void *dot_life(void *in_data)
{
    t_data *data = (t_data *)in_data;
    printf("%ld\n", data->nb_dot);
    size_t id = data->nb_dot;

    t_dot *dot = get_dot(data, id);
    if (!dot) {
        printf("Failed to get dot\n");
        return NULL;
    }
    printf("ui\n");

    while (true)
    {
        pthread_mutex_lock(&dot->mutex);
        bool is_alive = dot->alive;
        pthread_mutex_unlock(&dot->mutex);
        if (!is_alive)
            break;

        loop_collectible(dot, data);
        refresh_map(data);
        usleep(10000);
    }
    return NULL;
}