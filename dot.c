#include "lifegame.h"

void free_dots(t_data *data)
{
    t_dot *tmp;
    t_dot *current = data->dot;
    while (current)
    {
        tmp = current->next;
        pthread_mutex_lock(&current->mutex);
        current->alive = false;
        pthread_mutex_unlock(&current->mutex);
        usleep(100);
        pthread_join(current->thread, NULL);
        pthread_mutex_destroy(&current->mutex);
        free(current);
        current = tmp;
    }
    data->dot = NULL;
}

void gen_dot_pos(t_data *data, t_dot *dot)
{
    pthread_mutex_lock(&data->map->mutex); // Lock the map's mutex to ensure thread safety
    while(true){
        dot->pos_x = rand() % data->sync->size_x;
        dot->pos_y = rand() % data->sync->size_y;
        pthread_mutex_lock(&data->map->chunk[dot->pos_x][dot->pos_y].mutex);
        if (data->map->chunk[dot->pos_x][dot->pos_y].type == TYPE_EMPTY)
        {
            data->map->chunk[dot->pos_x][dot->pos_y].type = TYPE_DOT;
            pthread_mutex_unlock(&data->map->chunk[dot->pos_x][dot->pos_y].mutex);
            break;   
        }
        pthread_mutex_unlock(&data->map->chunk[dot->pos_x][dot->pos_y].mutex);
    }
    pthread_mutex_unlock(&data->map->mutex);
}

t_dot *get_last_dot(t_data *data)
{
    t_dot *ret = data->dot;
    while(ret && ret->next)
        ret = ret->next;
    return ret;
}

void add_dot(t_data *data)
{
    t_dot *new = malloc(sizeof(t_dot));
    if (!new)
    {
        perror("Failed to allocate memory for new dot");
        return;
    }
    new->pos_x = 0;
    new->pos_y = 0;
    new->map_ptr = data->map;
    new->next = NULL;
    new->alive = true;
    pthread_mutex_init(&new->mutex, NULL);

    gen_dot_pos(data, new);

    pthread_mutex_lock(&data->dots_mutex);
    if (!data->dot)
        data->dot = new;
    else
    {
        t_dot *current = data->dot;
        while (current->next)
            current = current->next;
        pthread_mutex_lock(&current->mutex);
        current->next = new;
        pthread_mutex_unlock(&current->mutex);
    }
    pthread_mutex_unlock(&data->dots_mutex);
    set_bool(&data->map->mutex, &data->map->refresh, true);
    int ret = pthread_create(&new->thread, NULL, dot_life, data);
    if (ret != 0)
    {
        perror("Failed to create dot thread");
        pthread_mutex_destroy(&new->mutex);
        free(new);
    }
    
}

void del_dot(t_data *data)
{
    if (!data->dot)
        return;
    data->nb_dot--;
    t_dot *current = data->dot;
    if (!current->next)
    {
        pthread_mutex_lock(&data->map->chunk[current->pos_x][current->pos_y].mutex);
        data->map->chunk[current->pos_x][current->pos_y].type = TYPE_EMPTY;
        pthread_mutex_unlock(&data->map->chunk[current->pos_x][current->pos_y].mutex);

        pthread_mutex_lock(&current->mutex);
        current->alive = false;
        pthread_mutex_unlock(&current->mutex);
        usleep(100);
        pthread_join(current->thread, NULL);
        free(current);
        data->dot = NULL;
        set_bool(&data->map->mutex, &data->map->refresh, true);
        return;
    }
    while (current->next->next)
        current = current->next;
    t_dot *to_delete = current->next;
    pthread_mutex_lock(&data->map->mutex);
    data->map->chunk[to_delete->pos_x][to_delete->pos_y].type = TYPE_EMPTY;
    pthread_mutex_unlock(&data->map->mutex);
    pthread_mutex_lock(&to_delete->mutex);
    to_delete->alive = false;
    pthread_mutex_unlock(&to_delete->mutex);
    usleep(100);
    pthread_join(to_delete->thread, NULL);
    free(to_delete);
    current->next = NULL;
    set_bool(&data->map->mutex, &data->map->refresh, true);
}