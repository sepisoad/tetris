#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
// #include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "lib/parson/parson.h"
#include "game.h"

//==============================================================================

extern const unsigned char shapes[7][4][16];
extern const char *about_screen_text;
extern cell_k board[BOARD_COLS][BOARD_ROWS];

//==============================================================================

static Mix_Music *music_intro = NULL;
static Mix_Music *music_menue = NULL;
static Mix_Music *music_one = NULL;
static Mix_Chunk *sfx_item = NULL;
static Mix_Chunk *sfx_select = NULL;
static bool can_show_grids = true;
static bool is_fullscreen = false;
static bool mute_music = false;
static bool mute_sfx = false;

//==============================================================================

void engine_init(engine_t *engine)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  TTF_Init();

  engine->window = SDL_CreateWindow("tets",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    WINDOW_WIDTH,
                                    WINDOW_HEIGHT,
                                    SDL_WINDOW_OPENGL);

  engine->renderer = SDL_CreateRenderer(
      engine->window,
      -1,
      SDL_RENDERER_ACCELERATED |
          SDL_RENDERER_PRESENTVSYNC);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_RenderSetLogicalSize(engine->renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

  Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 512);

  engine->font = TTF_OpenFont("data/font.ttf", 16);

  music_intro = Mix_LoadMUS(INTRO_OGG);
  music_one = Mix_LoadMUS(MUSIC_ONE_OGG);
  music_menue = Mix_LoadMUS(MUSIC_MENUE_OGG);
  sfx_item = Mix_LoadWAV(MENUE_ITEM_OGG);
  sfx_select = Mix_LoadWAV(MENUE_SELECT_OGG);
}

//==============================================================================

void engine_uninit(engine_t *engine)
{
  Mix_FreeMusic(music_intro);
  Mix_FreeMusic(music_one);
  Mix_FreeMusic(music_menue);
  Mix_FreeChunk(sfx_item);
  Mix_FreeChunk(sfx_select);
  SDL_DestroyRenderer(engine->renderer);
  SDL_DestroyWindow(engine->window);
  TTF_CloseFont(engine->font);
  Mix_Quit();
  TTF_Quit();
  SDL_Quit();
}

//==============================================================================

void intro_start(engine_t *engine)
{
  SDL_Surface *surface;
  SDL_Texture *texture;

  surface = IMG_Load(INTRO_SHEET);
  if (!surface)
  {
    return;
  }

  texture = SDL_CreateTextureFromSurface(engine->renderer, surface);
  if (!texture)
  {
    SDL_FreeSurface(surface);
    return;
  }
  SDL_FreeSurface(surface);

  JSON_Value *jsv_root = json_parse_file(INTRO_JSON);
  JSON_Object *jso_root = json_value_get_object(jsv_root);
  JSON_Array *jsa_frames = json_object_get_array(jso_root, "frames");

  int frames_count = (int)json_array_get_count(jsa_frames);

  SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(engine->renderer);
  SDL_RenderPresent(engine->renderer);
  Mix_PlayMusic(music_intro, 0);

  for (int idx = 0; idx < frames_count; idx++)
  {
    JSON_Object *jso_frame_rec = json_array_get_object(jsa_frames, idx);
    JSON_Object *jso_frame = json_object_get_object(jso_frame_rec, "frame");
    SDL_Rect rect = {
        (int)json_object_get_number(jso_frame, "x"),
        (int)json_object_get_number(jso_frame, "y"),
        (int)json_object_get_number(jso_frame, "w"),
        (int)json_object_get_number(jso_frame, "h")};

    SDL_Delay(35);

    SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(engine->renderer);
    SDL_RenderCopy(engine->renderer, texture, &rect, NULL);
    SDL_RenderPresent(engine->renderer);
  }

  while (Mix_PlayingMusic())
    ;

  //
  if (jsv_root)
  {
    json_value_free(jsv_root);
    jsv_root = NULL;
  }
}

//==============================================================================

bool main_menue_start(engine_t *engine)
{
  SDL_Event event;
  bool can_quit = false;
  menue_t menue;

  char **items = (char **)calloc(4, sizeof(char **));
  items[0] = (char *)calloc(32, sizeof(char *));
  items[1] = (char *)calloc(32, sizeof(char *));
  items[2] = (char *)calloc(32, sizeof(char *));
  items[3] = (char *)calloc(32, sizeof(char *));
  strcpy(items[0], "start");
  strcpy(items[1], "options");
  strcpy(items[2], "about");
  strcpy(items[3], "exit");

  menue.items = items;
  menue.count = 4;
  menue.selected = 0;

  if (!mute_music)
  {
    Mix_PlayMusic(music_menue, -1);
  }

  while (!can_quit)
  {
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.sym == SDLK_DOWN)
        {
          if (!mute_sfx)
          {
            Mix_PlayChannel(-1, sfx_item, 0);
          }

          menue.selected++;
          if (menue.selected >= menue.count)
          {
            menue.selected = 0;
          }
        }
        if (event.key.keysym.sym == SDLK_UP)
        {
          if (!mute_sfx)
          {
            Mix_PlayChannel(-1, sfx_item, 0);
          }

          menue.selected--;
          if (menue.selected < 0)
          {
            menue.selected = menue.count - 1;
          }
        }
        if (event.key.keysym.sym == SDLK_RETURN)
        {
          if (!mute_sfx)
          {
            Mix_PlayChannel(-1, sfx_select, 0);
          }

          if (menue.selected == 0)
          {
            can_quit = true;
            Mix_HaltMusic();
          }
          else if (menue.selected == 1)
          {
            options_menue_start(engine);
          }
          else if (menue.selected == 2)
          {
            about_screen_start(engine);
          }
          else if (menue.selected == menue.count - 1)
          {
            Mix_HaltMusic();
            SDL_Delay(500);
            return false;
          }
        }
      }
    }

    draw_main_menue(engine, &menue);
    SDL_RenderPresent(engine->renderer);
  }

  free(items[0]);
  free(items[1]);
  free(items[2]);
  free(items[3]);
  free(items);
  return true;
}

//==============================================================================

void draw_main_menue(engine_t *engine, menue_t *menue)
{
  SDL_SetRenderDrawColor(engine->renderer, 10, 10, 10, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(engine->renderer);

  int x = 100, y = 140, w, h;
  SDL_Color selected_fg = {255, 255, 255, 255};
  SDL_Color not_selected_fg = {150, 50, 50, 255};
  SDL_Color *ptr_color = &not_selected_fg;

  for (uint8_t idx = 0; idx < menue->count; idx++)
  {
    if (idx == menue->selected)
    {
      ptr_color = &selected_fg;
    }
    else
    {
      ptr_color = &not_selected_fg;
    }

    SDL_Surface *surface = TTF_RenderText_Solid(
        engine->font,
        menue->items[idx],
        *ptr_color);
    TTF_SizeText(engine->font, menue->items[idx], &w, &h);
    SDL_Rect rect = {x, y, w, h};
    SDL_Texture *texture = SDL_CreateTextureFromSurface(
        engine->renderer,
        surface);
    SDL_FreeSurface(surface);
    SDL_RenderCopy(engine->renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    y += 10;
  }
}

//==============================================================================

bool about_screen_start(engine_t *engine)
{
  bool can_quit = false;
  SDL_Event event;

  SDL_Color color = {150, 50, 50, 255};
  SDL_Rect text_rect = {10, 10, 0, 0};
  int w, h = 0;
  int len = strlen(about_screen_text);
  int idx = 0;
  char *string = (char *)calloc(len, sizeof(char));
  char character[2] = {0};

  SDL_Texture *render_texture = SDL_CreateTexture(
      engine->renderer,
      SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_TARGET,
      WINDOW_WIDTH,
      WINDOW_HEIGHT);

  SDL_SetRenderTarget(engine->renderer, render_texture);
  SDL_SetRenderDrawColor(engine->renderer, 10, 10, 10, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(engine->renderer);
  SDL_SetRenderTarget(engine->renderer, NULL);

  while (!can_quit)
  {
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_KEYDOWN)
      {
        can_quit = true;
      }
    }

    SDL_Delay(50);

    if (idx < len)
    {
      character[0] = about_screen_text[idx];
      if (about_screen_text[idx] == '\n')
      {
        SDL_Delay(200);
        text_rect.y += h;
        text_rect.x = 10;
        idx++;
        continue;
      }
      memset(string, 0, len);
      strncpy(string, about_screen_text, idx + 1);

      if (about_screen_text[idx] != ' ')
      {
        if (!mute_sfx)
        {
          Mix_PlayChannel(-1, sfx_select, 0);
        }
      }

      SDL_Surface *surface = TTF_RenderText_Solid(
          engine->font,
          character,
          color);
      TTF_SizeText(engine->font, character, &w, &h);

      text_rect.w = w;
      text_rect.h = h;

      SDL_Texture *texture = SDL_CreateTextureFromSurface(
          engine->renderer,
          surface);
      SDL_FreeSurface(surface);

      SDL_SetRenderTarget(engine->renderer, render_texture);
      SDL_RenderCopy(engine->renderer, texture, NULL, &text_rect);
      SDL_SetRenderTarget(engine->renderer, NULL);
      SDL_RenderCopy(engine->renderer, render_texture, NULL, NULL);
      SDL_DestroyTexture(texture);

      if (text_rect.x < WINDOW_WIDTH)
      {
        text_rect.x += w;
      }
    }
    idx++;

    SDL_RenderPresent(engine->renderer);
  }

  SDL_SetRenderTarget(engine->renderer, render_texture);
  SDL_SetRenderDrawColor(engine->renderer, 10, 10, 10, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(engine->renderer);
  SDL_SetRenderTarget(engine->renderer, NULL);
  SDL_RenderPresent(engine->renderer);
  SDL_DestroyTexture(render_texture);
  if (string)
  {
    free(string);
  }
  return true;
}

//==============================================================================

bool options_menue_start(engine_t *engine)
{
  SDL_Event event;
  bool can_quit = false;
  menue_t menue;

  char **items = (char **)calloc(5, sizeof(char **));
  items[0] = (char *)calloc(64, sizeof(char *));
  items[1] = (char *)calloc(64, sizeof(char *));
  items[2] = (char *)calloc(64, sizeof(char *));
  items[3] = (char *)calloc(64, sizeof(char *));
  items[4] = (char *)calloc(64, sizeof(char *));

  if (can_show_grids)
  {
    strcpy(items[0], "draw grids: yes");
  }
  else
  {
    strcpy(items[0], "draw grids: no");
  }

  if (is_fullscreen)
  {
    strcpy(items[1], "fullscreen: yes");
  }
  else
  {
    strcpy(items[1], "fullscreen: no");
  }

  if (mute_music)
  {
    strcpy(items[2], "mute music: yes");
  }
  else
  {
    strcpy(items[2], "mute music: no");
  }

  if (mute_sfx)
  {
    strcpy(items[3], "mute sfx: yes");
  }
  else
  {
    strcpy(items[3], "mute sfx: no");
  }

  strcpy(items[4], "back");

  menue.items = items;
  menue.count = 5;
  menue.selected = 0;

  while (!can_quit)
  {
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.sym == SDLK_DOWN)
        {
          if (!mute_sfx)
          {
            Mix_PlayChannel(-1, sfx_item, 0);
          }

          menue.selected++;
          if (menue.selected >= menue.count)
          {
            menue.selected = 0;
          }
        }
        if (event.key.keysym.sym == SDLK_UP)
        {
          if (!mute_sfx)
          {
            Mix_PlayChannel(-1, sfx_item, 0);
          }
          menue.selected--;
          if (menue.selected < 0)
          {
            menue.selected = menue.count - 1;
          }
        }
        if (event.key.keysym.sym == SDLK_RETURN)
        {
          if (!mute_sfx)
          {
            Mix_PlayChannel(-1, sfx_select, 0);
          }
          if (menue.selected == 0)
          {
            can_show_grids = !can_show_grids;
            if (can_show_grids)
            {
              strcpy(items[0], "draw grids: yes");
            }
            else
            {
              strcpy(items[0], "draw grids: no");
            }
          }
          else if (menue.selected == 1)
          {
            is_fullscreen = !is_fullscreen;
            if (is_fullscreen)
            {
              strcpy(items[1], "fullscreen: yes");
              SDL_SetWindowFullscreen(engine->window, SDL_WINDOW_FULLSCREEN);
            }
            else
            {
              strcpy(items[1], "fullscreen: no");
              SDL_SetWindowFullscreen(engine->window, 0);
            }
          }
          else if (menue.selected == 2)
          {
            mute_music = !mute_music;
            if (mute_music)
            {
              strcpy(items[2], "mute music: yes");
              Mix_HaltMusic();
            }
            else
            {
              strcpy(items[2], "mute music: no");
              Mix_PlayMusic(music_menue, -1);
            }
          }
          else if (menue.selected == 3)
          {
            mute_sfx = !mute_sfx;
            if (mute_sfx)
            {
              strcpy(items[3], "mute sfx: yes");
            }
            else
            {
              strcpy(items[3], "mute sfx: no");
            }
          }
          else if (menue.selected == 4)
          {
            can_quit = true;
          }
        }
      }
    }

    draw_options_menue(engine, &menue);
    SDL_RenderPresent(engine->renderer);
  }

  free(items[0]);
  free(items[1]);
  free(items[2]);
  free(items[3]);
  free(items[4]);
  free(items);

  return true;
}

//==============================================================================
void draw_options_menue(engine_t *engine, menue_t *menue)
{
  SDL_SetRenderDrawColor(engine->renderer, 10, 10, 10, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(engine->renderer);

  int x = 70, y = 120, w, h;
  SDL_Color selected_fg = {255, 255, 255, 255};
  SDL_Color not_selected_fg = {150, 50, 50, 255};
  SDL_Color *ptr_color = &not_selected_fg;

  for (uint8_t idx = 0; idx < menue->count; idx++)
  {
    if (idx == menue->selected)
    {
      ptr_color = &selected_fg;
    }
    else
    {
      ptr_color = &not_selected_fg;
    }

    SDL_Surface *surface = TTF_RenderText_Solid(
        engine->font,
        menue->items[idx],
        *ptr_color);
    TTF_SizeText(engine->font, menue->items[idx], &w, &h);
    SDL_Rect rect = {x, y, w, h};
    SDL_Texture *texture = SDL_CreateTextureFromSurface(
        engine->renderer,
        surface);
    SDL_FreeSurface(surface);
    SDL_RenderCopy(engine->renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    y += 10;
  }
}

//==============================================================================

void game_start(engine_t *engine)
{
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
  Mix_Chunk *sfx_shoot = Mix_LoadWAV(OBJECT_SHOOT_OGG);
  Mix_Chunk *sfx_move = Mix_LoadWAV(OBJECT_MOVE_OGG);
  Mix_Chunk *sfx_rotate = Mix_LoadWAV(OBJECT_ROTATE_OGG);

  shape_t shape;
  generate_shape(&shape);

  memset(board, 0, sizeof(board));

  if (!mute_music)
  {
    Mix_PlayMusic(music_one, -1);
  }

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
          if (!mute_sfx)
          {
            Mix_PlayChannel(-1, sfx_move, 0);
          }

          shape.pos.col++;
          if (can_collide(&shape, &is_game_over))
          {
            shape.pos.col--;
          }
        }
        if (event.key.keysym.sym == SDLK_LEFT)
        {
          if (!mute_sfx)
          {
            Mix_PlayChannel(-1, sfx_move, 0);
          }

          shape.pos.col--;
          if (can_collide(&shape, &is_game_over))
          {
            shape.pos.col++;
          }
        }
        if (event.key.keysym.sym == SDLK_DOWN)
        {
          if (!mute_sfx)
          {
            Mix_PlayChannel(-1, sfx_move, 0);
          }

          shape.pos.row++;
          if (can_collide(&shape, &is_game_over))
          {
            shape.pos.row--;
            shape.can_occupy = true;
          }
        }
        if (event.key.keysym.sym == SDLK_UP)
        {
          if (!mute_sfx)
          {
            Mix_PlayChannel(-1, sfx_rotate, 0);
          }

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
          if (!mute_sfx)
          {
            Mix_PlayChannel(-1, sfx_shoot, 0);
          }

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
      SDL_SetRenderDrawColor(engine->renderer, 50, 40, 60, SDL_ALPHA_OPAQUE);
      SDL_RenderClear(engine->renderer);

      refresh_shape(&shape);
      draw_board(engine->renderer);
      refresh_board();
      freed_count = free_compelete_rows();
      if (freed_count)
      {
        score += freed_count * ROW_SCORE;
      }
      draw_score(engine->renderer, engine->font, score);
    }

    SDL_RenderPresent(engine->renderer);
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

  Mix_HaltMusic();

  Mix_FreeChunk(sfx_shoot);
  Mix_FreeChunk(sfx_move);
  Mix_FreeChunk(sfx_rotate);
}

//==============================================================================

void draw_board(SDL_Renderer *renderer)
{
  SDL_Rect rect_big = {
      BOARD_WIDTH_SP,
      BOARD_HEIGHT_SP,
      BLOCK_SIZE * BOARD_COLS,
      BLOCK_SIZE * BOARD_ROWS};
  SDL_Rect rect = {
      BOARD_WIDTH_SP,
      BOARD_HEIGHT_SP,
      BLOCK_SIZE,
      BLOCK_SIZE};

  if (can_show_grids)
  {
    SDL_SetRenderDrawColor(renderer, 155, 155, 155, SDL_ALPHA_OPAQUE);
    for (int col = 0, step = 0; col < BOARD_COLS; col++, step += BLOCK_SIZE)
    {
      SDL_RenderDrawLine(
          renderer,
          BOARD_WIDTH_SP + step,
          BOARD_HEIGHT_SP,
          BOARD_WIDTH_SP + step,
          BOARD_HEIGHT_EP - 1);
    }
    for (int row = 0, step = 0; row < BOARD_ROWS; row++, step += BLOCK_SIZE)
    {
      SDL_RenderDrawLine(
          renderer,
          BOARD_WIDTH_SP,
          BOARD_HEIGHT_SP + step,
          BOARD_WIDTH_EP - 1,
          BOARD_HEIGHT_SP + step);
    }
  }

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawRect(renderer, &rect_big);

  for (int col = 0; col < BOARD_COLS; col++)
  {
    for (int row = 0; row < BOARD_ROWS; row++)
    {
      if (board[col][row] == CELL_OCP)
      {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
      }
      else if (board[col][row] == CELL_TMP)
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