#include "lifegame.h"

bool get_bool(pthread_mutex_t *mutex, bool *src)
{
    bool ret = false;

    pthread_mutex_lock(mutex);
    ret = *src;
    pthread_mutex_unlock(mutex);
    return ret;
}


void    set_bool(pthread_mutex_t *mutex, bool *dst, bool src)
{
    pthread_mutex_lock(mutex);
    *dst = src;
    pthread_mutex_unlock(mutex);
}