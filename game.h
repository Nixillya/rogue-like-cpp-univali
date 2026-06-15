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
#define TRAPBLOCK 5 // definição para a trap

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
    int heal = 0;
    int hp = 0;
    int strength = 0;
    int defense = 0;
    int dexterity = 0;
    int intelligence = 0;
    int cursed = false;
};

struct PLAYER{
    int attPoints = 5;
    int nivel = 1;
    int points = 0;
    int exp = 0;
    int keyInput = 0;
    int clockSpeed = clock();
    bool alive = true;
    bool inventoryOpened = false;
    bool fallen = false;
    bool key = false;
    bool attacked = false;
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
    POS items[10];
    int floor = 1;
    PLAYER player;
    MONSTER monsters[50];
};

struct MENU{
    int optionVertical = 4;
    int optionHorizontal = 1;;
};

struct GAME{
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
                        game.menu.optionHorizontal = 2;
                    }
                    if (game.menu.optionHorizontal > 2) {
                        game.menu.optionHorizontal = 1;
                    }
                    cout << "\e[?25l\e[H";
                    new_line("┏","━","┓",26);
                    cout << "┃       CODEX: ITENS       ┃\n";
                    new_line("┣","━","┫",26);
                    if (game.menu.optionVertical == 1) {
                        if (game.menu.optionHorizontal == 1) {
                            cout << "┃  \e[93m[Página 1 - ARMAS]\e[0m      ┃\n";
                        }
                        if (game.menu.optionHorizontal == 2) {
                            cout << "┃  \e[93m[Página 2 - ITENS]\e[0m      ┃\n";
                        }
                        cout << "┃                          ┃\n";
                    } else {
                        if (game.menu.optionHorizontal == 1) {
                            cout << "┃ [Página 1 - ARMAS]       ┃\n";
                        } 
                        if (game.menu.optionHorizontal == 2) {
                            cout << "┃ [Página 2 - ITENS]       ┃\n";
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
                        new_line("┏","━","┓",26);
                            cout << "┃ Espada Longa (|):                                                             ┃\n";
                            cout << "┃ Um lâmina forjada em aço pelos melhores orcs que a Horda pode oferecer,       ┃\n";
                            cout << "┃ pesada e implacável nas mãos de um guerreiro determinado.                     ┃\n";
                            cout << "┃ (Arma equipável. Aumenta a Força do jogador em 'X').                          ┃\n";
                            new_line("┣","━","┫",26);
                            cout << "┃ Adagas (=):                                                                   ┃\n";
                            cout << "┃ Um par de lâminas que, na verdade, são presas de um troll, amoladas ao ponto  ┃\n";
                            cout << "┃ de cortarem facilmente até o menor toque. É uma tradição dos trolls cortarem  ┃\n";
                            cout << "┃ as próprias presas e criarem suas próprias adagas, como uma forma de provar   ┃\n";
                            cout << "┃ ser um guerreiro capaz e digno.                                               ┃\n";
                            cout << "┃ Lâminas gêmeas, leves e equilibradas, feitas a partir de presas de Troll,     ┃\n";
                            cout << "┃ perfeitas para cortes rápidos antes que o inimigo possa reagir.               ┃\n";
                            cout << "┃ (Arma equipável. Aumenta a Destreza do jogador em 'X').                       ┃\n";
                            new_line("┣","━","┫",26);
                            cout << "┃ Cajado Elétrico (󱡄):                                                          ┃\n";
                            cout << "┃ Esculpido em madeira antiga, possui um Núcleo Laser de um Autônomo acoplado   ┃\n";
                            cout << "┃ em um capacitador, capaz de disparar feixes de energia pura concentrada. Um   ┃\n";
                            cout << "┃ protótipo que serve para demonstrar a engenharia e ambição dos Kobolds para   ┃\n";
                            cout << "┃ superar a tecnologia dos humanos e anões.                                     ┃\n";
                            cout << "┃ (Arma de ataque à distância. Aumenta a Inteligência do jogador em 'X').       ┃\n";
                            new_line("┗","━","┛",26);
                        break;

                        case 2:
                            new_line("┏","━","┓",26);
                            cout << "┃ Poção Regenerativa ( ):                                                        ┃\n";
                            cout << "┃ Uma bebida medicinal criada pelos Trogloditas, capaz de curar até os piores    ┃\n";
                            cout << "┃ ferimentos. Apesar dos Trogloditas não serem lá muito inteligentes, eles       ┃\n";
                            cout << "┃ possuem uma sabedoria nata para sobreviver até nos ambientes mais hostis, e    ┃\n";
                            cout << "┃ essa poção é um exemplo; criada através de alquimia, misturando cogumelos,     ┃\n";
                            cout << "┃ partes de Slimes e outros ingredientes, para criar uma bebida que deixa        ┃\n";
                            cout << "┃ diversos médicos no chinelo.                                                   ┃\n";
                            cout << "┃ (Recupera 'X' de vida do jogador).                                             ┃\n";
                            new_line("┣","━","┫",26);
                            cout << "┃ Anel Encantado ( ):                                                            ┃\n";
                            cout << "┃ Uma joia antiga gravada com runas atemporais, que parecem mudar de forma a     ┃\n";
                            cout << "┃ cada olhar, um dos poucos itens mágicos que ainda sobrevivem nos dias de hoje. ┃\n";
                            cout << "┃ (Concede um bônus em um atributo aleatório ao jogador).                        ┃\n";
                            new_line("┣","━","┫",26);
                            cout << "┃ Poção de Teleporte ( ):                                                        ┃\n";
                            cout << "┃ Outro exemplo das incríveis habilidades de alquimia dos Trogloditas, um        ┃\n";
                            cout << "┃ líquido violeta e instável produzido utilizando secreções de Slime e ferro     ┃\n";
                            cout << "┃ derretido. Não se sabe exatamente os detalhes do procedimento e funcionamento  ┃\n";
                            cout << "┃ da poção; é especulado que eles utilizem algum ingrediente mágico secreto.     ┃\n";
                            cout << "┃ (Ao ser consumida, teleporta o jogador para um local aleatório do mapa).       ┃\n";
                            new_line("┣","━","┫",26);
                            cout << "┃ Escudo de Batalha ( ):                                                         ┃\n";
                            cout << "┃ De tamanho médio, feito com metal resistente e escamas de guerreiros de elite  ┃\n";
                            cout << "┃ Kobolds, é incrivelmente resistente a ataques perfurantes e contundentes, e    ┃\n";
                            cout << "┃ com certeza protegerá o usuário de qualquer ataque.                            ┃\n";
                            cout << "┃ (Equipável. Aumenta a Armadura do jogador em 'X').                             ┃\n";
                            new_line("┗","━","┛",26);
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
                new_line("┏","━","┓",80);
                cout << "┃ CONTEXTO:                                                                      ┃\n";
                cout << "┃ - No mundo de [TEMPLATE], existe um arquipélago composto por três continentes: ┃\n";
                cout << "┃   os Reinos do Oeste, o reino dos humanos; os Reinos do Norte, o reino dos     ┃\n";
                cout << "┃   anões; e as Terras Selvagens, um continente localizado a leste, onde vivem   ┃\n";
                cout << "┃   diversas raças tradicionalmente hostis tanto aos humanos quanto aos anões,   ┃\n";
                cout << "┃   como goblins, kobolds, orcs, ogros, trolls e trogloditas. Ao longo da        ┃\n";
                cout << "┃   história, inúmeros conflitos ocorreram entre os três continentes. Na última  ┃\n";
                cout << "┃   grande guerra, uma aliança entre humanos e anões devastou quase              ┃\n";
                cout << "┃   completamente as Terras Selvagens, e quase todas as raças que ali viviam     ┃\n";
                cout << "┃   foram forçadas a se submeter aos termos impostos pelas outras duas nações.   ┃\n";
                cout << "┃   Vivendo sob constante repressão e obrigadas a trabalhar para os dois         ┃\n";
                cout << "┃   continentes vencedores, as antigas Terras Selvagens passaram a ser           ┃\n";
                cout << "┃   conhecidas como Terras do Fogo e Ferro, devido às inúmeras operações de      ┃\n";
                cout << "┃   mineração e metalurgia que hoje dominam a região.                            ┃\n";
                new_line("┣","━","┫",80);
                cout << "┃ A REBELIÃO DA BIGORNA E ASCENSÃO DA HORDA DE AÇO:                              ┃\n";
                cout << "┃ - Um certo kobold, cujo nome foi perdido ao longo da história, lutou na última ┃\n";
                cout << "┃   guerra e encontrou alguns livros pertencentes a uma caravana humana que      ┃\n";
                cout << "┃   havia sido emboscada em uma floresta. Esses livros narravam a história de    ┃\n";
                cout << "┃   impérios e grandes líderes humanos e, por coincidência, também continham     ┃\n";
                cout << "┃   conhecimentos sobre táticas e organização militar, técnicas de fundição e    ┃\n";
                cout << "┃   forja, além de engenharia, física e outros campos da ciência. Fascinado por  ┃\n";
                cout << "┃   esse conhecimento, ele estudava secretamente esses livros, acreditando que   ┃\n";
                cout << "┃   poderiam ajudar no esforço de guerra dos kobolds. Naquela época, as raças    ┃\n";
                cout << "┃   das Terras Selvagens não estavam unidas; cada povo lutava por si próprio.    ┃\n";
                cout << "┃   Posteriormente, com a derrota e a quase escravização de seu povo e das       ┃\n";
                cout << "┃   demais raças do continente, esse kobold adotou o nome Victor, uma palavra    ┃\n";
                cout << "┃   de uma das muitas línguas humanas que significa 'Conquistador'. Desde então, ┃\n";
                cout << "┃   passou a planejar uma forma de libertar o continente e se vingar do que      ┃\n";
                cout << "┃   humanos e anões haviam feito com ele e com seu povo. Utilizando o            ┃\n";
                cout << "┃   conhecimento adquirido nos livros e equipado com um braço mecânico feito de  ┃\n";
                cout << "┃   aço negro (na qual perdeu nas forjas), Victor começou a criar uma aliança    ┃\n";
                cout << "┃   secreta entre as raças oprimidas das Terras de Fogo e Ferro. Esse movimento  ┃\n";
                cout << "┃   culminaria na Rebelião da Bigorna, durante a qual a recém-formada Horda de   ┃\n";
                cout << "┃   Aço, empunhando armas de qualidade incrível e dotada de uma organização      ┃\n";
                cout << "┃   militar eficiente, expulsou humanos e anões do continente e iniciou os       ┃\n";
                cout << "┃   preparativos para uma força de invasão destinada a esmagar os reinos dos     ┃\n";
                cout << "┃   homens e dos anões. Durante a Rebelião, diversos humanos foram capturados e  ┃\n";
                cout << "┃   submetidos a experimentos na tentativa de fundir matéria biológica com       ┃\n";
                cout << "┃   componentes mecânicos. Desses experimentos surgiu a Escória: humanos         ┃\n";
                cout << "┃   terrivelmente deformados e mantidos sob o controle da Horda de Aço,          ┃\n";
                cout << "┃   servindo como trabalhadores ou tropas descartáveis, porém extremamente       ┃\n";
                cout << "┃   perigosas.                                                                   ┃\n";
                new_line("┣","━","┫",80);
                cout << "┃ VOCÊ:                                                                          ┃\n";
                cout << "┃ - Você foi um dos soldados que tentou resistir desesperadamente contra as      ┃\n";
                cout << "┃   investidas da Horda de Aço, mas foi capturado, e teve o mesmo destino de se  ┃\n";
                cout << "┃   tornar um Escória, e pior ainda, você foi descartado, apresentando sinais    ┃\n";
                cout << "┃   de mau funcionamento, e deixado para apodrecer em uma vala...                ┃\n";
                cout << "┃   Mas, o destino é uma coisa engraçada: Você acorda na entrada de uma das      ┃\n";
                cout << "┃   muitas forjas e fundições subterrâneas da Horda de Aço, de alguma forma,     ┃\n";
                cout << "┃   você sobreviveu, e ainda consegue lutar, e agora, cabe a você deter os       ┃\n";
                cout << "┃   planos da Horda nessa fundição, quais armas terríveis eles estão criando     ┃\n";
                cout << "┃   aqui? E você?                                                                ┃\n";
                cout << "┃   Pelo o que lutará?                                                           ┃\n";
                cout << "┃   Vingança? Dever? Honra? Ouro? Glória?                                        ┃\n";
                cout << "┃   Isso, é você quem decide...                                                  ┃\n";
                new_line("┗","━","┛",80);
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

void move_monsters(GAME &game){
    int blocks[4] = {FREEBLOCK,STAIRBLOCK,KEYBLOCK,TRAPBLOCK};
    for(int monster=0;monster<50;monster++){
        if(game.map.tiles[game.map.monsters[monster].pos.Y][game.map.monsters[monster].pos.X]==KEYBLOCK){
            game.map.tiles[game.map.monsters[monster].pos.Y][game.map.monsters[monster].pos.X] = FREEBLOCK;
            game.map.monsters[monster].key = true;
        }
        if(game.map.monsters[monster].attributes.hp<1){
            if(game.map.monsters[monster].alive==true){
                if(game.map.monsters[monster].id==0){
                    game.map.monsters[monster].id = 1;
                }
                game.map.player.exp += rand()%game.map.monsters[monster].id+1;
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
                if(rand()%2==0){
                    direction = -1;
                }
                bool success = true;
                for(int block=0;block<4;block++){
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
                for(int otherMonster=0;otherMonster<50;otherMonster++){
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

void move_player(GAME &game){
    cout<<"\e[23;1H";
    if(game.map.player.attributes.hp<1){
        if(game.map.player.inventory[0][2].id==9){
            game.map.player.attributes.hp = 1;
            while(true){
                int posY = rand()%MAPSIZEY;
                int posX = rand()%MAPSIZEX;
                if(game.map.tiles[posY][posX]==FREEBLOCK){
                    game.map.player.pos.Y = posY;
                    game.map.player.pos.X = posX;
                    break;
                }
            }
            game.map.player.inventory[0][2].id = 0;
            return;
        }
        cout<<"MORREU";
        game.map.player.clockSpeed = clock();
        while((clock()-game.map.player.clockSpeed)<3000){}
        cout<<"\ec";
        game.play = false;
        return;
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
        return;
    }
    if(kbhit()){
        game.map.player.keyInput = getch();
        if((clock()-game.map.player.clockSpeed)>1000/game.map.player.attributes.dexterity){
            cout<<"\e[2K";
            if(game.map.player.exp>=game.map.player.nivel){
                game.map.player.exp = 0;
                game.map.player.attPoints++;
                game.map.player.nivel++;
            }
            game.map.player.clockSpeed = clock();
            POS targetPos = {0,0};
            int blocks[5] = {FREEBLOCK,STAIRBLOCK,EMPTY,KEYBLOCK,TRAPBLOCK};
            bool success = true;
            for(int block=0;block<5;block++){
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
                    if(!game.map.player.inventoryOpened){
                        bool haveSpace = false;
                        for(int y=1;y<3;y++){
                            for(int x=0;x<3;x++){
                                if(game.map.player.inventory[y][x].id==0){
                                    haveSpace = true;
                                }
                            }
                        }
                        if(haveSpace = true){
                            for(int item=0;item<10;item++){
                                if(game.map.items[item].Y==game.map.player.pos.Y && game.map.items[item].X==game.map.player.pos.X){
                                    game.map.items[item].Y = -1;
                                    game.map.items[item].Y = -1;
                                    for(int y=1;y<3;y++){
                                        for(int x=0;x<3;x++){
                                            if(game.map.player.inventory[y][x].id==0){
                                                game.map.player.inventory[y][x].id = rand()%10+1;
                                                game.map.player.inventory[y][x].heal = 0;
                                                game.map.player.inventory[y][x].hp = 0;
                                                game.map.player.inventory[y][x].strength = 0;
                                                game.map.player.inventory[y][x].defense = 0;
                                                game.map.player.inventory[y][x].dexterity = 0;
                                                game.map.player.inventory[y][x].intelligence = 0;
                                                game.map.player.inventory[y][x].cursed = false;
                                                if(rand()%100==0){
                                                    game.map.player.inventory[y][x].cursed = true;
                                                }
                                                if(game.map.player.inventory[y][x].id==3){
                                                    int bonus = 1;
                                                    while(true){
                                                        if(rand()%2==0){
                                                            bonus++;
                                                        }else{
                                                            break;
                                                        }
                                                    }
                                                    int attribute = rand()%5;
                                                    if(attribute==0){
                                                        game.map.player.inventory[y][x].hp = rand()%game.map.floor+bonus;
                                                    }
                                                    if(attribute==1){
                                                        game.map.player.inventory[y][x].strength = rand()%game.map.floor+bonus;
                                                    }
                                                    if(attribute==2){
                                                        game.map.player.inventory[y][x].defense = rand()%game.map.floor+bonus;
                                                    }
                                                    if(attribute==3){
                                                        game.map.player.inventory[y][x].dexterity = rand()%game.map.floor+bonus;
                                                    }
                                                    if(attribute==4){
                                                        game.map.player.inventory[y][x].intelligence = rand()%game.map.floor+bonus;
                                                    }                                           
                                                }
                                                x = 3;
                                                y = 3;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X]==KEYBLOCK){
                            game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X] = FREEBLOCK;
                            game.map.player.key = true;
                        }
                        if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X]==STAIRBLOCK){
                            if(game.map.player.key){
                                game.next = true;
                                game.map.floor++;
                                cout<<"DESCENDO...";
                                game.map.player.clockSpeed = clock();
                                while((clock()-game.map.player.clockSpeed)<2000){}
                                return;
                            }
                        }
                    }
                }
                if(game.map.player.inventoryOpened){
                    if(game.map.player.inventorySelection.Y<0){
                        game.map.player.inventorySelection.Y = 0;
                    }
                    if(game.map.player.inventorySelection.Y>3){
                        game.map.player.inventorySelection.Y = 3;
                    }
                    if(game.map.player.inventorySelection.X<0){
                        game.map.player.inventorySelection.X = 0;
                    }
                    if(game.map.player.inventorySelection.X>2){
                        game.map.player.inventorySelection.X = 2;
                    }
                    if(game.map.player.keyInput==13){
                        if(game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id!=0){
                            bool haveSpace = false;
                            for(int y=1;y<3;y++){
                                for(int x=0;x<3;x++){
                                    if(game.map.player.inventory[y][x].id==0){
                                        haveSpace = true;
                                    }
                                }
                            }
                            if(game.map.player.inventorySelection.Y==0){
                                if(haveSpace){
                                    if(game.map.player.inventory[0][game.map.player.inventorySelection.X].id==3){
                                        game.map.player.attributes.hpMax-=game.map.player.inventory[0][game.map.player.inventorySelection.X].hp;
                                        game.map.player.attributes.defense-=game.map.player.inventory[0][game.map.player.inventorySelection.X].defense;
                                        game.map.player.attributes.strength-=game.map.player.inventory[0][game.map.player.inventorySelection.X].strength;
                                        game.map.player.attributes.intelligence-=game.map.player.inventory[0][game.map.player.inventorySelection.X].intelligence;
                                        game.map.player.attributes.dexterity-=game.map.player.inventory[0][game.map.player.inventorySelection.X].dexterity;
                                    }
                                    for(int y=1;y<3;y++){
                                        for(int x=0;x<3;x++){
                                            if(game.map.player.inventory[y][x].id==0){
                                                game.map.player.inventory[y][x].id = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id;
                                                game.map.player.inventory[y][x].defense = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].defense;
                                                game.map.player.inventory[y][x].dexterity = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].dexterity;
                                                game.map.player.inventory[y][x].heal = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].heal;
                                                game.map.player.inventory[y][x].hp = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].hp;
                                                game.map.player.inventory[y][x].intelligence = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].intelligence;
                                                game.map.player.inventory[y][x].strength = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].strength;
                                                x = 3;
                                                y = 3;
                                            }
                                        }
                                    }
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].defense = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].dexterity = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].heal = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].hp = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].intelligence = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].strength = 0;
                                }
                            }
                            bool equip = false;
                            int x = game.map.player.inventorySelection.X;
                            if(game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==1 || game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==4 || game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==5){
                                equip = true;
                                x = 0;
                            }
                            if(game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==2 || game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==8){
                                equip = true;
                                x = 1;
                            }
                            if(game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==3 || game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==9){
                                equip = true;
                                x = 2;
                            }
                            if(equip){
                                if(game.map.player.inventory[0][x].id==0){
                                    game.map.player.inventory[0][x].id = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id;
                                    game.map.player.inventory[0][x].defense = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].defense;
                                    game.map.player.inventory[0][x].dexterity = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].dexterity;
                                    game.map.player.inventory[0][x].heal = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].heal;
                                    game.map.player.inventory[0][x].hp = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].hp;
                                    game.map.player.inventory[0][x].intelligence = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].intelligence;
                                    game.map.player.inventory[0][x].strength = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].strength;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].defense = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].dexterity = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].heal = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].hp = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].intelligence = 0;
                                    game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].strength = 0;
                                    if(game.map.player.inventory[0][x].id==3){
                                        game.map.player.attributes.hpMax+=game.map.player.inventory[0][x].hp;
                                        game.map.player.attributes.defense+=game.map.player.inventory[0][x].defense;
                                        game.map.player.attributes.strength+=game.map.player.inventory[0][x].strength;
                                        game.map.player.attributes.intelligence+=game.map.player.inventory[0][x].intelligence;
                                        game.map.player.attributes.dexterity+=game.map.player.inventory[0][x].dexterity;
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            }
            if(!game.map.player.inventoryOpened){
                for(int monster=0;monster<50;monster++){
                    if(game.map.monsters[monster].pos.Y==game.map.player.pos.Y+targetPos.Y && game.map.monsters[monster].pos.X==game.map.player.pos.X+targetPos.X){
                        game.map.monsters[monster].attacked = true;
                        int attack = rand()%game.map.player.attributes.strength+1;
                        if(game.map.player.inventory[0][0].id==1){
                            if(rand()%4!=0){
                                attack += rand()%game.map.player.attributes.strength;
                            }
                            if(rand()%100==0){
                                game.map.player.inventory[0][0].id==0;
                            }
                        }
                        if(game.map.player.inventory[0][0].id==4){
                            if(rand()%2==0){
                                attack += rand()%game.map.player.attributes.dexterity;
                                while(true){
                                    if(rand()%2==0){
                                        attack++;
                                    }else{
                                        break;
                                    }
                                }
                                if(rand()%50==0){
                                    game.map.player.inventory[0][0].id==0;
                                }
                            }
                        }
                        if(game.map.player.inventory[0][0].id==5){
                            if(rand()%2==0){
                                attack += rand()%game.map.player.attributes.intelligence+1;
                                if(rand()%2==0){
                                    if(rand()%2==0){
                                        attack += rand()%game.map.player.attributes.intelligence+1;
                                    }else{
                                        attack -= rand()%game.map.player.attributes.intelligence;
                                    }
                                }
                            }
                            if(rand()%50==0){
                                game.map.player.inventory[0][0].id==0;
                            }
                        }
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
                    if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X] == TRAPBLOCK){
                        game.map.player.attributes.hp -= game.map.floor;
                        game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X] = FREEBLOCK;
                    }
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
    if(game.map.player.attPoints>0){
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
}

void create_map(GAME &game){
    if(game.next==true){
        game.map.player.attPoints++;
        while(true){
            if(rand()%2==0){
                game.map.player.attPoints++;
            }else{
                break;
            }
        }
    }
    game.next = false;
    game.map.player.key = false;
    while(true){
        if(!game.map.player.fallen){
            put_attributes(game);
            if(game.map.player.attPoints<=0){
                put_attributes(game);
                break;
            }
        }else{
            game.map.player.fallen = false;
            break;
        }
    }
    game.map.player.clockSpeed = clock();
    while((clock()-game.map.player.clockSpeed)<1000){}
    for(int y=0;y<MAPSIZEY;y++){
        for(int x=0;x<MAPSIZEX;x++){
            game.map.tiles[y][x] = EMPTY;
            game.map.memory[y][x] = 0;
            if(y==0 || x==0 || y==MAPSIZEY-1 || x==MAPSIZEX-1){
                game.map.tiles[y][x] = SOLIDBLOCK;
            }
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
            if(Y>=1 && Y<MAPSIZEY-1 && X>=1 && X<MAPSIZEX-1){
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
    for(int monster=0;monster<50;monster++){
        MONSTER resetMonsters[50];
        game.map.monsters[monster] = resetMonsters[monster];
    }
    for(int monster=0;monster<50;monster++){
        if(rand()%(11-game.map.floor)==0 || monster==0){
            int posY;
            int posX;
            bool success = false;
            int attPoints = (game.map.player.nivel*2)+(game.map.floor);
            game.map.monsters[monster].id = rand()%game.map.floor;
            while(attPoints>0){
                int attribute = rand()%5;
                if(rand()%4==0){
                    attPoints--;
                    continue;
                }
                if(game.map.monsters[monster].id==1){ // GOBLIN
                    if(rand()%2==0){
                        attribute = 4;
                    }
                }
                if(game.map.monsters[monster].id==2){ // KOBOLD
                    if(rand()%2==0){
                        attribute = 4;
                    }
                    if(rand()%2==0){
                        attribute = 3;
                    }
                }
                if(game.map.monsters[monster].id==3){ // OCRS
                    if(rand()%2==0){
                        attribute = 2;
                    }
                }
                if(game.map.monsters[monster].id==4){ // OGROS
                    if(rand()%2==0){
                        attribute = 2;
                    }
                }
                if(game.map.monsters[monster].id==5){ // TROLLS
                    if(rand()%2==0){
                        attribute = 2;
                    }
                    if(rand()%2==0){
                        attribute = 3;
                    }
                }
                if(game.map.monsters[monster].id==6){ // MIMICO
                    if(rand()%10==0){
                        attPoints++;
                        continue;
                    }
                    if(rand()%2==0){
                        attribute = 1;
                    }
                }
                if(game.map.monsters[monster].id==7){ // TROGLODITA
                    if(rand()%2==0){
                        attribute = 2;
                    }
                    if(rand()%2==0){
                        attribute = 4;
                    }
                }
                if(game.map.monsters[monster].id==8){ // AUTONOMOS
                    if(rand()%2==0){
                        attribute = 2;
                    }
                    if(rand()%2==0){
                        attribute = 4;
                    }
                    if(rand()%2==0){
                        attribute = 1;
                    }
                }
                if(game.map.monsters[monster].id==9){ // ESCORIAS
                    if(rand()%2==0){
                        attribute = 2;
                    }
                    if(rand()%2==0){
                        attribute = 4;
                    }
                }
                if(game.map.monsters[monster].id==10){ // BOSS
                    if(rand()%2==0){
                        attPoints++;
                        continue;
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
                if(game.map.tiles[posY][posX]==FREEBLOCK){
                    success = true;
                    break;
                }
                for(int otherMonster=0;otherMonster<50;otherMonster++){
                    if(otherMonster==monster){
                        continue;
                    }
                    if(game.map.monsters[monster].pos.Y == game.map.monsters[otherMonster].pos.Y && game.map.monsters[monster].pos.X == game.map.monsters[otherMonster].pos.X){
                        success = false;
                    }
                }
            }
            if(success){
                game.map.monsters[monster].pos.Y = posY;
                game.map.monsters[monster].pos.X = posX;
                game.map.monsters[monster].attributes.hp = game.map.monsters[monster].attributes.hpMax;
                game.map.monsters[monster].alive = true;
            }
        }else{
            continue;
        }
    }
    for(int item=0;item<10;item++){
        game.map.items[item].Y = -1;
        game.map.items[item].X = -1;
    }
    for(int item=0;item<10;item++){
        if(rand()%10==0 || item==0){
            while(true){
                int posY = rand()%MAPSIZEY;
                int posX = rand()%MAPSIZEX;
                game.map.items[item].Y = posY;
                game.map.items[item].X = posX;
                bool success = true;
                for(int otherItem=0;otherItem<10;otherItem++){
                    if(otherItem==item){
                        continue;
                    }
                    if(game.map.items[item].Y==game.map.items[otherItem].Y && game.map.items[item].X==game.map.items[otherItem].X){
                        success = false;
                        break;
                    }
                }
                if(success){
                    if(game.map.tiles[posY][posX]==FREEBLOCK){
                        break;
                    }
                }else{
                    game.map.items[item].Y = -1;
                    game.map.items[item].X = -1;
                }
            }
        }
    }
    // --- GERADOR DE ARMADILHAS ANTI STUN LOCK (ANTI-LOCK / TIMEOUT) ---
    int qtdArmadilhas = rand()%game.map.floor+1;
    int armadilhasCriadas = 0;

    while(armadilhasCriadas < qtdArmadilhas){
        int tentativas = 0; // Sistema-Timeout integrado (impedir que crashe se n conseguir achar espaço pras armadilhas)
        bool colocou = false;

        while(tentativas < 1000){
            int posY = rand()%MAPSIZEY;
            int posX = rand()%MAPSIZEX;

            if(game.map.tiles[posY][posX] == FREEBLOCK){
                bool livre = (
                    game.map.tiles[posY-1][posX] == FREEBLOCK &&
                    game.map.tiles[posY+1][posX] == FREEBLOCK &&
                    game.map.tiles[posY][posX-1] == FREEBLOCK &&
                    game.map.tiles[posY][posX+1] == FREEBLOCK &&

                    game.map.tiles[posY-1][posX+1] == FREEBLOCK &&
                    game.map.tiles[posY+1][posX-1] == FREEBLOCK &&
                    game.map.tiles[posY-1][posX-1] == FREEBLOCK &&
                    game.map.tiles[posY+1][posX+1] == FREEBLOCK
                );

                if(livre){
                    game.map.tiles[posY][posX] = TRAPBLOCK;
                    armadilhasCriadas++;
                    colocou = true;
                    break;
                }
            }
            tentativas++;
        }
        // Se após 1000 tentativas o mapa atual não tiver espaço grande o bastante,
        // quebra o loop principal para impedir que o jogo trave infinitamente.
        if(!colocou){
            armadilhasCriadas++;
            continue;
        }
    }
}

void show_inventory(GAME &game){
    cout << "\e[?25l\e[H";
    cout << "\e[1;1H";
    new_line("┏","━","┓",3*3);
    for(int y=0;y<4;y++){
        if(y==1){
            new_line("┣","━","┫",3*3);
        }
        cout<<"┃";
        for(int x=0;x<3;x++){
            cout<<" ";
            if(game.map.player.inventory[y][x].cursed){
                cout<<"\e[38;5;9m";
            }
            if(y==game.map.player.inventorySelection.Y && x==game.map.player.inventorySelection.X){
                if(game.map.player.inventory[y][x].cursed){
                    cout<<"\e[38;5;9m\e[48;5;237m";
                }else{
                    cout<<"\e[38;5;255m\e[48;5;237m";
                }
            }
            if(game.map.player.inventory[y][x].id==0){
                cout<<" ";
            }
            if(game.map.player.inventory[y][x].id==1){
                cout<<"󰓥"; // ESPADA
            }
            if(game.map.player.inventory[y][x].id==2){
                cout<<"󰒘"; // ESCUDO
            }
            if(game.map.player.inventory[y][x].id==3){
                cout<<""; // ANEL
            }
            if(game.map.player.inventory[y][x].id==4){
                cout<<"󰧼"; // ADAGA
            }
            if(game.map.player.inventory[y][x].id==5){
                cout<<"󱡄"; // CAJADO
            }
            if(game.map.player.inventory[y][x].id==6){
                cout<<"󱄰"; // POÇÃO DE CURA 
            }
            if(game.map.player.inventory[y][x].id==7){
                cout<<"󱄮"; // POÇÃO MISTERIOSA
            }
            if(game.map.player.inventory[y][x].id==8){
                cout<<"󰂪"; // ESCUDO REFLETOR
            }
            if(game.map.player.inventory[y][x].id==9){
                cout<<""; // TOTEM
            }
            if(game.map.player.inventory[y][x].id==10){
                cout<<""; // PERGAMINHO
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
            if(y<0){
                y--;
            }else{
                y++;
            }
        }
        if(rand()%2==0){
            if(x<0){
                x--;
            }else{
                x++;
            }
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
    if(game.map.player.attributes.hp<0){
        game.map.player.attributes.hp = 0;
    }
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
                if(game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]==TRAPBLOCK){
                    cout<<"\e[48;5;246m\e[38;5;196m󰝧";
                }
                if(game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]==KEYBLOCK){
                    cout<<"\e[48;5;246m\e[38;5;3m";
                }
                for(int item=0;item<10;item++){
                    if(game.map.items[item].Y==game.map.player.pos.Y+y && game.map.items[item].X==game.map.player.pos.X+x){
                        cout<<"\e[1D\e[38;5;13m󰜦";
                    }
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
                            if(game.map.monsters[monster].attacked==true){
                                cout<<"\e[48;5;1m";
                                game.map.monsters[monster].attacked = false;
                            }
                            if(game.map.monsters[monster].id==0){ // SLIME
                                cout<<"\e[38;5;31m󰛹";
                            }
                            if(game.map.monsters[monster].id==1){ // GOBLIN
                                cout<<"\e[38;5;46m";
                            }
                            if(game.map.monsters[monster].id==2){ // KOBOLD
                                cout<<"\e[38;5;202m";
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
                                cout<<"\e[38;5;13m󰜦";
                            }
                            if(game.map.monsters[monster].id==7){ // TROGLODITA
                                cout<<"\e[38;5;227m";
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
                            cout<<"\e[0m";
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
    if(((game.map.player.attributes.hp/game.map.player.attributes.hpMax)*10)<0){
        cout<<"\e[48;5;40m ";
    }
    for(bar=bar;bar<10;bar++){
        cout<<"\e[0m ";
    }
    cout<<"\e[0m┃ "<<game.map.player.attributes.hp<<"/"<<game.map.player.attributes.hpMax;
    cout<<"\e[3;"<<((vision+1)*2)+1<<"H";
    new_line("┗","━","┛",10);
    cout<<"\e[4;"<<((vision+1)*2)+1<<"H";
    new_line("┏","━","┓",3*3);
    cout<<"\e[5;"<<((vision+1)*2)+1<<"H┃";
    for(int x=0;x<3;x++){
        cout<<" ";
        if(game.map.player.inventory[0][x].cursed){
            cout<<"\e[38;5;9m";
        }
        if(game.map.player.inventory[0][x].id==0){
            cout<<" ";
        }
        if(game.map.player.inventory[0][x].id==1){
            cout<<"󰓥"; // ESPADA
        }
        if(game.map.player.inventory[0][x].id==2){
            cout<<"󰒘"; // ESCUDO
        }
        if(game.map.player.inventory[0][x].id==3){
            cout<<""; // ANEL
        }
        if(game.map.player.inventory[0][x].id==4){
            cout<<"󰧼"; // ADAGA
        }
        if(game.map.player.inventory[0][x].id==5){
            cout<<"󱡄"; // CAJADO
        }
        if(game.map.player.inventory[0][x].id==8){
            cout<<"󰂪"; // ESCUDO REFLETOR
        }
        if(game.map.player.inventory[0][x].id==9){
            cout<<""; // TOTEM
        }
        cout<<"\e[0m ";
    }
    cout<<"\e[0m┃\n";
    cout<<"\e[6;"<<((vision+1)*2)+1<<"H";
    new_line("┗","━","┛",3*3);
    cout<<"\e[7;"<<((vision+1)*2)+1<<"H";
    cout<<"ANDAR: "<<game.map.floor;
    cout<<"\e[8;"<<((vision+1)*2)+1<<"H";
    cout<<"NIVEL: "<<game.map.player.nivel;
    cout<<"\e[9;"<<((vision+1)*2)+1<<"H";
    cout<<"EXP: "<<game.map.player.exp<<"/"<<game.map.player.nivel<<"     ";
    if(game.map.player.key){
        cout<<"\e[10;"<<((vision+1)*2)+1<<"H";
        cout<<"\e[38;5;3m\e[0m";
    }
}