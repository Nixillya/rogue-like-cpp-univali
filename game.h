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
    int strength = 1;
    int intelligence = 1;
    int dexterity = 1;
};

struct STATUS{
    int hp = 1;
    int hpMax = 1;
    int defense = 1;
};

struct ITEM{
    int id=0;
    int heal=0;
};

struct PLAYER{
    int attPoints = 1;
    int nivel = 1;
    int points = 0;
    int exp = 0;
    int keyInput = 0;
    int clockSpeed = clock();
    bool inventoryOpened = false;
    ITEM inventory[4][3];
    POS pos;
    STATUS status;
    ATTRIBUTES attributes;
};

struct MONSTER{
    int id = 0;
    bool alive = false;
    int clockSpeed = clock();
    POS pos;
    STATUS status;
    ATTRIBUTES attributes;
};

struct MAP{
    int tiles[MAPSIZEY][MAPSIZEX];
    int memory[MAPSIZEY][MAPSIZEX];
    int floor = 1;
};

struct MENU{
    int optionVertical = 4;
    int optionHorizontal = 0;
};

struct GAME{
    int monsterQuantity = 50;
    bool exit = false;
    bool play = false;
    bool pause = false;
    bool next = false;
    bool codex = false;
    PLAYER player;
    MONSTER monsters[50];
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

void menu_render(GAME &game){
    cout << "\e[?25l\e[H";
    new_line("┏","━","┓",16);
    new_line("┃   ","ROGUE-LIKE","   ┃",1);
    new_line("┣","━","┫",16);
    if (game.menu.optionVertical == 4) {
        cout << "┃ > [JOGAR]      ┃\n";
    } else {
        cout << "┃ [JOGAR]        ┃\n";
    }
    if (game.menu.optionVertical == 5) {
        cout << "┃ > [CODEX]      ┃\n";
    } else { 
        cout << "┃ [CODEX]        ┃\n";
    }
    if (game.menu.optionVertical == 6) {
        cout << "┃ > [CRÉDITOS]   ┃\n";
    } else {
        cout << "┃ [CRÉDITOS]     ┃\n";
    }
    if (game.menu.optionVertical == 7) {
        cout << "┃ > [SAIR]       ┃\n";
    } else {
        cout << "┃ [SAIR]         ┃\n";
    }
    new_line("┗","━","┛",16);

    int key = getch();
    switch(key){
        case 119: // Ir para cima
            game.menu.optionVertical--;
            if (game.menu.optionVertical < 4) {
                game.menu.optionVertical = 7;
            }
            break;
        case 115: // Ir para baixo
                game.menu.optionVertical++;
                if (game.menu.optionVertical > 7) {
                    game.menu.optionVertical = 4;
                }
                break;
        case 13: // Input (ENTER)
            if (game.menu.optionVertical == 4) {
                cout << "\ec";
                game.play = true;
            }
            if (game.menu.optionVertical == 5) {
                game.codex = true;
                cout << "\ec";
            }
            if (game.menu.optionVertical == 6) {
                cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓";
                cout << "\n┃       Universidade do Vale do Itajaí - UNIVALI     ┃";
                cout << "\n┃              Escola Politícnica  - POLI            ┃";
                cout << "\n┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                cout << "\n┃ Curso: Ciência da Computação                       ┃";
                cout << "\n┃ Disciplina: Algoritmos e Programação II            ┃";
                cout << "\n┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                cout << "\n┃ Equipe de Desenvolvimento:                         ┃";
                cout << "\n┃  Alunos:                                           ┃";
                cout << "\n┃   - Adolfo Schaeffer.                              ┃";
                cout << "\n┃   - Douglas Vilar.                                 ┃";
                cout << "\n┃   - Gustavo Alves.                                 ┃";
                cout << "\n┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                cout << "\n┃ Professor: Alex Rese.                              ┃";
                cout << "\n┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                cout << "\n┃ Local/Data da produção: Itajaí-SC, Junho/2026.     ┃";
                cout << "\n┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛";
                getch();
                cout << "\ec";
            }
            if (game.menu.optionVertical == 7) {
                game.exit = true;
            }
            break;
    }
}

void codex_render(GAME &game){
    cout << "\e[?25l\e[H";
    new_line("┏","━","┓",15);
    new_line("┃     ","CODEX","     ┃",1);
    new_line("┣","━","┫",15);
    if (game.menu.optionVertical == 4) {
        cout << "┃ > [COMO JOGAR]┃\n";
    } else {
        cout << "┃ [COMO JOGAR]  ┃\n";
    }
    if (game.menu.optionVertical == 5) {
        cout << "┃ > [ITENS]     ┃\n";
    } else { 
        cout << "┃ [ITENS]       ┃\n";
    }
    if (game.menu.optionVertical == 6) {
        cout << "┃ > [INIMIGOS]  ┃\n";
    } else {
        cout << "┃ [INIMIGOS]    ┃\n";
    }
    if (game.menu.optionVertical == 7) {
        cout << "┃ > [VOLTAR]    ┃\n";
    } else {
        cout << "┃ [VOLTAR]      ┃\n";
    }
    new_line("┗","━","┛",15);

    int key = getch();
    switch(key){
        case 119: // Ir para cima
            game.menu.optionVertical--;
            if (game.menu.optionVertical < 4) {
                game.menu.optionVertical = 7;
            }
            break;
        case 115: // Ir para baixo
                game.menu.optionVertical++;
                if (game.menu.optionVertical > 7) {
                    game.menu.optionVertical = 4;
                }
                break;
        case 13: // Input (ENTER)
            if (game.menu.optionVertical == 4) {
                cout << "\nTemplate [COMO JOGAR]";
                getch();
                cout << "\ec";
            }
            if (game.menu.optionVertical == 5) {
                cout << "\nTemplate [ITENS]";
                getch();
                cout << "\ec";
            }
            if (game.menu.optionVertical == 6) {
                cout << "\nTemplate [INIMIGOS]";
                getch();
                cout << "\ec";
            }
            if (game.menu.optionVertical == 7) {
                cout << "\ec";
                game.menu.optionVertical = 4;
                game.codex = false;
            }
            break;
    }
}

int VA(int number){
    if(number<0){
        number*=-1;
    }
    return number;
}

void player_verifiers(GAME &game){
    if(game.map.tiles[game.player.pos.Y][game.player.pos.X]==EMPTY){
        game.next = true;
        game.map.floor++;
        cout<<"CAINDO...";
        while((clock()-game.player.clockSpeed)<1500){}
    }
    if(game.map.tiles[game.player.pos.Y][game.player.pos.X]==STAIRBLOCK){
        if(game.player.keyInput==13){
            game.next = true;
            game.map.floor++;
            cout<<"DESCENDO...";
            while((clock()-game.player.clockSpeed)<1500){}
        }
    }
}

void move_monsters(GAME &game){
    int blocks[2] = {FREEBLOCK,STAIRBLOCK};
    for(int monster=0;monster<game.monsterQuantity;monster++){
        if(game.monsters[monster].alive==true){
            if((clock()-game.monsters[monster].clockSpeed)>1000/game.monsters[monster].attributes.dexterity){
                game.monsters[monster].clockSpeed = clock();
                POS targetPos = {0,0};
                int direction = rand()%4;
                for(int block=0;block<2;block++){
                    if(direction==0){
                        if(game.map.tiles[game.monsters[monster].pos.Y-1][game.monsters[monster].pos.X]==blocks[block]){
                            targetPos.Y--;
                        }
                    }
                    if(direction==1){
                        if(game.map.tiles[game.monsters[monster].pos.Y+1][game.monsters[monster].pos.X]==blocks[block]){
                            targetPos.Y++;
                        }
                    }
                    if(direction==2){
                        if(game.map.tiles[game.monsters[monster].pos.Y][game.monsters[monster].pos.X-1]==blocks[block]){
                            targetPos.X--;
                        }
                    }
                    if(direction==3){
                        if(game.map.tiles[game.monsters[monster].pos.Y][game.monsters[monster].pos.X+1]==blocks[block]){
                            targetPos.X++;
                        }
                    }
                }
                game.monsters[monster].pos.Y+=targetPos.Y;
                game.monsters[monster].pos.X+=targetPos.X;
            }
        }
    }
}

void player_input(GAME &game){
    if(kbhit()){
        if((clock()-game.player.clockSpeed)>250/game.player.attributes.dexterity){
            game.player.clockSpeed = clock();
            game.player.keyInput = getch();
            POS targetPos = {0,0};
            int blocks[3] = {FREEBLOCK,STAIRBLOCK,EMPTY};
            for(int block=0;block<3;block++){
                if(block==2 && rand()%10!=0){
                    continue;
                }
                if(game.player.keyInput==119){
                    if(game.map.tiles[game.player.pos.Y-1][game.player.pos.X]==blocks[block]){
                        targetPos.Y--;
                    }
                }
                if(game.player.keyInput==115){
                    if(game.map.tiles[game.player.pos.Y+1][game.player.pos.X]==blocks[block]){
                        targetPos.Y++;
                    }
                }
                if(game.player.keyInput==97){
                    if(game.map.tiles[game.player.pos.Y][game.player.pos.X-1]==blocks[block]){
                        targetPos.X--;
                    }
                }
                if(game.player.keyInput==100){
                    if(game.map.tiles[game.player.pos.Y][game.player.pos.X+1]==blocks[block]){
                        targetPos.X++;
                    }
                }
                if(game.player.keyInput==105){
                    cout<<"\ec";
                    if(game.player.inventoryOpened == false){
                        game.player.inventoryOpened = true;
                    }else{
                        game.player.inventoryOpened = false;
                    }
                    game.player.keyInput = 0;
                }
            }
            game.player.pos.Y+=targetPos.Y;
            game.player.pos.X+=targetPos.X;
        }else{
            getch();
        }
    }
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
                    if(!(y==-tamY || y==tamY-1 || x==-tamX || x==tamX-1)){
                        game.map.tiles[Y+y][X+x] = FREEBLOCK;
                    }
                    if(game.map.tiles[Y+y][X+x]==EMPTY){
                        if(y==-tamY || y==tamY-1 || x==-tamX || x==tamX-1){
                            game.map.tiles[Y+y][X+x] = SOLIDBLOCK;
                        }
                    }
                }
            }
        }
        if(i>=game.map.floor*2){
            if(rand()%2==0){
                game.map.tiles[Y][X] = STAIRBLOCK;
                break;
            }
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
    for(int monster=0;monster<game.monsterQuantity;monster++){
        if(rand()%(11-game.map.floor)==0){
            int posY;
            int posX;
            bool success = false;
            int attPoints = game.player.nivel;
            while(attPoints>0){
                int attribute = rand()%5;
                if(attribute==0){
                    game.monsters[monster].status.hpMax+=1;
                }
                if(attribute==1){
                    game.monsters[monster].status.defense+=1;
                }
                if(attribute==2){
                    game.monsters[monster].attributes.strength+=1;
                }
                if(attribute==3){
                    game.monsters[monster].attributes.intelligence+=1;
                }
                if(attribute==4){
                    game.monsters[monster].attributes.dexterity+=1;
                }
                attPoints--;
            }
            while(!success){
                posY = rand()%MAPSIZEY;
                posX = rand()%MAPSIZEX;
                if(game.map.tiles[posY][posX] == FREEBLOCK){
                    success = true;
                    break;
                }
                for(int otherMonster=0;otherMonster<game.monsterQuantity;otherMonster++){
                    if(otherMonster==monster){
                        continue;
                    }
                    if(game.monsters[monster].pos.Y == game.monsters[otherMonster].pos.Y && game.monsters[monster].pos.X == game.monsters[otherMonster].pos.X){
                        success = false;
                    }
                }
            }
            if(success){
                game.monsters[monster].id = rand()%game.map.floor;
                game.monsters[monster].pos.Y = posY;
                game.monsters[monster].pos.X = posX;
                game.monsters[monster].status.hp = game.monsters[monster].status.hpMax*10;
                game.monsters[monster].alive = true;
            }
        }else{
            continue;
        }
    }
}

void show_inventory(GAME &game){
    cout << "\e[?25l\e[H";
    cout << "\e[1;1H";
    new_line("┏","━","┓",3*3);
    for(int y=0;y<4;y++){
        cout<<"┃";
        for(int x=0;x<3;x++){
            if(game.player.inventory[y][x].id==0){
                cout<<"   ";
            }
        }
        cout<<"\e[0m┃\n";
    }
    new_line("┗","━","┛",3*3);
}

void render_map(GAME &game){
    cout << "\e[?25l\e[H";
    cout << "\e[1;1H";
    int vision = 10;
    for(int y=-1;y<=1;y++){
        for(int x=-1;x<=1;x++){
            game.map.memory[game.player.pos.Y+y][game.player.pos.X+x] = 1;
        }
    }
    new_line("┏","━","┓",vision*2);
    for(int y=-vision;y<vision;y++){
        cout<<"┃";
        for(int x=-vision;x<vision;x++){
            if(game.map.memory[game.player.pos.Y+y][game.player.pos.X+x]==1){
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
                    cout<<"\e[48;5;246m\e[38;5;242mv";
                }
                if(y==0 && x==0){
                    cout<<"\e[1D";
                    cout<<"\e[38;5;255m@";
                }
                for(int monster=0;monster<50;monster++){   
                    if(game.monsters[monster].alive){
                        if(game.monsters[monster].pos.Y==game.player.pos.Y+y && game.monsters[monster].pos.X==game.player.pos.X+x){
                            cout<<"\e[1D";
                            if(game.monsters[monster].id==0){
                                cout<<"\e[38;5;31mO";
                            }
                            if(game.monsters[monster].id==1){
                                cout<<"\e[38;5;46mG";
                            }
                            if(game.monsters[monster].id==2){
                                cout<<"\e[38;5;202mK";
                            }
                            if(game.monsters[monster].id==3){
                                cout<<"\e[38;5;34mR";
                            }
                            if(game.monsters[monster].id==4){
                                cout<<"\e[38;5;106mH";
                            }
                            if(game.monsters[monster].id==5){
                                cout<<"\e[38;5;53mD";
                            }
                            if(game.monsters[monster].id==6){
                                cout<<"\e[38;5;13mM";
                            }
                            if(game.monsters[monster].id==7){
                                cout<<"\e[38;5;227mT";
                            }
                            if(game.monsters[monster].id==8){
                                cout<<"\e[38;5;214mA";
                            }
                            if(game.monsters[monster].id==9){
                                cout<<"\e[38;5;52mE";
                            }
                            if(game.monsters[monster].id==10){
                                cout<<"\e[38;5;3mB";
                            }
                        }
                    }
                }
            }else{
                if(!game.map.tiles[game.player.pos.Y+y][game.player.pos.X+x]==EMPTY){
                    cout<<"\e[0m?";
                }else{
                    cout<<"\e[0m ";
                }
            }
        }
        cout<<"\e[0m┃\n";
    }
    new_line("┗","━","┛",vision*2);
}