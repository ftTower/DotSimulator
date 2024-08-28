#include "lifegame.h"

void free_dots(t_data *data)
{
    t_dot *tmp;
    t_dot *current = data->dot;
    while (current)
    {
        tmp = current->next;
        pthread_join(current->thread, NULL);
        free(current);
        current = tmp;
    }
    data->dot = NULL;
}

void    gen_dot_pos(t_data *data, t_dot *dot)
{
    dot->pos_x = rand() % data->sync->size_x;
    dot->pos_y = rand() % data->sync->size_y;
    if (data->map->chunk[dot->pos_x][dot->pos_y].type == TYPE_EMPTY)
        data->map->chunk[dot->pos_x][dot->pos_y].type = TYPE_DOT;
    else
        gen_dot_pos(data, dot);
}

void    *dot_life(void *in_data)
{
    t_dot *dot;

    dot = (t_dot *)in_data;
    while(dot->alive)
    {
        // printf("%ld im alive\n", dot->pos);
        sleep(1);
    }
    return (NULL);
}

void set_dot_pos(t_data *data)
{
    size_t index = 0;
    t_dot *current = data->dot;
    while(current)
    {
        current->pos = index++;
        current = current->next;
    }
}

t_dot   *get_last_dot(t_data *data)
{
    t_dot *ret = data->dot;
    while(ret->next)
        ret = ret->next;
    return (ret);
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
    new->alive = true;
    new->map_ptr = data->map;
    gen_dot_pos(data, new);
    new->next = NULL;
    if (!data->dot)
        data->dot = new;
    else
    {
        t_dot *current = data->dot;
        while (current->next)
            current = current->next;
        current->next = new;
    }
    int ret = pthread_create(&new->thread, NULL, dot_life, get_last_dot(data));
    if (ret != 0)
        free(new);
}

void    del_dot(t_data *data)
{
    if (!data->dot)
        return ;
    t_dot *current = data->dot;
    if (!current->next)
    {
        data->map->chunk[current->pos_x][current->pos_y].type = TYPE_EMPTY;
        pthread_join(current->thread, NULL);
        free(current);
        data->dot = NULL;
        return;
    }
    while(current->next->next)
        current = current->next;
    t_dot *to_delete = current->next;
    data->map->chunk[to_delete->pos_x][to_delete->pos_y].type = TYPE_EMPTY;
    pthread_join(to_delete->thread, NULL);
    free(to_delete);
    current->next = NULL;
}