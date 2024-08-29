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

void move_dot(t_dot *dot, t_direction direction)
{
    size_t new_pos_x = dot->pos_x;
    size_t new_pos_y = dot->pos_y;

    if (direction == UP && dot->pos_y > 0)
        new_pos_y--;
    else if (direction == DOWN && dot->pos_y < dot->map_ptr->sync->size_y - 1)
        new_pos_y++;
    else if (direction == LEFT && dot->pos_x > 0)
        new_pos_x--;
    else if (direction == RIGHT && dot->pos_x < dot->map_ptr->sync->size_x - 1)
        new_pos_x++;

    pthread_mutex_t *first_lock, *second_lock;
    if (new_pos_x < dot->pos_x || (new_pos_x == dot->pos_x && new_pos_y < dot->pos_y)) {
        first_lock = &dot->map_ptr->chunk[new_pos_x][new_pos_y].mutex;
        second_lock = &dot->map_ptr->chunk[dot->pos_x][dot->pos_y].mutex;
    } else {
        first_lock = &dot->map_ptr->chunk[dot->pos_x][dot->pos_y].mutex;
        second_lock = &dot->map_ptr->chunk[new_pos_x][new_pos_y].mutex;
    }

    pthread_mutex_lock(first_lock);
    pthread_mutex_lock(second_lock);

    dot->map_ptr->chunk[dot->pos_x][dot->pos_y].type = TYPE_EMPTY;
    dot->pos_x = new_pos_x;
    dot->pos_y = new_pos_y;
    dot->map_ptr->chunk[dot->pos_x][dot->pos_y].type = TYPE_DOT;

    pthread_mutex_unlock(second_lock);
    pthread_mutex_unlock(first_lock);
}

void loop_collectible(t_dot *dot)
{
    while (true)
    {
        pthread_mutex_lock(&dot->map_ptr->mutex);
        bool is_collectible = dot->map_ptr->collectible;
        pthread_mutex_unlock(&dot->map_ptr->mutex);

        if (!is_collectible)
            break;

        move_dot(dot, UP);
        set_bool(&dot->map_ptr->mutex, &dot->map_ptr->refresh, true);
        pthread_mutex_lock(&dot->mutex);
        bool is_alive = dot->alive;
        pthread_mutex_unlock(&dot->mutex);
        if (!is_alive)
            break;
    }
}

t_dot *get_dot(t_data *data, size_t pos)
{
    t_dot *current = data->dot;
    while (current)
    {
        if (current->pos == pos)
            return current;
        current = current->next;
    }
    return NULL;
}

void *dot_life(void *in_data)
{
    t_data *data = (t_data *)in_data;
    size_t id = data->nb_dot - 1;

    t_dot *dot = get_dot(data, id);
    if (!dot) return NULL;

    while (true)
    {
        pthread_mutex_lock(&dot->mutex);
        bool is_alive = dot->alive;
        pthread_mutex_unlock(&dot->mutex);

        if (!is_alive)
            break;

        loop_collectible(dot);
        usleep(100);
    }
    return NULL;
}