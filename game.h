#include <iostream>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <string>
#include <fstream>

#define MAPSIZEX 500
#define MAPSIZEY 500

#define EMPTY 0
#define FREEBLOCK 1
#define SOLIDBLOCK 2
#define STAIRBLOCK 3

using namespace std;

struct POS{
    int X = -1;
    int Y = -1;
};

struct ATTRIBUTES{
    int strenght = 1;
    int defense = 1;
    int intelligence = 1;
    int dexterity = 1;
};

struct STATUS{
    int hp = 1;
    int mp = 1;
    int multiplier = 10;
};

struct PLAYER{
    POS pos;
    int attPoints = 5;
    int points = 0;
    int exp = 0;
    int keyInput = 0;
    STATUS status;
    ATTRIBUTES attributes;
};

struct MAP{
    int tiles[MAPSIZEY][MAPSIZEX];
    int memory[MAPSIZEY][MAPSIZEX];
    int level = 5;
};

struct MENU{
    int option = 0;
};

struct GAME{
    int exit = 0;
    int play = 1;
    int pause = 0;
    PLAYER player;
    MAP map;
    MENU menu;
};

void new_line(string x, string y, string z,int size){
    cout<<x;
    for(int i = 0; i<size; i++){
        cout<<y;
    }
    cout<<z<<endl;
}

int VA(int number){
    if(number<0){
        number*=-1;
    }
    return number;
}

int player_input(GAME &game){
    if(kbhit()){
        if(game.player.keyInput==0){
            game.player.keyInput = getch();
            if(game.player.keyInput==119){
                if(game.map.tiles[game.player.pos.Y-1][game.player.pos.X]==FREEBLOCK){
                    game.player.pos.Y--;
                }
            }
            if(game.player.keyInput==115){
                if(game.map.tiles[game.player.pos.Y+1][game.player.pos.X]==FREEBLOCK){
                    game.player.pos.Y++;
                }
            }
            if(game.player.keyInput==97){
                if(game.map.tiles[game.player.pos.Y][game.player.pos.X-1]==FREEBLOCK){
                    game.player.pos.X--;
                }
            }
            if(game.player.keyInput==100){
                if(game.map.tiles[game.player.pos.Y][game.player.pos.X+1]==FREEBLOCK){
                    game.player.pos.X++;
                }
            }
        }else{
            game.player.keyInput = getch();
        }
    }else{
        game.player.keyInput = 0;
    }
    return 0;
}

int simulate_vision(GAME &game,int y,int x,int i=0){
    if(i>=1){
        y += (y!=0) ? y/VA(y) : 0;
        x += (x!=0) ? x/VA(x) : 0;
    }
    if(game.map.tiles[game.player.pos.Y+y][game.player.pos.X+x]!=SOLIDBLOCK){
        i++;
        game.map.memory[game.player.pos.Y+y][game.player.pos.X+x] = 1;
        if(i<10){
            simulate_vision(game,y,x,i);
        }
    }
    game.map.memory[game.player.pos.Y+y][game.player.pos.X+x] = 1;
    return 0;
}

void create_map(GAME &game){
    for(int y=0;y<MAPSIZEY;y++){
        for(int x=0;x<MAPSIZEX;x++){
            game.map.tiles[y][x] = EMPTY;
            game.map.memory[y][x] = 0;
        }
    }
    int Y = MAPSIZEY/2;
    int X = MAPSIZEX/2;
    game.player.pos.Y = Y;
    game.player.pos.X = X;
    int i = 0;
    while(true){
        int tamY = rand()%5+2;
        int tamX = rand()%5+2;
        for(int y=-tamY;y<tamY;y++){
            for(int x=-tamX;x<tamX;x++){
                if(Y+y>=0 && Y+y<MAPSIZEY && X+x>=0 && X+x<MAPSIZEX){
                    if(game.map.tiles[Y+y][X+x]==EMPTY){
                        game.map.tiles[Y+y][X+x] = FREEBLOCK;
                        if(y==-tamY || y==tamY-1 || x==-tamX || x==tamX-1){
                            game.map.tiles[Y+y][X+x] = SOLIDBLOCK;
                        }
                    }
                } 
            }
        }
        if(i==game.map.level*2){
            game.map.tiles[Y][X] = STAIRBLOCK;
            break;
        }
        int direction = rand()%4;
        while(true){
            if(direction==0){
                Y--;
            }
            if(direction==1){
                Y++;
            }
            if(direction==2){
                X--;
            }
            if(direction==3){
                X++;
            }
            if(Y>=0 && Y<MAPSIZEY && X>=0 && X<MAPSIZEX){
                game.map.tiles[Y][X] = FREEBLOCK;
            }else{
                if(direction==0){
                    Y++;
                }
                if(direction==1){
                    Y--;
                }
                if(direction==2){
                    X++;
                }
                if(direction==3){
                    X--;
                }
            }
            if(rand()%100==0){
                break;
            }
            if(rand()%5==0){
                direction = rand()%4;
            }
        }
        i++;
    }
}

void render_map(GAME &game){
    cout << "\e[?25l\e[H";
    cout << "\e[1;1H";
    int vision = 10;
    for(int y=-1;y<=1;y++){
        for(int x=-1;x<=1;x++){
            simulate_vision(game,y,x);
        }
    }
    new_line("┏","━","┓",vision*2);
    for(int y=-vision/2;y<vision/2;y++){
        cout<<"┃";
        for(int x=-vision;x<vision;x++){
            if(game.map.memory[game.player.pos.Y+y][game.player.pos.X+x]==1){
                if(y==0 && x==0){
                    cout<<"\e[48;5;255m ";
                    continue;
                }
                if(game.map.tiles[game.player.pos.Y+y][game.player.pos.X+x]==EMPTY){
                    cout<<"\e[0m ";
                }
                if(game.map.tiles[game.player.pos.Y+y][game.player.pos.X+x]==FREEBLOCK){
                    cout<<"\e[48;5;246m ";
                }
                if(game.map.tiles[game.player.pos.Y+y][game.player.pos.X+x]==SOLIDBLOCK){
                    cout<<"\e[48;5;242m ";
                }
                if(game.map.tiles[game.player.pos.Y+y][game.player.pos.X+x]==STAIRBLOCK){
                    cout<<"\e[48;5;220m ";
                }
            }else{
                cout<<"\e[48;5;234m ";
            }
        }
        cout<<"\e[0m┃\n";
    }
    new_line("┗","━","┛",vision*2);
    int PosY;
    int PosX;
    bool success = true;
    for(int i=0;i<10;i++){
        PosY = rand()%MAPSIZEY;
        PosX = rand()%MAPSIZEX;
        if(game.map.memory[PosY][PosX]==1){
            break;
        }else{
            success = false;
        }
    }
    if(success){
        for(int y=-vision/2;y<vision/2;y++){
            for(int x=-vision;x<vision;x++){
                if(PosY == game.player.pos.Y+y || PosX == game.player.pos.X+x){
                    success = false;
                }
            }
        }
    }
    if(success){
        game.map.memory[PosY][PosX] = 0;
    }
}