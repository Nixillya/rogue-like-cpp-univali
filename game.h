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
#define KEYBLOCK 4

using namespace std;

struct POS{
    int X = -1;
    int Y = -1;
};

struct ATTRIBUTES{
    double hp = 1;
    double hpMax = 1;
    int defense = 1;
    int strength = 1;
    int dexterity = 1;
    int intelligence = 1;
};

struct ITEM{
    int id = 0;
    int type = 0;
    int heal = 0;
    int hp = 0;
    int strength = 0;
    int defense = 0;
    int dexterity = 0;
    int intelligence = 0;
    bool equipped = false;
};

struct PLAYER{
    int attPoints = 4;
    int nivel = 1;
    int points = 0;
    int exp = 0;
    int keyInput = 0;
    int clockSpeed = clock();
    bool alive = true;
    bool inventoryOpened = false;
    bool fallen = false;
    bool key = false;
    ITEM inventory[4][3];
    POS pos;
    POS inventorySelection = {0,0};
    ATTRIBUTES attributes;
};

struct MONSTER{
    int id = 0;
    bool attacked = false;
    bool alive = false;
    bool key = false;
    int clockSpeed = clock();
    POS pos;
    ATTRIBUTES attributes;
};

struct MAP{
    int tiles[MAPSIZEY][MAPSIZEX];
    int memory[MAPSIZEY][MAPSIZEX];
    int floor = 1;
    int floorMonsters = 0;
    PLAYER player;
    MONSTER monsters[50];
};

struct MENU{
    int optionVertical = 4;
    int optionHorizontal = 1;;
};

struct GAME{
    int monsterQuantity = 50;
    int attSelection = 0;
    bool exit = false;
    bool play = false;
    bool pause = false;
    bool next = false;
    bool codex = false;
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
                cout << "\ec";
                game.codex = true;  
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
    bool codexPages = true;
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
        cout << "┃ > [BESTIÁRIO] ┃\n";
    } else {
        cout << "┃ [BESTIÁRIO]   ┃\n";
    }
    if (game.menu.optionVertical == 7) {
        cout << "┃ > [HISTÓRIA]  ┃\n";
    } else {
        cout << "┃ [HISTÓRIA]    ┃\n";
    }
    if (game.menu.optionVertical == 8) {
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
                game.menu.optionVertical = 8;
            }
            break;

        case 115: // Ir para baixo
                game.menu.optionVertical++;
                if (game.menu.optionVertical > 8) {
                    game.menu.optionVertical = 4;
                }
                break;
//------------------------------------------------------------------//
        case 13: // Input (ENTER)
            if (game.menu.optionVertical == 4) { //COMO JOGAR
                new_line("┏","━","┓",46);
                cout << "┃ Controles:                                   ┃\n";
                cout << "┃                                              ┃\n";
                cout << "┃                Cima                          ┃\n";
                cout << "┃                 ^                            ┃\n";
                cout << "┃                 W                            ┃\n";
                cout << "┃    Esquerda < A S D > Direita                ┃\n";
                cout << "┃                 V                            ┃\n";
                cout << "┃               Baixo                          ┃\n";
                cout << "┃                                              ┃\n";
                new_line("┣","━","┫",46);
                cout << "┃ Iventário -> I                               ┃\n";
                cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ OBJETIVO ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃ - Sobreviva pelos 11 anderes, até chegar no último andar, e matar o Chefão.    ┃\n";
                cout << "┃ - Elimine o monstro carregando a chave, e usa-la para abrir a entrada para o   ┃\n";
                cout << "┃ - próximo andar.                                                               ┃\n";
                cout << "┃                                                                                ┃\n";
                cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ GERAÇÃO DO MAPA ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃ - O mapa é gerado aleatoriamente em todas as runs. Cuidado ao andar pelas      ┃\n";
                cout << "┃   áreas abertas sem paredes, você pode cair no abismo para o próximo andar,    ┃\n";
                cout << "┃   há uma chance de você sobreviver a queda, ou morrer instananeamente.         ┃\n";
                cout << "┃ - Os andares possuem armadilhas, e podem causar danos a você e os monstros.    ┃\n";
                cout << "┃                                                                                ┃\n";
                cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ INIMIGOS ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃                                                                                ┃\n";


                getch();
                cout << "\ec";
            } 
//------------------------------------------------------------------//
            if (game.menu.optionVertical == 5) { // ITENS
                cout << "\ec";
                game.menu.optionVertical = 1;
                while (codexPages == true) {
                    if (game.menu.optionVertical < 1) {
                        game.menu.optionVertical = 2;
                    }
                    if  (game.menu.optionVertical > 2) {
                        game.menu.optionVertical = 1;
                    }
                    if (game.menu.optionHorizontal < 1) {
                        game.menu.optionHorizontal = 3;
                    }
                    if (game.menu.optionHorizontal > 3) {
                        game.menu.optionHorizontal = 1;
                    }
                    cout << "\e[?25l\e[H";
                    new_line("┏","━","┓",26);
                    cout << "┃       CODEX: ITENS       ┃\n";
                    new_line("┣","━","┫",26);
                    if (game.menu.optionVertical == 1) {
                        if (game.menu.optionHorizontal == 1) {
                            cout << "┃  \e[93m[Página 1 - ARMAS]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 2) {
                            cout << "┃  \e[93m[Página 2 - ARMADURAS]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 3) {
                            cout << "┃  \e[93m[Página 3 - ITENS]\e[0m   ┃\n";
                        }
                        cout << "┃                          ┃\n";
                    } else {
                        if (game.menu.optionHorizontal == 1) {
                            cout << "┃ [Página 1 - ARMAS]    ┃\n";
                        }
                        if (game.menu.optionHorizontal == 2) {
                            cout << "┃ [Página 2 - ARMADURAS]    ┃\n";
                        }
                        if (game.menu.optionHorizontal == 3) {
                            cout << "┃ [Página 3 - ITENS]    ┃\n";
                        }
                        cout << "┃                          ┃\n";
                    }
                    if (game.menu.optionVertical == 2) {
                        cout << "┃  \e[93m[Voltar]\e[0m                ┃\n";
                    } else {
                        cout << "┃ [Voltar]                 ┃\n";
                    }
                    new_line("┗","━","┛",26);
                    cout<<"\e[?25l";
                    switch (game.menu.optionHorizontal) {
                        case 1:
                            cout << "\nTemplate 1.";
                        break;

                        case 2:
                            cout << "\nTemplate 2.";
                        break;

                        case 3:
                            cout << "\nTemplate 3.";
                        break;
                    }
                    key = getch();
                    switch (key) {
                        case 119: // Cima
                            game.menu.optionVertical--;
                        break;

                        case 115: // Baixo
                            game.menu.optionVertical++;
                        break;

                        case 97: //Esquerda
                            game.menu.optionHorizontal--;
                            cout << "\ec";
                        break;

                        case 100: //Direita
                            game.menu.optionHorizontal++;
                            cout << "\ec";
                        break;

                        case 13:
                            if (game.menu.optionVertical == 2) {
                                cout << "\ec";
                                game.menu.optionVertical = 5;
                                game.menu.optionHorizontal = 1;
                                codexPages = false;
                            }
                        break;
                    }
                }
            }
//------------------------------------------------------------------//
            if (game.menu.optionVertical == 6) { // BESTIARIO
                cout << "\ec"; 
                game.menu.optionVertical = 1;
                while (codexPages == true) {
                    if (game.menu.optionVertical < 1) {
                        game.menu.optionVertical = 2;
                    }
                    if  (game.menu.optionVertical > 2) {
                        game.menu.optionVertical = 1;
                    }
                    if (game.menu.optionHorizontal < 1) {
                        game.menu.optionHorizontal = 11;
                    }
                    if (game.menu.optionHorizontal > 11) {
                        game.menu.optionHorizontal = 1;
                    }
                    cout << "\e[?25l\e[H";
                    new_line("┏","━","┓",26);
                    cout << "┃       CODEX: BESTIÁRIO       ┃\n";
                    new_line("┣","━","┫",26);
                    if (game.menu.optionVertical == 1) {
                        if (game.menu.optionHorizontal == 1) {
                            cout << "┃  \e[93m[Página 1 - SLIME]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 2) {
                            cout << "┃  \e[93m[Página 2 - GOBLIN]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 3) {
                            cout << "┃  \e[93m[Página 3 - KOBOLD]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 4) {
                            cout << "┃  \e[93m[Página 4 - ORC]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 5) {
                            cout << "┃  \e[93m[Página 5 - OGRO]\e[0m   ┃\n";
                        }
                        if  (game.menu.optionHorizontal == 6) {
                            cout << "┃  \e[93m[Página 6 - TROLL]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 7) {
                            cout << "┃  \e[93m[Página 7 - MÍMICO]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 8) {
                            cout << "┃  \e[93m[Página 8 - TROGLODITA]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 9) {
                            cout << "┃  \e[93m[Página 9 - AUTÔNOMO]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 10) {
                            cout << "┃  \e[93m[Página 10 - ESCÓRIA]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 11) {
                            cout << "┃  \e[93m[Página 11 - BOSS]\e[0m   ┃\n";
                        }
                        cout << "┃                          ┃\n";
                    } else {
                        if (game.menu.optionHorizontal == 1) {
                            cout << "┃ [Página 1 - SLIME]    ┃\n";
                        }
                        if (game.menu.optionHorizontal == 2) {
                            cout << "┃ [Página 2 - GOBLIN]    ┃\n";
                        }
                        if (game.menu.optionHorizontal == 3) {
                            cout << "┃ [Página 3 - KOBOLD]    ┃\n";
                        }
                        if (game.menu.optionHorizontal == 4) {
                            cout << "┃  [Página 4 - ORC]   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 5) {
                            cout << "┃  [Página 5 - OGRO]   ┃\n";
                        }
                        if  (game.menu.optionHorizontal == 6) {
                            cout << "┃  [Página 6 - TROLL]   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 7) {
                            cout << "┃  [Página 7 - MÍMICO]   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 8) {
                            cout << "┃  [Página 8 - TROGLODITA]   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 9) {
                            cout << "┃  [Página 9 - AUTôNOMO]   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 10) {
                            cout << "┃  [Página 10 - ESCÓRIA]   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 11) {
                            cout << "┃  [Página 11 - BOSS]   ┃\n";
                        }
                        cout << "┃                          ┃\n";
                    }
                    if (game.menu.optionVertical == 2) {
                        cout << "┃  \e[93m[Voltar]\e[0m                ┃\n";
                    } else {
                        cout << "┃ [Voltar]                 ┃\n";
                    }
                    new_line("┗","━","┛",26);
                    cout<<"\e[?25l";
                    switch (game.menu.optionHorizontal) {
                        case 1:
                            cout << "\nTemplate 1.";
                        break;

                        case 2:
                            cout << "\nTemplate 2.";
                        break;

                        case 3:
                            cout << "\nTemplate 3.";
                        break;

                        case 4:
                            cout << "\nTemplate 4.";
                        break;

                        case 5:
                            cout << "\nTemplate 5.";
                        break;

                        case 6:
                            cout << "\nTemplate 6.";
                        break;

                        case 7:
                            cout << "\nTemplate 7.";
                        break;

                        case 8:
                            cout << "\nTemplate 8.";
                        break;

                        case 9:
                            cout << "\nTemplate 9.";
                        break;

                        case 10:
                            cout << "\nTemplate 10.";
                        break;

                        case 11:
                            cout << "\nTemplate 11.";
                        break;
                        
                    }
                    key = getch();
                    switch (key) {
                        case 119: // Cima
                            game.menu.optionVertical--;
                        break;

                        case 115: // Baixo
                            game.menu.optionVertical++;
                        break;

                        case 97: //Esquerda
                            game.menu.optionHorizontal--;
                            cout << "\ec";
                        break;

                        case 100: //Direita
                            game.menu.optionHorizontal++;
                            cout << "\ec";
                        break;

                        case 13:
                            if (game.menu.optionVertical == 2) {
                                cout << "\ec";
                                game.menu.optionVertical = 6;
                                game.menu.optionHorizontal = 1;
                                codexPages = false;
                            }
                        break;
                    }
                }
            }
//------------------------------------------------------------------//
            if (game.menu.optionVertical == 7) { // HISTÓRIA
                cout << "\nTemplate [HISTÓRIA]";
                getch();
                cout << "\ec";
            }
//------------------------------------------------------------------//
            if (game.menu.optionVertical == 8) { // SAIR
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
    cout<<"\e[23;1H";
    if(game.map.player.attributes.hp<1){
        cout<<"MORREU";
        game.map.player.clockSpeed = clock();
        while((clock()-game.map.player.clockSpeed)<3000){}
        cout<<"\ec";
        game.play = false;
    }
    if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X]==EMPTY){
        game.next = true;
        game.map.player.fallen = true;
        game.map.floor++;
        cout<<"CAINDO...";
        game.map.player.attributes.hp = 1;
        if(rand()%2==0){
            game.map.player.attributes.hp -= 1;
        }
        game.map.player.clockSpeed = clock();
        while((clock()-game.map.player.clockSpeed)<2000){}
    }
    if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X]==STAIRBLOCK){
        if(game.map.player.keyInput==13){
            game.next = true;
            game.map.floor++;
            cout<<"DESCENDO...";
            game.map.player.clockSpeed = clock();
            while((clock()-game.map.player.clockSpeed)<1500){}
        }
    }
}

void move_monsters(GAME &game){
    int blocks[3] = {FREEBLOCK,STAIRBLOCK,KEYBLOCK};
    for(int monster=0;monster<game.monsterQuantity;monster++){
        if(game.map.tiles[game.map.monsters[monster].pos.Y][game.map.monsters[monster].pos.X]==KEYBLOCK){
            game.map.tiles[game.map.monsters[monster].pos.Y][game.map.monsters[monster].pos.X] = FREEBLOCK;
            game.map.monsters[monster].key = true;
        }
        if(game.map.monsters[monster].attributes.hp<1){
            if(game.map.monsters[monster].alive==true){
                game.map.player.exp++;
                game.map.monsters[monster].alive = false;
                if(game.map.monsters[monster].key){
                    game.map.tiles[game.map.monsters[monster].pos.Y][game.map.monsters[monster].pos.X] = KEYBLOCK;
                    game.map.monsters[monster].key = false;
                }
            }
        }
        if(game.map.monsters[monster].alive==true){
            if((clock()-game.map.monsters[monster].clockSpeed)>1000/game.map.monsters[monster].attributes.dexterity){
                game.map.monsters[monster].clockSpeed = clock();
                POS targetPos = {0,0};
                int direction = rand()%4;
                bool success = true;
                for(int block=0;block<3;block++){
                    if(direction==0){
                        if(game.map.tiles[game.map.monsters[monster].pos.Y-1][game.map.monsters[monster].pos.X]==blocks[block]){
                            targetPos.Y--;
                        }
                    }
                    if(direction==1){
                        if(game.map.tiles[game.map.monsters[monster].pos.Y+1][game.map.monsters[monster].pos.X]==blocks[block]){
                            targetPos.Y++;
                        }
                    }
                    if(direction==2){
                        if(game.map.tiles[game.map.monsters[monster].pos.Y][game.map.monsters[monster].pos.X-1]==blocks[block]){
                            targetPos.X--;
                        }
                    }
                    if(direction==3){
                        if(game.map.tiles[game.map.monsters[monster].pos.Y][game.map.monsters[monster].pos.X+1]==blocks[block]){
                            targetPos.X++;
                        }
                    }
                }
                if(game.map.monsters[monster].pos.Y+targetPos.Y==game.map.player.pos.Y && game.map.monsters[monster].pos.X+targetPos.X==game.map.player.pos.X){
                    success = false;
                    int attack = rand()%game.map.monsters[monster].attributes.strength+1;
                    int defended = rand()%game.map.player.attributes.defense;
                    if(defended==0){
                        if(rand()%(100/game.map.player.attributes.intelligence)==0){
                            defended = rand()%game.map.player.attributes.defense+1;
                        }
                    }
                    if(defended>attack){
                        defended = attack;
                    }
                    game.map.player.attributes.hp-=(attack-defended);
                }
                for(int otherMonster=0;otherMonster<game.monsterQuantity;otherMonster++){
                    if(otherMonster==monster){
                        continue;
                    }
                    if(game.map.monsters[monster].pos.Y+targetPos.Y==game.map.monsters[otherMonster].pos.Y && game.map.monsters[monster].pos.X+targetPos.X==game.map.monsters[otherMonster].pos.X){
                        success = false;
                    }
                }
                if(success){
                    game.map.monsters[monster].pos.Y+=targetPos.Y;
                    game.map.monsters[monster].pos.X+=targetPos.X;
                }
            }
        }else{
            game.map.monsters[monster].pos.Y=-1;
            game.map.monsters[monster].pos.X=-1;
        }
    }
}

void player_input(GAME &game){
    if(kbhit()){
        game.map.player.keyInput = getch();
        if((clock()-game.map.player.clockSpeed)>1000/game.map.player.attributes.dexterity){
            if(game.map.player.exp>=game.map.player.nivel){
                game.map.player.exp = 0;
                game.map.player.attPoints++;
                game.map.player.nivel++;
            }
            game.map.player.clockSpeed = clock();
            POS targetPos = {0,0};
            int blocks[4] = {FREEBLOCK,STAIRBLOCK,EMPTY,KEYBLOCK};
            bool success = true;
            for(int block=0;block<4;block++){
                if(block==2 && rand()%10!=0){
                    continue;
                }
                if(game.map.player.keyInput==119){
                    if(game.map.player.inventoryOpened){
                        game.map.player.inventorySelection.Y--;
                    }
                    if(game.map.tiles[game.map.player.pos.Y-1][game.map.player.pos.X]==blocks[block]){
                        targetPos.Y--;
                    }
                }
                if(game.map.player.keyInput==115){
                    if(game.map.player.inventoryOpened){
                        game.map.player.inventorySelection.Y++;
                    }
                    if(game.map.tiles[game.map.player.pos.Y+1][game.map.player.pos.X]==blocks[block]){
                        targetPos.Y++;
                    }
                }
                if(game.map.player.keyInput==97){
                    if(game.map.player.inventoryOpened){
                        game.map.player.inventorySelection.X--;
                    }
                    if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X-1]==blocks[block]){
                        targetPos.X--;
                    }
                }
                if(game.map.player.keyInput==100){
                    if(game.map.player.inventoryOpened){
                        game.map.player.inventorySelection.X++;
                    }
                    if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X+1]==blocks[block]){
                        targetPos.X++;
                    }
                }
                if(game.map.player.keyInput==105){
                    cout<<"\ec";
                    if(game.map.player.inventoryOpened == false){
                        game.map.player.inventoryOpened = true;
                    }else{
                        game.map.player.inventoryOpened = false;
                    }
                    game.map.player.keyInput = 0;
                }
                if(game.map.player.keyInput==13){
                    if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X]==KEYBLOCK){
                        game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X] = FREEBLOCK;
                        game.map.player.key = true;
                    }
                }
                if(game.map.player.inventoryOpened){
                    if(game.map.player.inventorySelection.Y<0){
                        game.map.player.inventorySelection.Y = 0;
                    }
                    if(game.map.player.inventorySelection.Y>4){
                        game.map.player.inventorySelection.Y = 4;
                    }
                    if(game.map.player.inventorySelection.X<0){
                        game.map.player.inventorySelection.X = 0;
                    }
                    if(game.map.player.inventorySelection.X>4){
                        game.map.player.inventorySelection.X = 4;
                    }
                    break;
                }
            }
            if(!game.map.player.inventoryOpened){
                for(int monster=0;monster<game.monsterQuantity;monster++){
                    if(game.map.monsters[monster].pos.Y==game.map.player.pos.Y+targetPos.Y && game.map.monsters[monster].pos.X==game.map.player.pos.X+targetPos.X){
                        game.map.monsters[monster].attacked = true;
                        int attack = rand()%game.map.player.attributes.strength+1;
                        int defended = rand()%game.map.monsters[monster].attributes.defense;
                        if(defended==0){
                            if(rand()%(100/game.map.monsters[monster].attributes.intelligence)==0){
                                defended = rand()%game.map.player.attributes.defense+1;
                            }
                        }
                        if(defended>attack){
                            defended = attack;
                            game.map.monsters[monster].attacked = false;
                        }
                        game.map.monsters[monster].attributes.hp-=(attack-defended);
                        success = false;
                    }
                }
                if(success){
                    game.map.player.pos.Y+=targetPos.Y;
                    game.map.player.pos.X+=targetPos.X;
                }
            }else{
                game.map.player.keyInput = 0;
            }
        }
    }
}

void put_attributes(GAME &game){
    cout << "\e[?25l\e[H";
    cout << "\e[1;1H";
    cout<<"PONTOS DE ATRIBUTOS: "<<game.map.player.attPoints<<" ";
    cout<<"\n\n";
    if(game.attSelection<0){
        game.attSelection = 0;
    }
    if(game.attSelection>4){
        game.attSelection = 4;
    }
    if(game.attSelection==0){
        cout<<" \e[93mHP: "<<game.map.player.attributes.hpMax<<" \e[0m";
    }else{
        cout<<"HP: "<<game.map.player.attributes.hpMax<<" ";
    }
    cout<<"\n";
    if(game.attSelection==1){
        cout<<" \e[93mDEFESA: "<<game.map.player.attributes.defense<<" \e[0m";
    }else{
        cout<<"DEFESA: "<<game.map.player.attributes.defense<<" ";
    }
    cout<<"\n";
    if(game.attSelection==2){
        cout<<" \e[93mFORÇA: "<<game.map.player.attributes.strength<<" \e[0m";
    }else{
        cout<<"FORÇA: "<<game.map.player.attributes.strength<<" ";
    }
    cout<<"\n";
    if(game.attSelection==3){
        cout<<" \e[93mINTELIGENCIA: "<<game.map.player.attributes.intelligence<<" \e[0m";
    }else{
        cout<<"INTELIGENCIA: "<<game.map.player.attributes.intelligence<<" ";
    }
    cout<<"\n";
    if(game.attSelection==4){
        cout<<" \e[93mDESTREZA: "<<game.map.player.attributes.dexterity<<" \e[0m";
    }else{
        cout<<"DESTREZA: "<<game.map.player.attributes.dexterity<<" ";
    }
    int key = getch();
    if(key==119){
        game.attSelection--;
    }
    if(key==115){
        game.attSelection++;
    }
    if(key==13){
        if(game.attSelection==0){
            game.map.player.attributes.hpMax++;
            game.map.player.attributes.hp++;
        }
        if(game.attSelection==1){
            game.map.player.attributes.defense++;
        }
        if(game.attSelection==2){
            game.map.player.attributes.strength++;
        }
        if(game.attSelection==3){
            game.map.player.attributes.intelligence++;
        }
        if(game.attSelection==4){
            game.map.player.attributes.dexterity++;
        }
        game.map.player.attPoints--;
    }
}

void create_map(GAME &game){
    game.next = false;
    game.map.player.key = false;
    while(game.map.player.attPoints>0){
        if(!game.map.player.fallen){
            put_attributes(game);
        }else{
            game.map.player.fallen = false;
            break;
        }
    }
    if(game.map.floor==1){
        game.map.player.inventory[0][0].id = 1;
        game.map.player.inventory[0][0].type = 1;
        game.map.player.inventory[0][0].equipped = true;
        game.map.player.inventory[0][0].strength = 1;

        game.map.player.inventory[0][1].id = 2;
        game.map.player.inventory[0][1].type = 2;
        game.map.player.inventory[0][1].equipped = true;
        game.map.player.inventory[0][1].defense = 1;

        game.map.player.inventory[0][2].id = 3;
        game.map.player.inventory[0][2].type = 3;
        game.map.player.inventory[0][2].equipped = true;
        int attribute = rand()%5;
        if(attribute==0){
            game.map.player.inventory[0][2].defense = 1;
        }
        if(attribute==1){
            game.map.player.inventory[0][2].strength = 1;
        }
        if(attribute==2){
            game.map.player.inventory[0][2].hp = 1;
        }
        if(attribute==3){
            game.map.player.inventory[0][2].intelligence = 1;
        }
        if(attribute==4){
            game.map.player.inventory[0][2].dexterity = 1;
        }
    }
    for(int y=0;y<MAPSIZEY;y++){
        for(int x=0;x<MAPSIZEX;x++){
            game.map.tiles[y][x] = EMPTY;
            game.map.memory[y][x] = 0;
        }
    }
    int Y = MAPSIZEY/2;
    int X = MAPSIZEX/2;
    game.map.player.pos.Y = Y;
    game.map.player.pos.X = X;
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
                direction = rand()%4;
            }
            if(rand()%5==0){
                direction = rand()%4;
            }
            if(rand()%100==0){
                break;
            }
        }
        if(i>=game.map.floor*2){
            if(rand()%2==0){
                game.map.tiles[Y][X] = STAIRBLOCK;
                break;
            }
        }
        i++;
    }
    while(true){
        int y = rand()%MAPSIZEY;
        int x = rand()%MAPSIZEX;
        if(game.map.tiles[y][x]==FREEBLOCK){
            game.map.tiles[y][x] = KEYBLOCK;
            break;
        }
    }
    MONSTER resetMonsters[game.monsterQuantity];
    for(int monster=0;monster<game.monsterQuantity;monster++){
        game.map.monsters[monster] = resetMonsters[monster];
        if(rand()%(11-game.map.floor)==0 || monster==0){
            int posY;
            int posX;
            bool success = false;
            int attPoints = (game.map.player.nivel*2)+(game.map.floor/2);
            while(attPoints>0){
                int attribute = rand()%5;
                if(game.map.monsters[monster].id==0){
                    if(rand()%2==0){
                        attribute = -1;
                    }
                }
                if(game.map.monsters[monster].id==1){
                    if(rand()%2==0){
                        attribute = 4;
                    }
                    if(attribute!=4){
                        if(rand()%2==0){
                            attribute = -1;
                        } 
                    }
                }
                if(attribute==0){
                    game.map.monsters[monster].attributes.hpMax+=1;
                }
                if(attribute==1){
                    game.map.monsters[monster].attributes.defense+=1;
                }
                if(attribute==2){
                    game.map.monsters[monster].attributes.strength+=1;
                }
                if(attribute==3){
                    game.map.monsters[monster].attributes.intelligence+=1;
                }
                if(attribute==4){
                    game.map.monsters[monster].attributes.dexterity+=1;
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
                    if(game.map.monsters[monster].pos.Y == game.map.monsters[otherMonster].pos.Y && game.map.monsters[monster].pos.X == game.map.monsters[otherMonster].pos.X){
                        success = false;
                    }
                }
            }
            if(success){
                game.map.monsters[monster].id = rand()%game.map.floor;
                game.map.monsters[monster].pos.Y = posY;
                game.map.monsters[monster].pos.X = posX;
                game.map.monsters[monster].attributes.hp = game.map.monsters[monster].attributes.hpMax;
                game.map.monsters[monster].alive = true;
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
            cout<<" ";
            if(y==game.map.player.inventorySelection.Y && x==game.map.player.inventorySelection.X){
                cout<<"\e[38;5;226m\e[48;5;238m";
            }
            if(game.map.player.inventory[y][x].id==0){
                cout<<" ";
            }
            if(game.map.player.inventory[y][x].id==1){
                cout<<"󰓥";
            }
            if(game.map.player.inventory[y][x].id==2){
                cout<<"󰒘";
            }
            if(game.map.player.inventory[y][x].id==3){
                cout<<"";
            }
            cout<<"\e[0m ";
        }
        cout<<"\e[0m┃\n";
    }
    new_line("┗","━","┛",3*3);
}

int simulate_vision(GAME &game,int y,int x,int i=0){
    if(i>=1){
        if(rand()%2==0){
            y += (y!=0) ? y/VA(y) : 0;
        }
        if(rand()%2==0){
            x += (x!=0) ? x/VA(x) : 0;
        }
    }
    if(game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]!=SOLIDBLOCK){
        i++;
        game.map.memory[game.map.player.pos.Y+y][game.map.player.pos.X+x] = 1;
        if(i<game.map.player.attributes.intelligence){
            simulate_vision(game,y,x,i);
        }
    }
    game.map.memory[game.map.player.pos.Y+y][game.map.player.pos.X+x] = 1;
    return 0;
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
    for(int y=-vision;y<vision;y++){
        cout<<"┃";
        for(int x=-vision;x<vision;x++){
            if(game.map.memory[game.map.player.pos.Y+y][game.map.player.pos.X+x]==1){
                if(game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]==EMPTY){
                    cout<<"\e[0m ";
                }
                if(game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]==FREEBLOCK){
                    cout<<"\e[48;5;246m ";
                }
                if(game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]==SOLIDBLOCK){
                    cout<<"\e[48;5;242m ";
                }
                if(game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]==STAIRBLOCK){
                    if(!game.map.player.key){
                        cout<<"\e[48;5;246m\e[38;5;255m󱠟";
                    }else{
                        cout<<"\e[48;5;246m\e[38;5;255m󱊾";
                    }
                }
                if(game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]==KEYBLOCK){
                    cout<<"\e[48;5;246m\e[38;5;3m";
                }
                if(y==0 && x==0){
                    cout<<"\e[1D";
                    if(game.map.player.attributes.hp<1){
                        cout<<"\e[38;5;255m󰮢";
                    }else{
                        cout<<"\e[38;5;255m@";
                    }
                }
                for(int monster=0;monster<50;monster++){
                    if(game.map.monsters[monster].alive){
                        if(game.map.monsters[monster].pos.Y==game.map.player.pos.Y+y && game.map.monsters[monster].pos.X==game.map.player.pos.X+x){
                            cout<<"\e[1D";
                            if(game.map.monsters[monster].id==0){ // SLIME
                                cout<<"\e[38;5;31m󰛹";
                            }
                            if(game.map.monsters[monster].id==1){ // GOBLIN
                                cout<<"\e[38;5;46m";
                            }
                            if(game.map.monsters[monster].id==2){ // KOBOLD
                                cout<<"\e[38;5;202mK";
                            }
                            if(game.map.monsters[monster].id==3){ // ORC
                                cout<<"\e[38;5;34mR";
                            }
                            if(game.map.monsters[monster].id==4){ // OGRO
                                cout<<"\e[38;5;106mH";
                            }
                            if(game.map.monsters[monster].id==5){ // TROLL
                                cout<<"\e[38;5;53m";
                            }
                            if(game.map.monsters[monster].id==6){ // MIMICO
                                cout<<"\e[38;5;13mM";
                            }
                            if(game.map.monsters[monster].id==7){ // TROGLODITA
                                cout<<"\e[38;5;227mT";
                            }
                            if(game.map.monsters[monster].id==8){ // AUTONOMO
                                cout<<"\e[38;5;214m󱚝";
                            }
                            if(game.map.monsters[monster].id==9){ // ESCORIA
                                cout<<"\e[38;5;52mE";
                            }
                            if(game.map.monsters[monster].id==10){ // BOSS
                                cout<<"\e[38;5;3m";
                            }
                            if(game.map.monsters[monster].attacked==true){
                                cout<<"\e[1D";
                                cout<<"\e[38;5;1󰞱";
                                game.map.monsters[monster].attacked = false;
                            }
                        }
                    }
                }
            }else{
                if(!game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]==EMPTY){
                    cout<<"\e[48;5;234m ";
                }else{
                    cout<<"\e[0m ";
                }
            }
        }
        cout<<"\e[0m┃\n";
    }
    new_line("┗","━","┛",vision*2);
    cout<<"\e[1;"<<((vision+1)*2)+1<<"H";
    new_line("┏","━","┓",10);
    cout<<"\e[2;"<<((vision+1)*2)+1<<"H┃";
    int bar=0;
    for(bar=bar;bar<((game.map.player.attributes.hp/game.map.player.attributes.hpMax)*10);bar++){
        cout<<"\e[48;5;40m ";
    }
    for(bar=bar;bar<10;bar++){
        cout<<"\e[0m ";
    }
    cout<<"\e[0m┃";
    cout<<"\e[3;"<<((vision+1)*2)+1<<"H";
    new_line("┗","━","┛",10);
    cout<<"\e[4;"<<((vision+1)*2)+1<<"H";
    cout<<"ANDAR: "<<game.map.floor;
    cout<<"\e[5;"<<((vision+1)*2)+1<<"H";
    cout<<"NIVEL: "<<game.map.player.nivel;
    cout<<"\e[6;"<<((vision+1)*2)+1<<"H";
    cout<<"EXP: "<<game.map.player.exp<<"/"<<game.map.player.nivel<<"     ";
    if(game.map.player.key){
        cout<<"\e[7;"<<((vision+1)*2)+1<<"H";
        cout<<"\e[38;5;3m\e[0m";
    }
}