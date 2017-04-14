// This file is part of titrees.
//
// titrees is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Foobar is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

// Author: Sepehr Aryani (sepisoad)
// Copyright: Sepehr Aryani © 2017
// Twitter: @sepisoad
// Github: @sepisoad
// Descriptin: titris is a clone of Tetris™ game

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#undef EXDEBUG

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

//==============================================================================

typedef enum cell_k cell_k;
enum cell_k
{
  CELL_MPT = 0, // empty cell
  CELL_TMP = 1, // temporary occupied cell by current shape
  CELL_OCP = 2, // fully occupied cell after setteling a shape
  // CELL_HDN = 3  // when a cell is out of board area and not visible
};

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

typedef enum dir_k dir_k;
enum dir_k
{
  DIR_UP = 0,
  DIR_RIGHT = 1,
  DIR_DOWN = 2,
  DIR_LEFT = 3
};

typedef struct pos_t pos_t;
struct pos_t
{
  int col;
  int row;
};

typedef struct shape_t shape_t;
struct shape_t
{
  shape_k kind;
  dir_k dir;
  pos_t pos;
  bool can_occupy;
};

//==============================================================================

// 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16
//{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
static unsigned char shapes[7][4][16] = {
    { //SHAPE_O
     {//DIR_UP
      0, 0, 0, 0,
      0, 1, 1, 0,
      0, 1, 1, 0,
      0, 0, 0, 0},
     {//DIR_RIGHT
      0, 0, 0, 0,
      0, 1, 1, 0,
      0, 1, 1, 0,
      0, 0, 0, 0},
     {//DIR_DOWN
      0, 0, 0, 0,
      0, 1, 1, 0,
      0, 1, 1, 0,
      0, 0, 0, 0},
     {//DIR_LEFT
      0, 0, 0, 0,
      0, 1, 1, 0,
      0, 1, 1, 0,
      0, 0, 0, 0}},
    { //SHAPE_L
     {//DIR_UP
      0, 1, 0, 0,
      0, 1, 0, 0,
      0, 1, 1, 0,
      0, 0, 0, 0},
     {//DIR_RIGHT
      0, 0, 0, 0,
      1, 1, 1, 0,
      1, 0, 0, 0,
      0, 0, 0, 0},
     {//DIR_DOWN
      0, 1, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 0, 0, 0},
     {//DIR_LEFT
      0, 0, 0, 1,
      0, 1, 1, 1,
      0, 0, 0, 0,
      0, 0, 0, 0}},
    { //SHAPE_J
     {//DIR_UP
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 1, 1, 0,
      0, 0, 0, 0},
     {//DIR_RIGHT
      0, 0, 0, 0,
      0, 1, 0, 0,
      0, 1, 1, 1,
      0, 0, 0, 0},
     {//DIR_DOWN
      0, 0, 0, 0,
      0, 1, 1, 0,
      0, 1, 0, 0,
      0, 1, 0, 0},
     {//DIR_LEFT
      0, 0, 0, 0,
      1, 1, 1, 0,
      0, 0, 1, 0,
      0, 0, 0, 0}},
    { //SHAPE_S
     {//DIR_UP
      0, 1, 0, 0,
      0, 1, 1, 0,
      0, 0, 1, 0,
      0, 0, 0, 0},
     {//DIR_RIGHT
      0, 0, 0, 0,
      0, 1, 1, 0,
      1, 1, 0, 0,
      0, 0, 0, 0},
     {//DIR_DOWN
      0, 0, 0, 0,
      0, 1, 0, 0,
      0, 1, 1, 0,
      0, 0, 1, 0},
     {//DIR_LEFT
      0, 0, 0, 0,
      0, 1, 1, 0,
      1, 1, 0, 0,
      0, 0, 0, 0}},
    { //SHAPE_Z
     {//DIR_UP
      0, 0, 1, 0,
      0, 1, 1, 0,
      0, 1, 0, 0,
      0, 0, 0, 0},
     {//DIR_RIGHT
      0, 0, 0, 0,
      1, 1, 0, 0,
      0, 1, 1, 0,
      0, 0, 0, 0},
     {//DIR_DOWN
      0, 0, 0, 0,
      0, 0, 1, 0,
      0, 1, 1, 0,
      0, 1, 0, 0},
     {//DIR_LEFT
      0, 0, 0, 0,
      1, 1, 0, 0,
      0, 1, 1, 0,
      0, 0, 0, 0}},
    { //SHAPE_T
     {//DIR_UP
      0, 0, 0, 0,
      0, 1, 0, 0,
      1, 1, 1, 0,
      0, 0, 0, 0},
     {//DIR_RIGHT
      0, 1, 0, 0,
      0, 1, 1, 0,
      0, 1, 0, 0,
      0, 0, 0, 0},
     {//DIR_DOWN
      0, 0, 0, 0,
      1, 1, 1, 0,
      0, 1, 0, 0,
      0, 0, 0, 0},
     {//DIR_LEFT
      0, 0, 1, 0,
      0, 1, 1, 0,
      0, 0, 1, 0,
      0, 0, 0, 0}},
    { //SHAPE_I
     {//DIR_UP
      0, 1, 0, 0,
      0, 1, 0, 0,
      0, 1, 0, 0,
      0, 1, 0, 0},
     {//DIR_RIGHT
      0, 0, 0, 0,
      1, 1, 1, 1,
      0, 0, 0, 0,
      0, 0, 0, 0},
     {//DIR_DOWN
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0},
     {//DIR_LEFT
      0, 0, 0, 0,
      0, 0, 0, 0,
      1, 1, 1, 1,
      0, 0, 0, 0}},
};

static cell_k board[BOARD_COLS][BOARD_ROWS] = {CELL_MPT};

//==============================================================================

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

//==============================================================================

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  TTF_Font *font = TTF_OpenFont("data/font.ttf", 16);
  window = SDL_CreateWindow("tets",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            WINDOW_WIDTH,
                            WINDOW_HEIGHT,
                            SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window,
                                -1,
                                SDL_RENDERER_ACCELERATED |
                                    SDL_RENDERER_PRESENTVSYNC);

  bool can_quit = false;
  bool is_paused = false;
  bool is_game_over = false;
  SDL_Event event;
  long frame_duration = 1000 / FPS;
  int score = 0;
  int freed_count;
  int old_tick = SDL_GetTicks();
  int new_tick = old_tick;
  int dur_tick = 0;
  int sec_tick = 0;
  shape_t shape;
  generate_shape(&shape);

reset_point:
  while (!can_quit && !is_game_over)
  {
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
          can_quit = true;
          continue;
        }
        if (event.key.keysym.sym == SDLK_RIGHT)
        {
          shape.pos.col++;
          if (can_collide(&shape, &is_game_over))
          {
            shape.pos.col--;
          }
        }
        if (event.key.keysym.sym == SDLK_LEFT)
        {
          shape.pos.col--;
          if (can_collide(&shape, &is_game_over))
          {
            shape.pos.col++;
          }
        }
        if (event.key.keysym.sym == SDLK_DOWN)
        {
          shape.pos.row++;
          if (can_collide(&shape, &is_game_over))
          {
            shape.pos.row--;
            shape.can_occupy = true;
          }
        }
        if (event.key.keysym.sym == SDLK_UP)
        {
          dir_k old_dir = shape.dir;

          shape.dir += 1;
          if (shape.dir > DIR_LEFT)
          {
            shape.dir = DIR_UP;
          }

          if (can_collide(&shape, &is_game_over))
          {
            shape.dir = old_dir;
          }
        }
        if (event.key.keysym.sym == SDLK_RETURN)
        {
          if (fast_land_shape(&shape, &is_game_over))
          {
            shape.pos.row--;
            shape.can_occupy = true;
          }

          // occupy_board(&shape);
          // score += SHAPE_SCORE;
        }
        if (event.key.keysym.sym == SDLK_p)
        {
          is_paused = !is_paused;
        }
      }
    }

    if (!is_paused)
    {
      new_tick = SDL_GetTicks();
      dur_tick = new_tick - old_tick;

      if (dur_tick < frame_duration)
      {
        continue;
      }
      old_tick = new_tick;
      sec_tick += 1;
      if (sec_tick >= 50)
      {
        sec_tick = 0;
        shape.pos.row++;
        if (can_collide(&shape, &is_game_over))
        {
          shape.pos.row--;
          shape.can_occupy = true;
        }
      }

      if (shape.can_occupy)
      {
        occupy_board(&shape);
        generate_shape(&shape);
        score += SHAPE_SCORE;
      }
      SDL_SetRenderDrawColor(renderer, 50, 40, 60, SDL_ALPHA_OPAQUE);
      SDL_RenderClear(renderer);

      refresh_shape(&shape);
      draw_board(renderer);
      refresh_board();
      freed_count = free_compelete_rows();
      if (freed_count)
      {
        score += freed_count * ROW_SCORE;
      }
      draw_score(renderer, font, score);
    }

    SDL_RenderPresent(renderer);
  }

  if (is_game_over)
  {
    can_quit = false;
    is_paused = false;
    is_game_over = false;
    score = 0;
    freed_count = 0;
    old_tick = SDL_GetTicks();
    new_tick = old_tick;
    dur_tick = 0;
    sec_tick = 0;
    generate_shape(&shape);
    memset(board, CELL_MPT, sizeof(board));
    goto reset_point;
  }

  //clean up
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();

  return 0;
}

//==============================================================================

void draw_board(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 155, 155, 155, SDL_ALPHA_OPAQUE);

  SDL_Rect rect = {BOARD_WIDTH_SP, BOARD_HEIGHT_SP, BLOCK_SIZE, BLOCK_SIZE};
  for (int col = 0; col < BOARD_COLS; col++)
  {
    for (int row = 0; row < BOARD_ROWS; row++)
    {
      if (board[col][row] == CELL_OCP)
      {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
      }
      else if (board[col][row] == CELL_TMP)
      {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &rect);
      }
      else
      {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &rect);
      }

      rect.y += BLOCK_SIZE;
    }
    rect.x += BLOCK_SIZE;
    rect.y = BOARD_HEIGHT_SP;
  }
}

//==============================================================================

void refresh_shape(shape_t *shape)
{
  for (int idx = 0, i = 0, j = 0; idx < 16; idx++)
  {
    if (shape->pos.row + j >= 0)
    {
      if (shapes[shape->kind][shape->dir][idx])
      {
        if (board[i + shape->pos.col][j + shape->pos.row] != CELL_OCP)
        {
          board[i + shape->pos.col][j + shape->pos.row] = CELL_TMP;
        }
      }
    }

    i++;
    if ((idx + 1) % 4 == 0)
    {
      j++;
      i = 0;
    }
  }
}

//==============================================================================

void refresh_board()
{
  for (int col = 0; col < BOARD_COLS; col++)
  {
    for (int row = 0; row < BOARD_ROWS; row++)
    {
      if (board[col][row] == CELL_TMP)
      {
        board[col][row] = CELL_MPT;
      }
    }
  }
}

//==============================================================================

void occupy_board(shape_t *shape)
{
  for (int idx = 0, i = 0, j = 0; idx < 16; idx++)
  {
    if (shapes[shape->kind][shape->dir][idx])
    {
      board[i + shape->pos.col][j + shape->pos.row] = CELL_OCP;
    }

    i++;
    if ((idx + 1) % 4 == 0)
    {
      j++;
      i = 0;
    }
  }
}

//==============================================================================

bool can_collide(shape_t *shape, bool *is_game_over)
{
  for (int idx = 0, i = 0, j = 0; idx < 16; idx++)
  {
    if (shapes[shape->kind][shape->dir][idx])
    {
      int col = i + shape->pos.col;
      int row = j + shape->pos.row;

      if (row == 0 && board[col][row] == CELL_OCP)
      {
        *is_game_over = true;
        return true;
      }

      if (row >= BOARD_ROWS)
      {
        shape->can_occupy = true;
        return true;
      }

      if ((col < 0) || (col >= BOARD_COLS))
      {
        return true;
      }

      if (col >= 0 && row >= 0)
      {
        if (board[col][row] == CELL_OCP)
        {
          return true;
        }
      }
    }

    i++;
    if ((idx + 1) % 4 == 0)
    {
      j++;
      i = 0;
    }
  }

  return false;
}

//==============================================================================

void generate_shape(shape_t *shape)
{
  time_t t;
  srand((unsigned int)time(&t));
  shape->kind = rand() % (SHAPE_I + 1);
  shape->dir = DIR_UP;
  shape->pos.col = 4;
  shape->pos.row = -4;
  shape->can_occupy = false;
}

//==============================================================================

void draw_score(SDL_Renderer *renderer, TTF_Font *font, int score)
{
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
  char score_str[64] = {0};
  sprintf(score_str, "score: %d", score);

  int w, h;
  SDL_Color fg = {255, 255, 255, 255};
  SDL_Surface *srfc_text_score = TTF_RenderText_Solid(font, score_str, fg);
  TTF_SizeText(font, score_str, &w, &h);
  SDL_Rect rect_text_score = {10, 10, w, h};
  SDL_Texture *txtr_text_score = SDL_CreateTextureFromSurface(renderer,
                                                              srfc_text_score);
  SDL_FreeSurface(srfc_text_score);
  SDL_RenderCopy(renderer, txtr_text_score, NULL, &rect_text_score);
  SDL_DestroyTexture(txtr_text_score);
}

//==============================================================================

int free_compelete_rows()
{
  int occupied_cells = 0;
  int freed_rows = 0;
  for (int row = 0; row < BOARD_ROWS; row++)
  {
    for (int col = 0; col < BOARD_COLS; col++)
    {
      if (board[col][row] == CELL_OCP)
      {
        occupied_cells++;
        if (occupied_cells >= BOARD_COLS)
        {
          shif_rows_down(row);
          freed_rows++;
          break;
        }
      }
    }
    occupied_cells = 0;
  }

  return freed_rows;
}

//==============================================================================

bool fast_land_shape(shape_t *shape, bool *is_game_over)
{
  for (int row = shape->pos.row; row < BOARD_ROWS; row++)
  {
    shape->pos.row++;
    if (can_collide(shape, is_game_over))
    {
      return true;
    }
  }

  return false;
}

//==============================================================================

void shif_rows_down(int s_row)
{
  for (int row = s_row; row >= 1; row--)
  {
    for (int col = 0; col < BOARD_COLS; col++)
    {
      board[col][row] = board[col][row - 1];
    }
  }
}

//==============================================================================
