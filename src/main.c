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
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"

//==============================================================================

int main(int argc, char **argv)
{
  engine_t engine;

  (void)argc;
  (void)argv;

  engine_init(&engine);
  intro_start(&engine);
  while (main_menue_start(&engine))
  {
    game_start(&engine);
  }
  engine_uninit(&engine);

  return EXIT_SUCCESS;
}
