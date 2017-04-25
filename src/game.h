#pragma once

#include <stdbool.h>
#include <stdint.h>

//==============================================================================

#define WINDOW_WIDTH 256
#define WINDOW_HEIGHT 240
#define SHAPE_COLS 4
#define SHAPE_ROWS 4
#define BOARD_COLS 12
#define BOARD_ROWS 20
#define BOARD_WIDTH_SP 50
#define BOARD_HEIGHT_SP 40
#define BOARD_WIDTH_EP 146
#define BOARD_HEIGHT_EP 200
#define BLOCK_SIZE 8
#define FPS 60
#define CELL_SCORE 3
#define SHAPE_SCORE (CELL_SCORE * 4)
#define ROW_SCORE (BOARD_COLS * CELL_SCORE)

#define INTRO_SHEET "data/intro.png"
#define INTRO_JSON "data/intro.json"
#define INTRO_OGG "data/intro.ogg"
#define MENUE_ITEM_OGG "data/menue_item.ogg"
#define MENUE_SELECT_OGG "data/menue-select.ogg"

//==============================================================================

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct _TTF_Font TTF_Font;

//==============================================================================

typedef enum cell_k cell_k;
enum cell_k
{
  CELL_MPT = 0, // empty cell
  CELL_TMP = 1, // temporary occupied cell by current shape
  CELL_OCP = 2, // fully occupied cell after setteling a shape
  // CELL_HDN = 3  // when a cell is out of board area and not visible
};

//==============================================================================

typedef enum shape_k shape_k;
enum shape_k
{
  SHAPE_O = 0,
  SHAPE_L = 1,
  SHAPE_J = 2,
  SHAPE_S = 3,
  SHAPE_Z = 4,
  SHAPE_T = 5,
  SHAPE_I = 6
};

//==============================================================================

typedef enum dir_k dir_k;
enum dir_k
{
  DIR_UP = 0,
  DIR_RIGHT = 1,
  DIR_DOWN = 2,
  DIR_LEFT = 3
};

//==============================================================================

typedef struct pos_t pos_t;
struct pos_t
{
  int col;
  int row;
};

//==============================================================================

typedef struct shape_t shape_t;
struct shape_t
{
  shape_k kind;
  dir_k dir;
  pos_t pos;
  bool can_occupy;
};

//==============================================================================

typedef struct engine_t engine_t;
struct engine_t
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
};

//==============================================================================

typedef struct menue_t menue_t;
struct menue_t
{
  char **items;
  uint8_t count;
  int8_t selected;
};

//==============================================================================

void engine_init(engine_t *engine);
void engine_uninit(engine_t *engine);
void intro_start(engine_t *engine);
bool main_menue_start(engine_t *engine);
bool about_screen_start(engine_t *engine);
void draw_main_menue(engine_t *engine, menue_t *menue);
void game_start(engine_t *engine);
void draw_board(SDL_Renderer *renderer);
void refresh_shape(shape_t *shape);
void refresh_board();
void occupy_board(shape_t *shape);
bool can_collide(shape_t *shape, bool *is_game_over);
void generate_shape(shape_t *shape);
void draw_score(SDL_Renderer *renderer, TTF_Font *font, int score);
bool fast_land_shape(shape_t *shape, bool *is_game_over);
int free_compelete_rows();
void shif_rows_down(int s_row);