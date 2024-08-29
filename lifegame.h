#ifndef LIFEGAME_H
# define LIFEGAME_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <mlx.h>

struct s_data;

#define CHUNK_SIZE 15

typedef enum e_type {
    TYPE_EMPTY,
    TYPE_WALL,
    TYPE_DOT,
    TYPE_COLLECTIBLE,
} t_type;

typedef struct s_chunk {
    size_t pos_x;
    size_t pos_y;
    t_type type;
    pthread_mutex_t mutex;
} t_chunk;

typedef struct s_collectible {
    size_t pos_x;
    size_t pos_y;
    pthread_mutex_t mutex;
}   t_collectible;

typedef struct s_map {
    bool    refresh;
    
    t_chunk **chunk;
    t_collectible *collectible;
    struct s_sync *sync;
    
    pthread_mutex_t mutex;
} t_map;

typedef struct s_sync {
    size_t size_x;
    size_t size_y;
} t_sync;

typedef struct s_image {
    void *img;
    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
    struct s_image *next;
} t_image;


typedef struct s_dot {
    size_t pos_x;
    size_t pos_y;
    size_t pos;
    bool alive;

    struct s_map *map_ptr;
    pthread_mutex_t mutex;
    pthread_t thread;
    struct s_dot *next;
} t_dot;

typedef struct s_win {
    t_image *image;
    void *mlx_ptr;
    void *win_ptr;
} t_win;

typedef struct s_data {
    t_map *map;
    t_win *window;
    t_dot *dot;
    pthread_mutex_t dots_mutex;
    size_t nb_dot;
    t_sync *sync;
} t_data;

bool init_data(t_data **data, char **argv);
bool init_sync(t_data *data, char **argv);
bool init_map(t_data *data);
bool init_dot(t_data *data);
bool init_window(t_data *data);
void free_all(t_data *data);
void free_images(t_data *data);
void free_data(t_data *data);
void free_map(t_data *data);
void free_dots(t_data *data);
void gen_map(t_data *data, size_t ratio);
void res_map(t_data *data);
void add_dot(t_data *data);
void del_dot(t_data *data);
void print_term_chunk(t_chunk *chunk);
void print_term_map(t_data *data);
void refresh_map(t_data *data);
void *dot_life(void *in_data);
int key_event(int keycode, t_data *data);
void loop(t_data *data);
void    gen_collectible(t_data *data);

void set_bool(pthread_mutex_t *mutex, bool *dst, bool src);
bool get_bool(pthread_mutex_t *mutex, bool *src);
t_dot *get_dot(t_data *data, size_t pos);

#endif
