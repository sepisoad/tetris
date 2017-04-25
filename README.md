# tetris
a tetris clone written in C using SDL2

![alt text](screenshots/1.png "in game screenshot")![alt text](screenshots/2.png "in game screenshot")![alt text](screenshots/2.png "in game screenshot")

## Where to get the game
if you want you can download the pre-build and packaged version on itch.io
<iframe frameborder="0" src="https://itch.io/embed/138834" width="552" height="167"></iframe>


## How to build

### Prerequisite
in order to build the game you need to first download [premake4](https://premake.github.io/download.html#v4)
you also need to get [libSDL2](https://www.libsdl.org/), [libSDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/) and [SDL_mixer 2.0](https://www.libsdl.org/projects/SDL_mixer/)

### Build Instructions
run following commands in your shell:
```bash
premake4 gmake
```
```bash
make config=release
```
that's it.

### Extra notes:
I built and tested the game on Window 7(32 bit), Windows 10(64 bit) and Ubuntu GNU/Linux (32/64 bit).

The source code can also be easily built on OSX too though I have not tested it yet. 

BTW I used GCC on linux and MinGW on windows to compile the code.


### How to contact me:

twitter: [@sepisoad](https://twitter.com/sepisoad)

github: [@sepisoad](https://github.com/sepisoad)