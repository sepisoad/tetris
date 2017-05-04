#include "game.h"

// 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16
//{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

const unsigned char shapes[7][4][16] = {
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

//==============================================================================

const char *about_screen_text = "\
tetrisk version 1.0\n\
developed by `Sepehr Aryani`\n\
tetrisk is an Tetris(tm) clone\n\
\n\
this game is free software and\n\
the source code is available\n\
on github:\n\
github.com/sepisoad/tetrisk\n\
\n\
you can contact me on:\n\
twitter: @sepisoad\n\
github: @sepisoad\n\
email: sepehr.aryani@gmail.com\n\
website: sepisoad.com\n\
\n\
      press any key... ";

//==============================================================================

cell_k board[BOARD_COLS][BOARD_ROWS] = {CELL_MPT};

//==============================================================================