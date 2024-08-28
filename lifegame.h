
#ifndef LIFEGAME_H
# define LIFEGAME_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <mlx.h>

// =========================Data=============================

// Define the size of each chunk
#define CHUNK_SIZE 10

// Enum for different chunk types
typedef enum e_type {
    TYPE_EMPTY,
    TYPE_WALL,
    TYPE_DOT
} t_type;

// Struct for a single chunk in the map
typedef struct s_chunk {
    size_t pos_x;          // X position in the grid
    size_t pos_y;          // Y position in the grid
    t_type type;           // Type of the chunk (empty, wall, dot)
    pthread_mutex_t mutex; // Mutex for thread safety
} t_chunk;

// Struct for the map, containing a 2D array of chunks
typedef struct s_map {
    t_chunk **chunk;       // 2D array of chunks
    struct s_sync *sync;   // Synchronization data for the map
} t_map;

// Struct for synchronization data
typedef struct s_sync {
    size_t size_x;         // Width of the map (number of chunks in X direction)
    size_t size_y;         // Height of the map (number of chunks in Y direction)
} t_sync;

// Struct for an image in the graphics library
typedef struct s_image {
    void *img;             // Pointer to the image object
    char *addr;            // Pointer to image data
    int bits_per_pixel;    // Bits per pixel
    int line_length;       // Length of a line in bytes
    int endian;            // Endianness of the image
    struct s_image *next;  // Pointer to the next image in a list
} t_image;

// Struct for a dot in the game
typedef struct s_dot {
    size_t pos_x;          // X position of the dot
    size_t pos_y;          // Y position of the dot
    size_t pos;            // Position in a flat array or index
    bool alive;             // Whether the dot is alive

    struct s_map *map_ptr; // Pointer to the map containing the dot
    pthread_mutex_t mutex; // Mutex for thread safety
    pthread_t thread;      // Thread associated with the dot
    struct s_dot *next;    // Pointer to the next dot in the list
} t_dot;

// Struct for the window and graphics
typedef struct s_win {
    t_image *image;        // Pointer to image data
    void *mlx_ptr;         // Pointer to MLX instance
    void *win_ptr;         // Pointer to the window
} t_win;

// Struct for all main data
typedef struct s_data {
    t_map *map;            // Pointer to the map
    t_win *window;         // Pointer to window-related data
    t_dot *dot;            // Linked list of dots
    t_sync *sync;          // Synchronization data
} t_data;

// Function prototypes
bool init_data(t_data **data, char **argv);
bool init_sync(t_data *data, char **argv);
bool init_map(t_data *data);
bool init_dot(t_data *data);
bool init_window(t_data *data);
void    free_all(t_data *data);
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
void refresh_map(t_data *data); // Assuming you have this function
void *dot_life(void *in_data);
int key_event(int keycode, t_data *data);
void loop(t_data *data);

#endif