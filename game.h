#include <iostream>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <string>
#include <fstream>
#include <SFML/Audio.hpp>
#include <map>

#define MAPSIZEX 500
#define MAPSIZEY 500

#define EMPTY 0
#define FREEBLOCK 1
#define SOLIDBLOCK 2
#define STAIRBLOCK 3
#define KEYBLOCK 4
#define TRAPBLOCK 5 // definição para a trap
#define NPCBLOCK 6
#define DOORBLOCK 7

using namespace std;
using namespace sf;

inline map<string, SoundBuffer> gameBuffers;
inline map<string, Sound*> gameSounds;

inline void playSound(string nomeSom, string caminhoArquivo) {
    if (gameBuffers.find(nomeSom) == gameBuffers.end()) {
        if (!gameBuffers[nomeSom].loadFromFile(caminhoArquivo)) {
            cout << "ERRO: Não foi possível carregar o arquivo: " << caminhoArquivo << endl;
            return;
        }
        gameSounds[nomeSom] = new Sound(gameBuffers[nomeSom]);
    }
    gameSounds[nomeSom]->play();
}

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
    int gold = 0;
    clock_t timerStart;
    clock_t tempoPausado = 0;
    string name;
    int exp = 0;
    int nextExp = 2;
    int keyInput = 0;
    int clockSpeed = clock();
    bool alive = true;
    bool inventoryOpened = false;
    bool fallen = false;
    bool key = false;
    bool firstAtt = true;
    ITEM inventory[4][3];
    POS pos;
    POS inventorySelection = {0,0};
    ATTRIBUTES attributes;
};

struct MONSTER{
    int id = 0;
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
    int optionHorizontal = 1;
    bool menuDificuldade = true;
};

struct GAME{
    int attSelection = 0;
    bool exit = false;
    bool play = false;
    bool pause = false;
    bool pauseTudo = false;
    clock_t pauseTudoClock;
    bool next = false;
    bool codex = false;
    int difficulty = 0;
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

void mostrar_pontos(){ // Mostra os pontos (código reutilizado do Bomberman, e made by Nixillya)
    int pointsMax = 0;
    string ignore;
    ifstream file;

    // Conta quantos pontos tem no arquivo para definir o tamanho dos arrays
    file.open("Score.txt");

    string nome;
    int ponto;

    pointsMax = 0;

    if(file.is_open()){
        while(file >> nome >> ponto){
            pointsMax++;
        }
    }

    file.close();

    string nomes[pointsMax];
    int pontos[pointsMax];

    // Lê o arquivo novamente, mas dessa vez armazena os pontos e os nomes em arrays para depois ordenar e mostrar na tela
    file.open("Score.txt");
    if(file.is_open()){
        int j = 0;

        while(file >> nomes[j] >> pontos[j]){
            j++;
        }
    }
    file.close();

    // Ordena os pontos e os nomes, ele procura o maior ponto e coloca na posição correta, depois repete o processo para o próximo ponto até ordenar todoss os pontos
    for(int id=0; id<pointsMax; id++){
        int idAlto = id;
        int maiorPonto = pontos[id];
        for(int i=id; i<pointsMax; i++){
            if(pontos[i]>maiorPonto){
                maiorPonto = pontos[i];
                idAlto = i;
            }
        }
        if(idAlto<pointsMax){
            int auxPonto = pontos[idAlto];
            string auxNome = nomes[idAlto];
            pontos[idAlto] = pontos[id];
            nomes[idAlto] = nomes[id];
            pontos[id] = auxPonto;
            nomes[id] = auxNome;
        }
    }
    cout<<"\e[10;1H";
    cout<<"\nPontuações:"<<endl<<endl;
    if(pointsMax==0){
        cout<<"    Nenhuma pontuação registrada"<<endl;
    }
    for(int i=0; i<pointsMax; i++){
        cout<<"    "<<i+1<<" - "<<nomes[i] <<"   \e[38;5;46m"<<pontos[i]<<"\e[0m\n";
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void menu_render(GAME &game){
    game.menu.menuDificuldade = true;
    cout << "\e[?25l\e[H";
    new_line("┏","━","┓",16);
    new_line("┃   ","ROGUE-LIKE","   ┃",1);
    new_line("┣","━","┫",16);
    if (game.menu.optionVertical == 4) {
        cout << "┃ \e[93m> [JOGAR]\e[0m      ┃\n";
    } else {
        cout << "┃ [JOGAR]        ┃\n";
    }
    if (game.menu.optionVertical == 5) {
        cout << "┃ \e[93m> [CODEX]\e[0m      ┃\n";
    } else {
        cout << "┃ [CODEX]        ┃\n";
    }
    if (game.menu.optionVertical == 6) {
        cout << "┃ \e[93m> [CRÉDITOS]\e[0m   ┃\n";
    } else {
        cout << "┃ [CRÉDITOS]     ┃\n";
    }
    if (game.menu.optionVertical == 7) {
        cout << "┃ \e[93m> [RANK]\e[0m       ┃\n";
    } else {
        cout << "┃ [RANK]         ┃\n";
    }
    if (game.menu.optionVertical == 8) {
        cout << "┃ \e[93m> [SAIR]\e[0m       ┃\n";
    } else {
        cout << "┃ [SAIR]         ┃\n";
    }
    new_line("┗","━","┛",16);

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
        case 13: // Input (ENTER)
            if (game.menu.optionVertical == 4) { // JOGAR
                cout << "\e[9;1H";
                cout << "\n\n Nome: ";
                cin >> game.map.player.name;
                cout << "\ec";
                game.menu.optionVertical = 1;
                while (game.menu.menuDificuldade == true) {
                    cout << "\ec";
                    cout << "\e[?25l";
                    cout << "\e[1;18H"; // Precisa arrumar isso depois
                    cout << "\e[?25l\e[1;18H";
                    new_line("┏","━","┓",24);
                    if (game.menu.optionVertical == 1) {
                        cout << "┃\e[93m  [Dificuldade Facil]   \e[0m┃\n";
                    } else {
                        cout << "┃ [Dificuldade Facil]    ┃\n";
                    }
                    if (game.menu.optionVertical == 2) {
                        cout << "┃\e[93m  [Dificuldade Medía]   \e[0m┃\n";
                    } else {
                        cout << "┃ [Dificuldade Medía]    ┃\n";
                    }
                    if (game.menu.optionVertical == 3) {
                        cout << "┃\e[93m  [Dificuldade Dificil] \e[0m┃\n";
                    } else {
                        cout << "┃ [Dificuldade Dificil]  ┃\n";
                    }
                    new_line("┗","━","┛",24);
                    cout << "\e[?25l\e[1;18H";
                    int key = getch();
                    switch(key) {
                        case 119: // Cima
                        game.menu.optionVertical--;
                        if (game.menu.optionVertical < 1) {
                            game.menu.optionVertical = 3;
                        }
                        break;

                        case 115: // Baixo
                        game.menu.optionVertical++;
                        if (game.menu.optionVertical > 3) {
                            game.menu.optionVertical = 1;
                        }
                        break;

                        case 13: // Imput
                        if (game.menu.optionVertical == 1) {
                            game.map.player.timerStart = clock();
                            game.play = true;
                            game.difficulty = 1;
                            game.menu.menuDificuldade = false;
                        }
                        if (game.menu.optionVertical == 2) {
                            game.map.player.timerStart = clock();
                            game.play = true;
                            game.difficulty = 2;
                            game.menu.menuDificuldade = false;
                        }
                        if (game.menu.optionVertical == 3) {
                            game.map.player.timerStart = clock();
                            game.play = true;
                            game.difficulty = 3;
                            game.menu.menuDificuldade = false;
                        }
                    }
                }
            }
            if (game.menu.optionVertical == 5) {
                cout << "\ec";
                game.codex = true;
            }
            if (game.menu.optionVertical == 6) {
                cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓";
                cout << "\n┃        Universidade do Vale do Itajaí - UNIVALI    ┃";
                cout << "\n┃             Escola Politícnica  - POLI             ┃";
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
                mostrar_pontos();
                getch();
                cout << "\ec";
            }

            if (game.menu.optionVertical == 8) {
                game.exit = true;
            }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void codex_render(GAME &game){
    bool codexPages = true;
    cout << "\e[?25l\e[H";
    new_line("┏","━","┓",15);
    new_line("┃     ","CODEX","     ┃",1);
    new_line("┣","━","┫",15);
    if (game.menu.optionVertical == 4) {
        cout << "┃ \e[93m> [COMO JOGAR]\e[0m┃\n";
    } else {
        cout << "┃ [COMO JOGAR]  ┃\n";
    }
    if (game.menu.optionVertical == 5) {
        cout << "┃ \e[93m> [ITENS]\e[0m     ┃\n";
    } else {
        cout << "┃ [ITENS]       ┃\n";
    }
    if (game.menu.optionVertical == 6) {
        cout << "┃ \e[93m> [BESTIÁRIO]\e[0m ┃\n";
    } else {
        cout << "┃ [BESTIÁRIO]   ┃\n";
    }
    if (game.menu.optionVertical == 7) {
        cout << "┃ \e[93m> [HISTÓRIA]\e[0m  ┃\n";
    } else {
        cout << "┃ [HISTÓRIA]    ┃\n";
    }
    if (game.menu.optionVertical == 8) {
        cout << "┃ \e[93m> [VOLTAR]\e[0m    ┃\n";
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
                cout << "┃ Controles (funcionam no inventário também):  ┃\n";
                cout << "┃                                              ┃\n";
                cout << "┃                Cima                          ┃\n";
                cout << "┃                 ^                            ┃\n";
                cout << "┃                 W                            ┃\n";
                cout << "┃    Esquerda < A S D > Direita                ┃\n";
                cout << "┃                 V                            ┃\n";
                cout << "┃               Baixo                          ┃\n";
                cout << "┃                                              ┃\n";
                new_line("┣","━","┫",46);
                cout << "┃ Iventário > I                                ┃\n";
                new_line("┣","━","┫",46);
                cout << "┃ Enter > Interagir com coisas.                ┃\n";
                new_line("┗","━","┛",46);
                cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ OBJETIVO ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃ - Sobreviva e lute pelos 11 andares, até chegar ao último andar e derrotar     ┃\n";
                cout << "┃  o Chefão.                                                                     ┃\n";
                cout << "┃ - Para descer de andar, você precisa de uma chave para abrir o alçapão da      ┃\n";
                cout << "┃  escada. A chave é carregada por um monstro aleatório do andar. Encontre-o     ┃\n";
                cout << "┃  e elimine-o para pegar a chave e abrir o alçapão.                             ┃\n";
                cout << "┃                                                                                ┃\n";
                cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━ GERAÇÃO DE ANDARES ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃ - Os andares são gerados aleatoriamente em todas as runs. Existem salas        ┃\n";
                cout << "┃  (locais cercados por paredes) e pontes. As áreas em preto representam o       ┃\n";
                cout << "┃  abismo. Cuidado ao andar pelas pontes, pois há uma chance de você cair.       ┃\n";
                cout << "┃ - Se tiver sorte, sobreviverá à queda para o próximo andar (e perderá uma      ┃\n";
                cout << "┃  quantidade considerável de vida). Porém, também existe a chance de morrer     ┃\n";
                cout << "┃  instantaneamente.                                                             ┃\n";
                cout << "┃ - Os andares possuem armadilhas (por cortesia dos Kobolds), o dano de uma      ┃\n";
                cout << "┃  é baseado no número do andar atual.                                           ┃\n";
                cout << "┃ - À medida que você adentra as profundezas da Fundição, o calor infernal da    ┃\n";
                cout << "┃  Forja ficará cada vez mais intenso. Cuidado para não cair no mar de ferro     ┃\n";
                cout << "┃  derretido do último andar...                                                  ┃\n";
                cout << "┃                                                                                ┃\n";
                cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ MONSTROS ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃ - Certos monstros possuem afinidades com certos atributos, e tem a chance de   ┃\n";
                cout << "┃   receberem um ponto de atributo com base na afinidade deles.                  ┃\n";
                cout << "┃ - Os monstros andam aleatóriamente pelos andares.                              ┃\n";
                cout << "┃                                                                                ┃\n";
                cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ PONTUAÇÃO ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃ - Derrotar monstros, descer de andares e abrir baús lhe dará pontuação,        ┃\n";
                cout << "┃   também conhecida como 'moedas de ouro'.                                      ┃\n";
                cout << "┃                                                                                ┃\n";
                cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ ATRIBUTOS E STATUS ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃ - O seu personagem possui 3 atributos e 3 status.                              ┃\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃ - ATRIBUTOS:                                                                   ┃\n";
                cout << "┃ - Força (Strength): Determina quanto dano você pode causar.                    ┃\n";
                cout << "┃ - Destreza (Dexterity): Determina sua velocidade de caminhada.                 ┃\n";
                cout << "┃ - Inteligência (Intelligence): Determina seu raio de visão no mapa.            ┃\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃ - STATUS:                                                                      ┃\n";
                cout << "┃ - HP (Pontos de Vida): Determina sua vida. Se chegar a 0, bem, você já         ┃\n";
                cout << "┃   sabe como é.                                                                 ┃\n";
                cout << "┃ - Defense (Defesa): Determina quanto dano você pode bloquear.                  ┃\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃ - Quando você inicia uma run, pode alocar 5 pontos em qualquer atributo. Ao    ┃\n";
                cout << "┃  subir de nível, porém, só poderá distribuir novos pontos ao descer para um    ┃\n";
                cout << "┃  novo andar.                                                                   ┃\n";
                cout << "┃                                                                                ┃\n";
                cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ ITENS E ARMAS ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n";
                cout << "┃                                                                                ┃\n";
                cout << "┃ - As armas possuem uma chance de realizar um efeito especial, e também possuem ┃\n";
                cout << "┃   uma pequena chance de quebrarem. Para equipar um item ou arma, abra o        ┃\n";
                cout << "┃   inventário, selecione a arma ou item, e aperte 'Enter' para equipa-lo, para  ┃\n";
                cout << "┃   desequipar uma arma ou item, selecione-o e aperte 'Enter'.                   ┃\n";
                cout << "┃ - Há uma chance de obter um item amaldiçoado. Itens amaldiçoados não podem ser ┃\n";
                cout << "┃   desequipados, a não ser que quebrem (todas as armas tem uma pequena chance   ┃\n";
                cout << "┃   de quebrarem).                                                               ┃\n";
                cout << "┃                                                                                ┃\n";
                new_line("┗","━","┛",80);
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
                        new_line("┏","━","┓",80);
                            cout << "┃  - Espada Longa (󰓥):                                                           ┃\n";
                            cout << "┃ Um lâmina forjada em aço pelos melhores orcs que a Horda pode oferecer,        ┃\n";
                            cout << "┃ pesada e implacável nas mãos de um guerreiro determinado.                      ┃\n";
                            cout << "┃ Tem 75% de somar um valor aleatório baseado na sua força +1, 4% de quebrar.    ┃\n";
                            new_line("┣","━","┫",80);
                            cout << "┃ - Adagas (󰧼):                                                                  ┃\n";
                            cout << "┃ Um par de lâminas que, na verdade, são presas de um troll, amoladas ao ponto   ┃\n";
                            cout << "┃ de cortarem facilmente até o menor toque. É uma tradição dos trolls cortarem   ┃\n";
                            cout << "┃ as próprias presas e criarem suas próprias adagas, como uma forma de provar    ┃\n";
                            cout << "┃ ser um guerreiro capaz e digno.                                                ┃\n";
                            cout << "┃ Lâminas gêmeas, leves e equilibradas, feitas a partir de presas de Troll,      ┃\n";
                            cout << "┃ perfeitas para cortes rápidos antes que o inimigo possa reagir.                ┃\n";
                            cout << "┃ Tem 50% de somar um valor aleatório baseado na sua destreza e entra num loop   ┃\n";
                            cout << "┃ de 50% +1 no dano e tem 2% de chance de quebrar ao fazer os dois.              ┃\n";
                            new_line("┣","━","┫",80);
                            cout << "┃ - Cajado Elétrico (🪄):                                                         ┃\n";
                            cout << "┃ Esculpido em madeira antiga, possui um Núcleo Laser de um Autônomo acoplado    ┃\n";
                            cout << "┃ em um capacitador, capaz de disparar feixes de energia pura concentrada. Um    ┃\n";
                            cout << "┃ protótipo que serve para demonstrar a engenharia e ambição dos Kobolds para    ┃\n";
                            cout << "┃ superar a tecnologia dos humanos e anões.                                      ┃\n";
                            cout << "┃ Tem 50% de somar um valor aleatório baseado na sua inteligência e fazer isso   ┃\n";
                            cout << "┃ novamente, mas, diminuindo ou somando, tem 2% de quebrar ao fazer os dois.     ┃\n";
                            new_line("┗","━","┛",80);
                        break;

                        case 2:
                            new_line("┏","━","┓",80);
                            cout << "┃ - Poção Regenerativa (󱄰):                                                      ┃\n";
                            cout << "┃ Uma bebida medicinal criada pelos Trogloditas, capaz de curar até os piores    ┃\n";
                            cout << "┃ ferimentos. Apesar dos Trogloditas não serem lá muito inteligentes, eles       ┃\n";
                            cout << "┃ possuem uma sabedoria nata para sobreviver até nos ambientes mais hostis, e    ┃\n";
                            cout << "┃ essa poção é um exemplo; criada através de alquimia, misturando cogumelos,     ┃\n";
                            cout << "┃ partes de Slimes e outros ingredientes, para criar uma bebida que deixa        ┃\n";
                            cout << "┃ diversos médicos no chinelo.                                                   ┃\n";
                            cout << "┃ Recupera uma quantidade de vida de acordo com número do anda onde foi obtida.  ┃\n";
                            new_line("┣","━","┫",80);
                            cout << "┃ - Anel Encantado ():                                                          ┃\n";
                            cout << "┃ Uma joia antiga gravada com runas atemporais, que parecem mudar de forma a     ┃\n";
                            cout << "┃ cada olhar, um dos poucos itens mágicos que ainda sobrevivem nos dias de hoje. ┃\n";
                            cout << "┃ Equipar o anel lhe concede +1 atributo ou status aleatório.                    ┃\n";
                            new_line("┣","━","┫",80);
                            cout << "┃ - Poção de Teleporte (󱄮):                                                      ┃\n";
                            cout << "┃ Outro exemplo das incríveis habilidades de alquimia dos Trogloditas, um        ┃\n";
                            cout << "┃ líquido violeta e instável produzido utilizando secreções de Slime e ferro     ┃\n";
                            cout << "┃ derretido. Não se sabe exatamente os detalhes do procedimento e funcionamento  ┃\n";
                            cout << "┃ da poção; é especulado que eles utilizem algum ingrediente mágico secreto.     ┃\n";
                            cout << "┃ (Ao ser consumida, teleporta o jogador para um local aleatório do mapa).       ┃\n";
                            new_line("┣","━","┫",80);
                            cout << "┃ - Escudo de Batalha (󰒘):                                                       ┃\n";
                            cout << "┃ De tamanho médio, feito com metal resistente e escamas de guerreiros de elite  ┃\n";
                            cout << "┃ Kobolds, é incrivelmente resistente a ataques perfurantes e contundentes, e    ┃\n";
                            cout << "┃ com certeza protegerá o usuário de qualquer ataque.                            ┃\n";
                            cout << "┃ 50% de somar um valor aleatório da sua defesa +1 e 5% quebrar ao fazer isso.   ┃\n";
                            new_line("┣","━","┫",80);
                            cout << "┃ - Escudo de Defletor (󰂪):                                                      ┃\n";
                            cout << "┃ Um escudo mecânico que usa uma tecnlogia experimental de repulsão, que, em     ┃\n";
                            cout << "┃ teoria, é capaz de defletir ataques, e faze-los voltar para o agressor.        ┃\n";
                            cout << "┃ 25% de dar dano no inimigo baseado na sua defesa, 25% de quebrar ao fazer isso.┃\n";
                            new_line("┣","━","┫",80);
                            cout << "┃ - Pergaminho de Conhecimento ():                                              ┃\n";
                            cout << "┃ Apesar da Horda de Aço não utilizar tanta magia, o Pergaminho é utilizado para ┃\n";
                            cout << "┃ treinar rapidamente soldados em pouco tempo.                                   ┃\n";
                            cout << "┃ Usa-lo concede +1 ou -1 em algum atributo ou status aleatório.                 ┃\n";
                            new_line("┣","━","┫",80);
                            cout << "┃ - Totem da Ressurreição ():                                                   ┃\n";
                            cout << "┃ Um totem de um dragão mecânico esculpido em obsidiana com detalhes em prata,   ┃\n";
                            cout << "┃ um dos poucos itens mágicos produzidos pela Horda de Aço, e mais raro ainda    ┃\n";
                            cout << "┃ por seus efeitos: Salvar um ser vivo da morte.                                 ┃\n";
                            cout << "┃ Impede que você morra, precisa estar equipado para surtir efeito.              ┃\n";
                            new_line("┗","━","┛",80);
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
                    cout << "┃     CODEX: BESTIÁRIO     ┃\n";
                    new_line("┣","━","┫",26);
                    if (game.menu.optionVertical == 1) {
                        if (game.menu.optionHorizontal == 1) {
                            cout << "┃  \e[93m[Página 1 - SLIME]\e[0m      ┃\n";
                        }
                        if (game.menu.optionHorizontal == 2) {
                            cout << "┃  \e[93m[Página 2 - GOBLIN]\e[0m     ┃\n";
                        }
                        if (game.menu.optionHorizontal == 3) {
                            cout << "┃  \e[93m[Página 3 - KOBOLD]\e[0m     ┃\n";
                        }
                        if (game.menu.optionHorizontal == 4) {
                            cout << "┃  \e[93m[Página 4 - ORC]\e[0m        ┃\n";
                        }
                        if (game.menu.optionHorizontal == 5) {
                            cout << "┃  \e[93m[Página 5 - CICLOPE]\e[0m    ┃\n";
                        }
                        if  (game.menu.optionHorizontal == 6) {
                            cout << "┃  \e[93m[Página 6 - TROLL]\e[0m      ┃\n";
                        }
                        if (game.menu.optionHorizontal == 7) {
                            cout << "┃  \e[93m[Página 7 - MÍMICO]\e[0m     ┃\n";
                        }
                        if (game.menu.optionHorizontal == 8) {
                            cout << "┃  \e[93m[Página 8 - TROGLODITA]\e[0m ┃\n";
                        }
                        if (game.menu.optionHorizontal == 9) {
                            cout << "┃  \e[93m[Página 9 - AUTÔNOMO]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 10) {
                            cout << "┃  \e[93m[Página 10 - ESCÓRIA]\e[0m   ┃\n";
                        }
                        if (game.menu.optionHorizontal == 11) {
                            cout << "┃  \e[93m[Página 11 - BOSS]\e[0m      ┃\n";
                        }
                        cout << "┃                          ┃\n";
                    } else {
                        if (game.menu.optionHorizontal == 1) {
                            cout << "┃ [Página 1 - SLIME]       ┃\n";
                        }
                        if (game.menu.optionHorizontal == 2) {
                            cout << "┃ [Página 2 - GOBLIN]      ┃\n";
                        }
                        if (game.menu.optionHorizontal == 3) {
                            cout << "┃ [Página 3 - KOBOLD]      ┃\n";
                        }
                        if (game.menu.optionHorizontal == 4) {
                            cout << "┃ [Página 4 - ORC]         ┃\n";
                        }
                        if (game.menu.optionHorizontal == 5) {
                            cout << "┃ [Página 5 - CICLOPE]     ┃\n";
                        }
                        if  (game.menu.optionHorizontal == 6) {
                            cout << "┃ [Página 6 - TROLL]       ┃\n";
                        }
                        if (game.menu.optionHorizontal == 7) {
                            cout << "┃ [Página 7 - MÍMICO]      ┃\n";
                        }
                        if (game.menu.optionHorizontal == 8) {
                            cout << "┃ [Página 8 - TROGLODITA]  ┃\n";
                        }
                        if (game.menu.optionHorizontal == 9) {
                            cout << "┃ [Página 9 - AUTôNOMO]    ┃\n";
                        }
                        if (game.menu.optionHorizontal == 10) {
                            cout << "┃ [Página 10 - ESCÓRIA]    ┃\n";
                        }
                        if (game.menu.optionHorizontal == 11) {
                            cout << "┃ [Página 11 - BOSS]       ┃\n";
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
                            new_line("┏","━","┓",76);
                            cout << "┃ - SLIME (󰛹):                                                               ┃\n";
                            cout << "┃ 'Slime' é um termo informal e popular utilizado para descrever 'criaturas' ┃\n";
                            cout << "┃ que, na verdade, consistem em um aglomerado de micro-organismos            ┃\n";
                            cout << "┃ unicelulares reunidos em uma massa amorfa semelhante a uma bolha, não se   ┃\n";
                            cout << "┃ sabe ao certo como surgem, mas é de conhecimento geral que são criaturas   ┃\n";
                            cout << "┃ relativamente fracas, porém, não se deve subestimar seu perigo. Os slimes  ┃\n";
                            cout << "┃ são capazes de absorver e manter matéria inorgânica em seus corpos,        ┃\n";
                            cout << "┃ utilizando-a como arma; pedras afiadas, pedaços de metal, e até mesmo      ┃\n";
                            cout << "┃ adagas já foram encontrados alojados em seu interior. Seu método de ataque ┃\n";
                            cout << "┃ mais comum consiste em saltar contra o alvo, impulsionando os objetos      ┃\n";
                            cout << "┃ contidos em seu corpo na direção da vítima. Observou-se que os slimes das  ┃\n";
                            cout << "┃ Terras do Fogo e Ferro raramente atacam os habitantes locais, demonstrando ┃\n";
                            cout << "┃ hostilidade quase exclusiva contra humanos e anões, a razão para esse      ┃\n";
                            cout << "┃ comportamento permanece desconhecida.                                      ┃\n";
                            new_line("┗","━","┛",76);
                        break;

                        case 2:
                            new_line("┏","━","┓",76);
                            cout << "┃ - GOBLIM ():                                                              ┃\n";
                            cout << "┃ Pequenos humanoides de pele esverdeada, os goblins são criaturas           ┃\n";
                            cout << "┃ relativamente fracas e conhecidas por sua natureza cautelosa, muitas vezes ┃\n";
                            cout << "┃ confundida com covardia. Ainda assim, não hesitam em lutar quando          ┃\n";
                            cout << "┃ ameaçados. Dentro da Horda de Aço, os goblins costumam ser designados para ┃\n";
                            cout << "┃ tarefas simples, como patrulhamento, manutenção, limpeza e organização de  ┃\n";
                            cout << "┃ suprimentos. Apesar disso, possuem uma reputação como comerciantes         ┃\n";
                            cout << "┃ habilidosos, por parte da sua ganância e obsessão por riqueza, são mestres ┃\n";
                            cout << "┃ do mercantilismo, capazes de negociar praticamente qualquer coisa com      ┃\n";
                            cout << "┃ qualquer pessoa. Ironicamente, mesmo em meio às hostilidades entre os      ┃\n";
                            cout << "┃ continentes, diversos goblins podem ser encontrados operando nos mercados  ┃\n";
                            cout << "┃ negros dos 3 continentes, comercializam armas, ferramentas e equipamentos  ┃\n";
                            cout << "┃ de excelente qualidade produzidos nas forjas da Horda. Alguns goblins são  ┃\n";
                            cout << "┃ menos extremistas do que muitos de seus compatriotas, colocando o lucro    ┃\n";
                            cout << "┃ acima de rivalidades raciais ou políticas. Embora frequentemente sejam     ┃\n";
                            cout << "┃ considerados pouco inteligentes por outras raças, os goblins são           ┃\n";
                            cout << "┃ especialistas em gambiarras, reaproveitando sucata para reparar e montar   ┃\n";
                            cout << "┃ máquinas, embora suas invenções raramente sejam elegantes ou seguras,      ┃\n";
                            cout << "┃ costumam funcionar surpreendentemente bem.                                 ┃\n";
                            new_line("┗","━","┛",76);
                        break;

                        case 3:
                            new_line("┏","━","┓",76);
                            cout << "┃ - KOBOLDS ():                                                             ┃\n";
                            cout << "┃ Pequenos humanoides escamosos, geralmente de coloração alaranjada com tons ┃\n";
                            cout << "┃ vermelhos e aparência dracônica, os kobolds afirmam ser descendentes dos   ┃\n";
                            cout << "┃ antigos dragões, hoje considerados extintos, em razão dessa crença,        ┃\n";
                            cout << "┃ costumam adornar suas armaduras com características e até estruturas       ┃\n";
                            cout << "┃ metálicas em forma de asas, tanto para reforçar sua suposta herança quanto ┃\n";
                            cout << "┃ para intimidar seus inimigos. Apesar da baixa estatura, os kobolds são     ┃\n";
                            cout << "┃ adversários perigosos e jamais devem ser subestimados em combate. Na Horda ┃\n";
                            cout << "┃ de Aço, os kobolds ocupam posições de grande importância, reconhecidos como┃\n";
                            cout << "┃ excelentes ferreiros, engenheiros e artesãos, responsáveis pela construção ┃\n";
                            cout << "┃ e manutenção das vastas forjas subterrâneas que sustentam a máquina de     ┃\n";
                            cout << "┃ guerra da Horda. Também são especialistas na criação de armadilhas e       ┃\n";
                            cout << "┃ mecanismos defensivos, muitas vezes trabalhando em conjunto com inventores ┃\n";
                            cout << "┃ goblins para desenvolver elas. Além de suas funções industriais, os        ┃\n";
                            cout << "┃ kobolds atuam como supervisores e administradores, coordenando operações de┃\n";
                            cout << "┃ mineração, produção de armamentos e logística militar.                     ┃\n";
                            new_line("┗","━","┛",76);
                        break;

                        case 4:
                            new_line("┏","━","┓",76);
                            cout << "┃ - ORCS (󰶏):                                                                ┃\n";
                            cout << "┃ Grandes humanoides de pele verde-oliva e durões, os orcs são combatentes   ┃\n";
                            cout << "┃ temíveis por sua força física, mas o que realmente os distingue de outras  ┃\n";
                            cout << "┃ raças guerreiras é o respeito que demonstram pela honra e coragem em       ┃\n";
                            cout << "┃ batalha. Na Horda de Aço, os orcs desempenham um papel fundamental ao      ┃\n";
                            cout << "┃ operar as enormes fornalhas e auxiliar na produção do vasto arsenal        ┃\n";
                            cout << "┃ necessário para sustentar o esforço de guerra, sua resistência ao calor, à ┃\n";
                            cout << "┃ fadiga e ao trabalho pesado os torna especialmente adequados para as       ┃\n";
                            cout << "┃ exigências das grandes fundições subterrâneas. Os orcs também servem como  ┃\n";
                            cout << "┃ excelentes guerreiros, embora raramente sejam reconhecidos por sua         ┃\n";
                            cout << "┃ inteligência, costumam ser surpreendentemente sociáveis, em comparação com ┃\n";
                            cout << "┃ seus parentes mais brutais, os ciclopes, os orcs são geralmente muiuto     ┃\n";
                            cout << "┃ mais fáceis de compreender e de conviver.                                  ┃\n";
                            new_line("┗","━","┛",76);
                        break;

                        case 5:
                            new_line("┏","━","┓",76);
                            cout << "┃ - CICLOPES ():                                                            ┃\n";
                            cout << "┃ Gigantes humanoides de pele verde-escura, corpo massivo, os ciclopes       ┃\n";
                            cout << "┃ possuem apenas um único olho no centro do rosto. Geralmente gordos e de    ┃\n";
                            cout << "┃ intelecto limitado, são criaturas temidas por sua força descomunal e por   ┃\n";
                            cout << "┃ seu temperamento violento. Embora compartilhem algumas características     ┃\n";
                            cout << "┃ com os orcs, as semelhanças terminam aí, os ciclopes encontram prazer na   ┃\n";
                            cout << "┃ própria violência, tratando combates como uma forma de entretenimento      ┃\n";
                            cout << "┃ brutal, transformando seus oponentes em verdadeiros sacos de pancadas. Na  ┃\n";
                            cout << "┃ Horda de Aço, devido à dificuldade que apresentam para executar tarefas    ┃\n";
                            cout << "┃ complexas ou seguir instruções detalhadas, os ciclopes são normalmente     ┃\n";
                            cout << "┃ designados para o trabalho pesado nas minas, escavando túneis, extraindo   ┃\n";
                            cout << "┃ minério e transportando enormes quantidades de pedra e metal para as       ┃\n";
                            cout << "┃ fundições.                                                                 ┃\n";
                            new_line("┗","━","┛",76);
                        break;

                        case 6:
                            new_line("┏","━","┓",76);
                            cout << "┃ - TROLLS ():                                                              ┃\n";
                            cout << "┃ Humanoides médios de pele verde-azulada escura, os trolls possuem corpos   ┃\n";
                            cout << "┃ esguios, o que lhes confere uma agilidade que seus outros 'primos' mais    ┃\n";
                            cout << "┃ brutos não possuem. Em termos intelectuais, trolls e orcs apresentam       ┃\n";
                            cout << "┃ capacidades relativamente semelhantes, embora os trolls sejam geralmente   ┃\n";
                            cout << "┃ considerados um pouco mais perspicazes. Essa combinação de inteligência    ┃\n";
                            cout << "┃ prática e agilidade faz deles excelentes assassinos. Na Horda de Aço, os   ┃\n";
                            cout << "┃ trolls são frequentemente empregados como batedores e assassinos. Embora   ┃\n";
                            cout << "┃ não possuam a mesma resistência física dos orcs, compensam essa limitação  ┃\n";
                            cout << "┃ com velocidade, agilidade e furtividade letal.                             ┃\n";
                            new_line("┗","━","┛",76);
                        break;

                        case 7:
                            new_line("┏","━","┓",76);
                            cout << "┃ - MÍMICOS (󰜦):                                                             ┃\n";
                            cout << "┃ Criaturas de natureza estranha e aparentemente amorfa, os mímicos possuem  ┃\n";
                            cout << "┃ a capacidade de assumir a aparência de um baú comum. Sua origem permanece  ┃\n";
                            cout << "┃ desconhecida, embora existam teorias que afirmem que foram criados pela    ┃\n";
                            cout << "┃ própria Horda de Aço como uma forma de proteger depósitos de suprimentos,  ┃\n";
                            cout << "┃ tesouros e armamentos contra incursões de humanos e anões.                 ┃\n";
                            cout << "┃ Independentemente de sua verdadeira origem, os mímicos são criaturas       ┃\n";
                            cout << "┃ extremamente perigosas, permanecem imóveis por longos períodos, escondidos ┃\n";
                            cout << "┃ em salas de tesouro, aguardando pacientemente até que uma vítima se        ┃\n";
                            cout << "┃ aproxime. À primeira vista, distinguem-se de um baú comum apenas por       ┃\n";
                            cout << "┃ pequenos detalhes que passam despercebidos, somente quando alguém tenta    ┃\n";
                            cout << "┃ abrir o 'baú' é que o mímico se revela, deformando-se rapidamente e        ┃\n";
                            cout << "┃ expondo uma enorme boca repleta de dentes afiados e uma língua             ┃\n";
                            cout << "┃ surpreendentemente longa.                                                  ┃\n";
                            new_line("┗","━","┛",76);
                        break;

                        case 8:
                            new_line("┏","━","┓",76);
                            cout << "┃ - TROGLODITA ():                                                          ┃\n";
                            cout << "┃ Grandes criaturas reptilianas cobertas por escamas brancas e               ┃\n";
                            cout << "┃ avermelhadas, embora não sejam conhecidas por sua inteligência,            ┃\n";
                            cout << "┃ compensam essa limitação com força física, resistência e selvageria. É     ┃\n";
                            cout << "┃ especulado que os trogloditas possuam alguma ligação com os kobolds ou até ┃\n";
                            cout << "┃ mesmo com os antigos dragões. No entanto, devido à dificuldade de realizar ┃\n";
                            cout << "┃ novos estudos e investigações mais profundas, não existe nada de concreto  ┃\n";
                            cout << "┃ que comprove essa teoria. Diferentemente de outras raças que habitavam as  ┃\n";
                            cout << "┃ antigas Terras Selvagens, os trogloditas permaneceram praticamente         ┃\n";
                            cout << "┃ desconhecidos para humanos e anões durante séculos, vivendo em vastas      ┃\n";
                            cout << "┃ redes de cavernas abaixo da superfície. Foi apenas após a ascensão da      ┃\n";
                            cout << "┃ Horda de Aço e a expansão das operações de mineração que sua existência se ┃\n";
                            cout << "┃ tornou amplamente conhecida. Na Horda, os trogloditas são empregados como  ┃\n";
                            cout << "┃ mineradores, escavadores e guardas das regiões mais profundas das minas,   ┃\n";
                            cout << "┃ forjas e fundições subterrâneas.                                           ┃\n";
                            new_line("┗","━","┛",76);
                        break;

                        case 9:
                            new_line("┏","━","┓",76);
                            cout << "┃ - AUTÔNOMO (󰢻):                                                            ┃\n";
                            cout << "┃ Máquinas semiautônomas originalmente desenvolvidas por humanos e anões,    ┃\n";
                            cout << "┃ após a expulsão desses povos das Terras do Fogo e Ferro, a Horda de Aço    ┃\n";
                            cout << "┃ conseguiu recuperar exemplares, estudar sua construção e eventualmente     ┃\n";
                            cout << "┃ produzir suas próprias versões. Existem diversos modelos de Autônomos      ┃\n";
                            cout << "┃ produzidos, e o modelo mais comum nas instalações da Horda é o Atlas.      ┃\n";
                            cout << "┃ Ele possui um torso metálico de formato humanoide conectado a uma base     ┃\n";
                            cout << "┃ sustentada por quatro pernas articuladas, lembrando vagamente uma aranha.  ┃\n";
                            cout << "┃ Seus braços terminam em garras mecânicas, e sua cabeça contém um único e   ┃\n";
                            cout << "┃ grande olho mecânico. O Atlas pode desempenhar funções de transporte de    ┃\n";
                            cout << "┃ materiais e operações logísticas, mas também pode ser empregado            ┃\n";
                            cout << "┃ diretamente como unidade de combate.                                       ┃\n";
                            new_line("┗","━","┛",76);
                        break;

                        case 10:
                            new_line("┏","━","┓",76);
                            cout << "┃ - ESCÓRIA ():                                                             ┃\n";
                            cout << "┃ Os Escórias são humanos capturados pela Horda de Aço e submetidos a        ┃\n";
                            cout << "┃ experimentos para fundir carne e metal em um único organismo. O objetivo   ┃\n";
                            cout << "┃ original dessas pesquisas era criar trabalhadores e soldados capazes de    ┃\n";
                            cout << "┃ suportar condições extremas para qualquer ser vivo comum, incorporando     ┃\n";
                            cout << "┃ próteses mecânicas e outros componentes artificiais diretamente ao corpo.  ┃\n";
                            cout << "┃ Os resultados, porém, raramente corresponderam às expectativas, com a      ┃\n";
                            cout << "┃ maioria dos indivíduos não sobrevivendo aos processos, muito dos outros    ┃\n";
                            cout << "┃ dos sobreviventes perderam a sanidade ou sofreram deformações severas.     ┃\n";
                            cout << "┃ Apenas uma pequena parcela conseguiu permanecer funcional após as          ┃\n";
                            cout << "┃ modificações. Eles são empregados em tarefas extremamente perigosas, como  ┃\n";
                            cout << "┃ a manutenção de fornalhas e o transporte de materiais em áreas perigosas.  ┃\n";
                            cout << "┃ Já aqueles considerados inadequados costumam ser relegados a trabalhos     ┃\n";
                            cout << "┃ degradantes ou utilizados como bucha de canhão. A aparência de um Escória  ┃\n";
                            cout << "┃ varia de acordo com os implantes e os danos sofridos durante sua           ┃\n";
                            cout << "┃ transformação; membros mecânicos e placas metálicas fundidas à pele são    ┃\n";
                            cout << "┃ características comuns entre esses infelizes. O termo 'Escória' deriva do  ┃\n";
                            cout << "┃ subproduto gerado durante o processo de fundição dos minérios: uma massa   ┃\n";
                            cout << "┃ de impurezas que flutua sobre o metal líquido na forma de uma espuma       ┃\n";
                            cout << "┃ escura.                                                                    ┃\n";
                            new_line("┗","━","┛",76);
                        break;

                        case 11:
                            new_line("┏","━","┓",76);
                            cout << "┃ - CORAÇÃO DA FORJA - FLAGELO DO DRAGÃO ():                                ┃\n";
                            cout << "┃ O Flagelo do Dragão é um gigantesco mecha projetado pelo próprio Victor e  ┃\n";
                            cout << "┃ considerado a maior realização da engenharia da Horda de Aço. Construído   ┃\n";
                            cout << "┃ nas profundezas das grandes fundições e armado com diversos protótipos de  ┃\n";
                            cout << "┃ armamentos experimentais, sua estrutura bípede colossal combina placas de  ┃\n";
                            cout << "┃ aço reforçadas, articulações hidráulicas e tecnologias que muitos          ┃\n";
                            cout << "┃ acreditavam ser impossíveis de reproduzir, ainda pouco se sabe sobre suas  ┃\n";
                            cout << "┃ capacidades completas. O Flagelo do Dragão, porém, não foi concebido       ┃\n";
                            cout << "┃ apenas como uma arma de destruição, para Victor, representa um ideal:      ┃\n";
                            cout << "┃ um símbolo do futuro que a Horda de Aço pretende construir e da            ┃\n";
                            cout << "┃ superioridade da engenharia sobre as tradições e magias do passado.        ┃\n";
                            new_line("┗","━","┛",76);
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
                cout << "┃ - No mundo de Farlands, existe um arquipélago composto por três continentes:   ┃\n";
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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
            if(y==game.map.player.inventorySelection.Y && x==game.map.player.inventorySelection.X){
                cout<<"\e[38;5;255m\e[48;5;237m";
            }
            if(y==0){
                if(game.map.player.inventory[y][x].cursed){
                    cout<<"\e[38;5;9m";
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
                cout<<"󱄮"; // POÇÃO DE TELEPORTE

            }
            if(game.map.player.inventory[y][x].id==8){
                cout<<"󰂪"; // ESCUDO REFLETOR

            }
            if(game.map.player.inventory[y][x].id==9){
                cout<<""; // TOTEM

            }
            if(game.map.player.inventory[y][x].id==10){
                cout<<""; // PERGAMINHO
            }
            cout<<"\e[0m ";
        }
        cout<<"\e[0m┃\n";
    }
    new_line("┗","━","┛",3*3);
    cout<<"\e[1;12H";
    cout<<"HP: "<<game.map.player.attributes.hp<<"/"<<game.map.player.attributes.hpMax<<"   ";
    cout<<"\e[2;12H";
    cout<<"DEFESA: "<<game.map.player.attributes.defense<<"   ";
    cout<<"\e[3;12H";
    cout<<"FORÇA: "<<game.map.player.attributes.strength<<"   ";
    cout<<"\e[4;12H";
    cout<<"INTELIGENCIA: "<<game.map.player.attributes.intelligence<<"   ";
    cout<<"\e[5;12H";
    cout<<"DESTREZA: "<<game.map.player.attributes.dexterity<<"   ";
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    string getPlayerAvatar(GAME &game) {

                    int armaID = game.map.player.inventory[0][0].id;
                    if(game.map.player.attributes.hp < 1) return "󰮢";
                    if(armaID == 1) return ""; // Espada
                    if(armaID == 5) return "󱅻"; // Cajado
                    if(armaID == 4) return ""; // Adaga
                    return "";
                }


void render_map(GAME &game){
    int tempo = (clock() - game.map.player.timerStart - game.map.player.tempoPausado) / CLOCKS_PER_SEC;

    int horas = tempo / 3600;
    int minutos = (tempo % 3600) / 60;
    int segundos = tempo % 60;
    if(game.map.player.inventoryOpened && game.map.player.attributes.hp>0){
        show_inventory(game);
        return;
    }
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
                    if(game.map.floor==11){
                        if(rand()%2==0){
                            cout<<"\e[48;5;202m ";
                        }else{
                            if(rand()%2==0){
                                cout<<"\e[48;5;208m ";
                            }else{
                                cout<<"\e[48;5;202m ";
                            }
                        }

                    }else{
                        cout<<"\e[0m ";
                    }
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
                    cout<<"\e[48;5;246m\e[38;5;247m";
                }
                if(game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]==KEYBLOCK){
                    cout<<"\e[48;5;246m\e[38;5;3m";
                }
                if(game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]==NPCBLOCK){
                    cout<<"\e[48;5;246m\e[38;5;121m";
                }
                for(int item=0;item<10;item++){
                    if(game.map.items[item].Y==game.map.player.pos.Y+y && game.map.items[item].X==game.map.player.pos.X+x){
                        cout<<"\e[1D\e[38;5;13m󰜦";
                    }
                }


                   if(y==0 && x==0){
                    cout << "\e[1D"; // Volta uma casa
                    cout << "\e[38;5;255m" << getPlayerAvatar(game) << "\e[0m";
                    }

                for(int monster=0;monster<50;monster++){
                    if(game.map.monsters[monster].alive){
                        if(game.map.monsters[monster].pos.Y==game.map.player.pos.Y+y && game.map.monsters[monster].pos.X==game.map.player.pos.X+x){
                            cout<<"\e[1D";
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
                                cout<<"\e[38;5;34m󰶏";
                            }
                            if(game.map.monsters[monster].id==4){ // CICLOPE
                                cout<<"\e[38;5;106m";
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
                                cout<<"\e[38;5;214m󰢻";
                            }
                            if(game.map.monsters[monster].id==9){ // ESCORIA
                                cout<<"\e[38;5;52m";
                            }
                            if(game.map.monsters[monster].id==10){ // BOSS
                                cout<<"\e[38;5;232m";
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
    cout<<"\e[0m┃ "<<game.map.player.attributes.hp<<"/"<<game.map.player.attributes.hpMax<<"   ";
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
            cout<<""; // TOTEM
        }
        cout<<"\e[0m ";
    }
    cout<<"\e[0m┃";
    if(game.map.player.key){
        cout<<" \e[38;5;3m\e[0m";
    }
    cout<<"\e[6;"<<((vision+1)*2)+1<<"H";
    new_line("┗","━","┛",3*3);
    cout<<"\e[7;"<<((vision+1)*2)+1<<"H";
    cout<<"ANDAR: "<<game.map.floor;
    cout<<"\e[8;"<<((vision+1)*2)+1<<"H";
    cout<<"NIVEL: "<<game.map.player.nivel;
    cout<<"\e[9;"<<((vision+1)*2)+1<<"H";
    if(game.map.player.exp>=game.map.player.nextExp){
        cout<<"\e[38;5;3m";
    }
    cout<<"EXP: "<<game.map.player.exp<<"/"<<game.map.player.nextExp<<"\e[0m      ";
    cout<<"\e[10;"<<((vision+1)*2)+1<<"H";
    cout<<"OURO: "<<game.map.player.gold;
    cout<<"\e[11;"<<((vision+1)*2)+1<<"H";
    cout<<"TEMPO: ";
    if(horas < 10) cout<<"0";
    cout << horas <<":";

    if(minutos < 10) cout<<"0";
    cout << minutos <<":";

    if(segundos < 10) cout<<"0";
    cout << segundos;

    if(game.pauseTudo){
    cout << "\e[13;"<<((vision+1)*2)+1<<"H";
    cout << "\e[38;5;226m=== PAUSADO ===\e[0m";
    }
}

void send_message(GAME &game,string message,int time){
    cout<<"\e[23;1H";
    cout<<message;
    int timeClock = clock();
    while(clock()-timeClock<time){
        if(kbhit()){
            getch();
        }
    }
    game.map.player.clockSpeed-=time;
    for(int monster=0;monster<50;monster++){
        game.map.monsters[monster].clockSpeed-=time;
    }
    cout<<"\e[23;1H";
    cout<<"\e[2K";
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void clear_slot(GAME &game,int y,int x){
    game.map.player.inventory[y][x].id = 0;
    game.map.player.inventory[y][x].defense = 0;
    game.map.player.inventory[y][x].dexterity = 0;
    game.map.player.inventory[y][x].heal = 0;
    game.map.player.inventory[y][x].hp = 0;
    game.map.player.inventory[y][x].intelligence = 0;
    game.map.player.inventory[y][x].strength = 0;
    game.map.player.inventory[y][x].cursed = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool add_to_inventory(GAME &game, int itemId) {
    for(int y = 1; y < 4; y++) {
        for(int x = 0; x < 3; x++) {
            if(game.map.player.inventory[y][x].id == 0) {
                game.map.player.inventory[y][x].id = itemId;
                return true;
            }
        }
    }
    return false;
}


void move_monsters(GAME &game){
    if(game.pauseTudo){
    return;
    }
    int blocks[4] = {FREEBLOCK,STAIRBLOCK,KEYBLOCK,TRAPBLOCK};
    bool win = true;
    for(int monster=0;monster<50;monster++){
        if(game.map.tiles[game.map.monsters[monster].pos.Y][game.map.monsters[monster].pos.X]==KEYBLOCK){
            game.map.tiles[game.map.monsters[monster].pos.Y][game.map.monsters[monster].pos.X] = FREEBLOCK;
            game.map.monsters[monster].key = true;
        }
        if(game.map.monsters[monster].attributes.hp<1){
            if(game.map.monsters[monster].alive){
                if(game.map.monsters[monster].id == 0) {
                playSound("morte_slime", "Sounds/slime_morte.wav");
            }
            else if(game.map.monsters[monster].id == 3) {
                playSound("morte_orc", "Sounds/orc_morte.wav");
            }

            else if(game.map.monsters[monster].id == 5) {
                playSound("morte_troll", "Sounds/troll_morte.ogg");
            }
            else if(game.map.monsters[monster].id == 1) {
                playSound("morte_goblin", "Sounds/goblin_morte.wav");
            }
             else if(game.map.monsters[monster].id == 2) {
                playSound("morte_kobold", "Sounds/kobold_morte.ogg");
            }

             else if(game.map.monsters[monster].id == 4) {
                playSound("morte_ciclope", "Sounds/ciclope_morte.ogg");
            }
             else if(game.map.monsters[monster].id == 7) {
                playSound("morte_troglodita", "Sounds/troglodita_morte.wav");
            }
             else if(game.map.monsters[monster].id == 9) {
                playSound("morte_escoria", "Sounds/escoria_morte.wav");
            }
             else if(game.map.monsters[monster].id == 8) {
                playSound("morte_autonomo", "Sounds/autonomo_morte.wav");
            }


            int dropChance = rand() % 100;
            int itemId = 0;

            if(game.map.monsters[monster].id == 0 && dropChance < 5) itemId = 7;      // Slime: 5% Teleporte
            else if(game.map.monsters[monster].id == 1 && dropChance < 20) itemId = 4; // Goblin: 20% Adaga
            else if(game.map.monsters[monster].id == 3 && dropChance < 20) itemId = 1; // Orc: 20% Espada
            else if(game.map.monsters[monster].id == 8 && dropChance < 20) itemId = 5; // Autonomo: 20% Cajado
            else if(game.map.monsters[monster].id == 9 && dropChance < 5) itemId = 9;  // Escoria: 5% Totem
            else if(game.map.monsters[monster].id == 5 && dropChance < 10) itemId = 3; // Troll: 10% Anel
            else if(game.map.monsters[monster].id == 6 && dropChance < 10) itemId = 10;// Mimico: 10% Pergaminho
            else if(game.map.monsters[monster].id == 2 && dropChance < 5) itemId = 3;  // Kobold: 5% pot

            if(itemId != 0) {
                if(add_to_inventory(game, itemId)) {
                    playSound("loot", "Sounds/loot.wav");
                    send_message(game, "ITEM COLETADO!", 1000);
                }
            }

                if (!(game.map.floor == 6 && monster == 0)) {
                game.map.monsters[monster].id++;
                }
                game.map.player.exp += rand()%game.map.monsters[monster].id+1;
                game.map.player.gold += rand()%game.map.monsters[monster].id+1;
                game.map.monsters[monster].alive = false;
                if(game.map.monsters[monster].key){
                    game.map.monsters[monster].key = false;
                    if(game.map.tiles[game.map.monsters[monster].pos.Y][game.map.monsters[monster].pos.X]==FREEBLOCK){
                        game.map.tiles[game.map.monsters[monster].pos.Y][game.map.monsters[monster].pos.X] = KEYBLOCK;
                    }else{
                        while(true){
                            int y = rand()%MAPSIZEY;
                            int x = rand()%MAPSIZEX;
                            if(game.map.tiles[y][x]==FREEBLOCK){
                                game.map.tiles[y][x] = KEYBLOCK;
                                break;
                            }
                        }
                    }
                }
            }
        }
        if(game.map.monsters[monster].alive){
            win = false;
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
                    int defended = (rand()%game.map.player.attributes.defense+1);
                    if(game.map.player.inventory[0][1].id==2){
                        if(rand()%2==0){ // 50%
                            defended += (rand()%game.map.player.attributes.defense+1);
                            if(rand()%10==0){ // 10%
                                clear_slot(game,0,1);
                            }
                        }
                    }
                    if(game.map.player.inventory[0][1].id==8){
                        if(rand()%2==0){ // 50%
                            game.map.monsters[monster].attributes.hp-=defended;
                            defended = 1;
                            if(rand()%10==0){ // 10%
                                clear_slot(game,0,1);
                            }
                        }
                    }
                    if(game.map.player.inventory[0][2].id==3){
                        if(rand()%100==0){
                            clear_slot(game,0,2);
                        }
                    }
                    if(defended==0){
                        if(rand()%(100/game.map.player.attributes.intelligence)==0){
                            defended = rand()%game.map.player.attributes.defense+1;
                        }
                    }
                    if(defended>attack){
                        defended = attack;
                    }
                    int danoReal = attack - defended;


                    game.map.player.attributes.hp-=(attack-defended);
                    game.map.player.inventoryOpened = false;
                    if (danoReal > 0) {
                        render_map(game);
                        playSound("dano_recebido", "Sounds/dano_recebido.wav");
                        send_message(game, "\e[38;5;9mDANO RECEBIDO: " + to_string(danoReal) + "\e[0m", 1000);
                    }

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
    if(game.map.floor==6){
            if(win){
            render_map(game);
            cout<<"\e[23;1H";
            cout<<"VITORIA!";
            fstream file; // Salva um arquivo com a pontuação do player
            file.open("Score.txt", ios::app);
            file << game.map.player.name << "\n";
            file << game.map.player.gold << "\n";
            file.close();
            int timeClock = clock();
            while(clock()-timeClock<2000){
                if(kbhit()){
                    getch();
                }
            }
            getch();
            cout<<"\ec";
            game.play = false;
            return;
        }
        if(game.map.monsters[0].attributes.hp<1){
            for(int monster=0;monster<50;monster++){
                game.map.monsters[monster].attributes.hp = 0;
                game.map.monsters[monster].pos.Y = -1;
                game.map.monsters[monster].pos.X = -1;
            }
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void move_player(GAME &game){
    if(game.pauseTudo){
    if(kbhit()){
        game.map.player.keyInput = getch();

        if(game.map.player.keyInput=='e' || game.map.player.keyInput=='E'){
            game.pauseTudo = false;

            game.map.player.clockSpeed = clock();

            for(int i=0;i<50;i++){
                game.map.monsters[i].clockSpeed = clock();
            }
        }
    }
    return;
    }
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
            for(int y=0;y<MAPSIZEY;y++){
                for(int x=0;x<MAPSIZEX;x++){
                    game.map.memory[y][x] = 0;
                }
            }
            for(int y=0;y<4;y++){
                for(int x=0;x<3;x++){
                    clear_slot(game,y,x);
                }
            }
            return;
        }
        playSound("Morto", "Sounds/morte.wav");
        send_message(game,"MORREU",4000);
        cout<<"\ec";
        game.play = false;
        return;
    }
    if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X]==EMPTY){
        if(game.map.floor==11){
            game.map.player.attributes.hp = 0;
            return;
        }
        game.next = true;
        game.map.player.fallen = true;
        game.map.floor++;
        game.map.player.attributes.hp = 1;
        if(rand()%2==0){
            game.map.player.attributes.hp -= 1;
        }
        playSound("caindo", "Sounds/caindo.wav");
        send_message(game,"CAINDO...",2000);
        return;
    }
    if(kbhit()){
        game.map.player.keyInput = getch();
        if(game.map.player.keyInput=='e' || game.map.player.keyInput=='E'){
            game.pauseTudo = !game.pauseTudo;
            if(game.pauseTudo) {
                game.pauseTudoClock = clock();

            } else {

                game.map.player.tempoPausado += clock() - game.pauseTudoClock;
                game.map.player.clockSpeed = clock();
                for(int i=0;i<50;i++){
                    game.map.monsters[i].clockSpeed = clock();
                }
            }
            return;
        }
        if((clock()-game.map.player.clockSpeed)>1000/game.map.player.attributes.dexterity){
            if(game.map.player.exp>=game.map.player.nextExp){
                game.map.player.exp = 0;
                game.map.player.attPoints++;
                game.map.player.nivel++;
                game.map.player.nextExp+=rand()%game.map.player.nextExp+1;
            }
            game.map.player.clockSpeed = clock();
            POS targetPos = {0,0};
            int blocks[5] = {FREEBLOCK,STAIRBLOCK,EMPTY,KEYBLOCK,TRAPBLOCK};
            bool success = true;
            for(int block=0;block<5;block++){
                if(block==2 && rand()%10!=0){
                    continue;
                }
                if(game.map.player.keyInput==119){        // cima passo
                    if(game.map.player.inventoryOpened){
                        game.map.player.inventorySelection.Y--;
                    }
                    if(game.map.tiles[game.map.player.pos.Y-1][game.map.player.pos.X]==blocks[block]){
                        targetPos.Y--;
                    playSound("passo", "Sounds/passo.ogg");
                    }


                }
                if(game.map.player.keyInput==115){
                    if(game.map.player.inventoryOpened){
                        game.map.player.inventorySelection.Y++;

                    }
                    if(game.map.tiles[game.map.player.pos.Y+1][game.map.player.pos.X]==blocks[block]){
                        targetPos.Y++;
                    playSound("passo", "Sounds/passo.ogg");
                    }
                }
                if(game.map.player.keyInput==97){
                    if(game.map.player.inventoryOpened){
                        game.map.player.inventorySelection.X--;
                    }
                    if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X-1]==blocks[block]){
                        targetPos.X--;
                    playSound("passo", "Sounds/passo.ogg");
                    }
                }
                if(game.map.player.keyInput==100){
                    if(game.map.player.inventoryOpened){
                        game.map.player.inventorySelection.X++;
                    }
                    if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X+1]==blocks[block]){
                        targetPos.X++;
                    playSound("passo", "Sounds/passo.ogg");
                    }
                }
                if(game.map.player.keyInput==105){
                    cout<<"\ec";
                    if(game.map.player.inventoryOpened == false){
                        game.map.player.inventoryOpened = true;
                    playSound("inventario", "Sounds/inventario.wav");
                    }else{
                        game.map.player.inventoryOpened = false;
                    }
                    game.map.player.keyInput = 0;
                }
                if(game.map.player.keyInput==13){
                    if(!game.map.player.inventoryOpened){
                        bool achouSacerdote = (
                            game.map.tiles[game.map.player.pos.Y-1][game.map.player.pos.X] == NPCBLOCK ||
                            game.map.tiles[game.map.player.pos.Y+1][game.map.player.pos.X] == NPCBLOCK ||
                            game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X-1] == NPCBLOCK ||
                            game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X+1] == NPCBLOCK
                        );
                        if(achouSacerdote){
                            game.map.player.attributes.hp = game.map.player.attributes.hpMax; // quantidade da cura
                            if(game.map.player.attributes.hp > game.map.player.attributes.hpMax){
                                game.map.player.attributes.hp = game.map.player.attributes.hpMax; // pra n passar da vida máxima
                            }
                            for(int y=-1;y<=1;y++){
                                for(int x=-1;x<=1;x++){
                                    if(game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x]==NPCBLOCK){
                                        game.map.tiles[game.map.player.pos.Y+y][game.map.player.pos.X+x] = FREEBLOCK;
                                    }
                                }
                            }
                            playSound("Heal", "Sounds/Heal.wav");
                            send_message(game,"\e[38;5;10mCURANDO...\e[0m",1000);
                            return;
                        }
                        for(int item=0;item<10;item++){
                            if(game.map.items[item].Y==game.map.player.pos.Y && game.map.items[item].X==game.map.player.pos.X){
                                playSound("abrir_bau", "Sounds/bau.wav"); // bau
                                for(int y=1;y<4;y++){
                                    for(int x=0;x<3;x++){
                                        if(game.map.player.inventory[y][x].id==0){
                                            game.map.player.gold+=game.map.floor;
                                            game.map.items[item].Y = -1;
                                            game.map.items[item].Y = -1;
                                            clear_slot(game,y,x);
                                            game.map.player.inventory[y][x].id = rand()%10+1;
                                            if(rand()%20==0){
                                                game.map.player.inventory[y][x].cursed = true;
                                            }
                                            if(rand()%4==0){
                                                game.map.player.inventory[y][x].id = 6;
                                            }
                                            if(game.map.player.inventory[y][x].id==3){
                                                int attribute = rand()%5;
                                                if(attribute==0){
                                                    game.map.player.inventory[y][x].hp = rand()%game.map.floor+1;
                                                }
                                                if(attribute==1){
                                                    game.map.player.inventory[y][x].strength = rand()%game.map.floor+1;
                                                }
                                                if(attribute==2){
                                                    game.map.player.inventory[y][x].defense = rand()%game.map.floor+1;
                                                }
                                                if(attribute==3){
                                                    game.map.player.inventory[y][x].dexterity = rand()%game.map.floor+1;
                                                }
                                                if(attribute==4){
                                                    game.map.player.inventory[y][x].intelligence = rand()%game.map.floor+1;
                                                }
                                            }
                                            if(game.map.player.inventory[y][x].id==6){
                                                game.map.player.inventory[y][x].heal = game.map.floor;
                                            }
                                            x = 3;
                                            y = 3;
                                        }
                                    }
                                }
                            }
                        }
                        if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X]==KEYBLOCK){
                            game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X] = FREEBLOCK;
                            game.map.player.key = true;
                            playSound("pegar_chave", "Sounds/chave.wav");
                        }
                        if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X]==STAIRBLOCK){
                            if(game.map.player.key){
                                game.map.player.gold += 50;
                                game.next = true;
                                game.map.floor++;
                                playSound("descendo", "Sounds/escada.wav"); //escadas
                                send_message(game,"DESCENDO...",2000);
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
                    if(game.map.player.keyInput==113){
                        if(game.map.player.inventorySelection.Y!=0){
                            clear_slot(game,game.map.player.inventorySelection.Y,game.map.player.inventorySelection.X);
                        }
                    }
                    if(game.map.player.keyInput==13){
                        if(game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id!=0){
                            if(game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==7){
                                game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id = 0;
                                while(true){
                                    int y = rand()%MAPSIZEY;
                                    int x = rand()%MAPSIZEX;
                                    if(game.map.tiles[y][x]==FREEBLOCK){
                                        game.map.player.pos.Y = y;
                                        game.map.player.pos.X = x;
                                        break;
                                    }
                                }
                            playSound("teleporte", "Sounds/equip_pot_tp.wav");
                            }
                            if(game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==10){
                                game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id = 0;
                                int attribute = rand()%5;
                                if(attribute==0){
                                    game.map.player.attributes.hp++;
                                    game.map.player.attributes.hpMax++;
                                }
                                if(attribute==1){
                                    game.map.player.attributes.defense++;
                                }
                                if(attribute==2){
                                    game.map.player.attributes.strength++;
                                }
                                if(attribute==3){
                                    game.map.player.attributes.intelligence++;
                                }
                                if(attribute==4){
                                    game.map.player.attributes.dexterity++;
                                }
                                playSound("pergaminho", "Sounds/pergaminho.ogg");
                            }
                            if(game.map.player.inventorySelection.Y==0){
                                if(!game.map.player.inventory[0][game.map.player.inventorySelection.X].cursed){
                                    for(int y=1;y<4;y++){
                                        for(int x=0;x<3;x++){
                                            if(game.map.player.inventory[y][x].id==0){
                                                if(game.map.player.inventory[0][game.map.player.inventorySelection.X].id==3){
                                                    game.map.player.attributes.hpMax-=game.map.player.inventory[0][game.map.player.inventorySelection.X].hp;
                                                    game.map.player.attributes.defense-=game.map.player.inventory[0][game.map.player.inventorySelection.X].defense;
                                                    game.map.player.attributes.strength-=game.map.player.inventory[0][game.map.player.inventorySelection.X].strength;
                                                    game.map.player.attributes.intelligence-=game.map.player.inventory[0][game.map.player.inventorySelection.X].intelligence;
                                                    game.map.player.attributes.dexterity-=game.map.player.inventory[0][game.map.player.inventorySelection.X].dexterity;
                                                }
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
                                    clear_slot(game,0,game.map.player.inventorySelection.X);
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
                                playSound("escudo", "Sounds/equip_shield.wav");
                            }
                            if(game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==3 || game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==9){
                                equip = true;
                                x = 2;
                                playSound("acessório", "Sounds/equip_acessorio.wav");
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
                                    game.map.player.inventory[0][x].cursed = game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].cursed;
                                    clear_slot(game,game.map.player.inventorySelection.Y,game.map.player.inventorySelection.X);
                                    int novoId = game.map.player.inventory[0][x].id;
                                        if (game.map.player.inventory[0][x].cursed) {
                                                playSound("maldição", "Sounds/cursed.ogg");
                                            }

                                    else{
                                    if(novoId == 1) playSound("espada", "Sounds/equip_espada.ogg");
                                    else if(novoId == 4) playSound("adaga", "Sounds/equip_adaga.ogg");
                                    else if(novoId == 5) playSound("cajado", "Sounds/equip_cajado.wav");
                                    }

                                    if(game.map.player.inventory[0][x].id==3){
                                        game.map.player.attributes.hpMax+=game.map.player.inventory[0][x].hp;
                                        game.map.player.attributes.defense+=game.map.player.inventory[0][x].defense;
                                        game.map.player.attributes.strength+=game.map.player.inventory[0][x].strength;
                                        game.map.player.attributes.intelligence+=game.map.player.inventory[0][x].intelligence;
                                        game.map.player.attributes.dexterity+=game.map.player.inventory[0][x].dexterity;
                                    }
                                }
                            }
                            if(game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].id==6){
                                if(game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].cursed){
                                    game.map.player.attributes.hp -= game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].heal;
                                }else{
                                    game.map.player.attributes.hp += game.map.player.inventory[game.map.player.inventorySelection.Y][game.map.player.inventorySelection.X].heal;
                                }
                                clear_slot(game,game.map.player.inventorySelection.Y,game.map.player.inventorySelection.X);
                                playSound("pot_cura", "Sounds/equip_pot_cura.wav");
                                if(game.map.player.attributes.hp>game.map.player.attributes.hpMax){
                                    game.map.player.attributes.hp = game.map.player.attributes.hpMax;
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
                        int attack = rand()%game.map.player.attributes.strength+1;
                        if(game.map.player.inventory[0][0].id==1){
                            if(rand()%4!=0){ // 75%
                                attack += rand()%(game.map.player.attributes.strength+1);
                                if(rand()%20==0){ // 5%
                                    clear_slot(game,0,0);
                                }
                            }
                        }
                        if(game.map.player.inventory[0][0].id==4){
                            if(rand()%2==0){ // 50%
                                attack += rand()%(game.map.player.attributes.dexterity+1);
                                attack -= rand()%(game.map.player.attributes.dexterity+1);
                                while(true){
                                    if(rand()%2==0){ // 50%
                                        attack++;
                                    }else{
                                        break;
                                    }
                                }
                                if(rand()%20==0){ // 5%
                                    clear_slot(game,0,0);
                                }
                            }
                        }
                        if(game.map.player.inventory[0][0].id==5){
                            if(rand()%2==0){ // 50%
                                attack += rand()%(game.map.player.attributes.intelligence+1);
                                if(rand()%2==0){
                                    if(rand()%2==0){
                                        attack += rand()%(game.map.player.attributes.intelligence+1);
                                    }else{
                                        attack -= rand()%game.map.player.attributes.intelligence;
                                    }
                                }
                                if(rand()%20==0){ // 5%
                                    clear_slot(game,0,0);
                                }
                            }
                        }
                        if(game.map.player.inventory[0][2].id==3){
                            if(rand()%100==0){
                                clear_slot(game,0,2);
                            }
                        }
                        int defended = rand()%(game.map.monsters[monster].attributes.defense+1);
                        if(defended==0){
                            if(rand()%(100/game.map.monsters[monster].attributes.intelligence)==0){
                                defended = rand()%game.map.player.attributes.defense+1;
                            }
                        }
                        if(defended>attack){
                            defended = attack;
                        }
                        int dano_causado = attack - defended;
                        game.map.monsters[monster].attributes.hp-=(attack-defended);
                        if (dano_causado > 0) {

                        int id_arma_equipada = game.map.player.inventory[0][0].id;


                        switch (id_arma_equipada) {
                            case 1:
                                playSound("Espada Pesada" , "Sounds/som_espada_hit.wav");
                                break;
                            case 4:
                                playSound("Adaga", "Sounds/som_adaga_hit.wav");
                                break;
                            case 5:
                                playSound("Cajado", "Sounds/som_cajado_hit.wav");
                                break;
                            default:
                                playSound("Hit", "Sounds/som_hit.wav");
                                break;
                        }
                    }           else {

                            playSound("missHit", "Sounds/miss_hit.wav");
                    }
                        send_message(game,"\e[38;5;46mDANO CAUSADO: "+to_string(attack-defended)+"\e[0m",1000);
                        success = false;
                    }
                }
                if(success){
                    game.map.player.pos.Y+=targetPos.Y;
                    game.map.player.pos.X+=targetPos.X;
                    if(game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X] == TRAPBLOCK){
                        game.map.player.attributes.hp -= game.map.floor;
                        game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X] = FREEBLOCK;
                    playSound("armadilha", "Sounds/trap.wav");        // armadilha som
                    }
                }
            }else{
                game.map.player.keyInput = 0;
            }
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void put_attributes(GAME &game){
    cout << "\e[?25l\e[H";
    cout << "\e[2;1H";
    cout<<"┃ PONTOS DE ATRIBUTOS: "<<game.map.player.attPoints<<" ";
    cout<<"\n\n";
    if(game.attSelection<0){
        game.attSelection = 0;
    }
    if(game.attSelection>4){
        game.attSelection = 4;
    }
    if(game.attSelection==0){
        cout<<"┃  \e[93mHP: "<<game.map.player.attributes.hp<<"/"<<game.map.player.attributes.hpMax<<"   \e[0m";
    }else{
        cout<<"┃ HP: "<<game.map.player.attributes.hpMax<<"   ";
    }
    cout<<"\n";
    if(game.attSelection==1){
        cout<<"┃  \e[93mDEFESA: "<<game.map.player.attributes.defense<<"   \e[0m";
    }else{
        cout<<"┃ DEFESA: "<<game.map.player.attributes.defense<<"   ";
    }
    cout<<"\n";
    if(game.attSelection==2){
        cout<<"┃  \e[93mFORÇA: "<<game.map.player.attributes.strength<<"   \e[0m";
    }else{
        cout<<"┃ FORÇA: "<<game.map.player.attributes.strength<<"   ";
    }
    cout<<"\n";
    if(game.attSelection==3){
        cout<<"┃  \e[93mINTELIGENCIA: "<<game.map.player.attributes.intelligence<<"   \e[0m";
    }else{
        cout<<"┃ INTELIGENCIA: "<<game.map.player.attributes.intelligence<<"   ";
    }
    cout<<"\n";
    if(game.attSelection==4){
        cout<<"┃  \e[93mDESTREZA: "<<game.map.player.attributes.dexterity<<"   \e[0m";
    }else{
        cout<<"┃ DESTREZA: "<<game.map.player.attributes.dexterity<<"   ";
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
            int valorUpgrade = 1;
            if(game.difficulty == 1) valorUpgrade = 5;      // Facil
            else if(game.difficulty == 2) valorUpgrade = 3; // Media
            else if(game.difficulty == 3) valorUpgrade = 2; // Dificil

            if(game.attSelection==0){
                game.map.player.attributes.hpMax += valorUpgrade;

            }
            if(game.attSelection==1) game.map.player.attributes.defense += valorUpgrade;
            if(game.attSelection==2) game.map.player.attributes.strength += valorUpgrade;
            if(game.attSelection==3) game.map.player.attributes.intelligence += valorUpgrade;
            if(game.attSelection==4) game.map.player.attributes.dexterity += valorUpgrade;

            game.map.player.attPoints--;
        }
    }
}



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void create_map(GAME &game){
    if(game.next==true){
        game.map.player.firstAtt = true;
        game.map.player.attPoints++;

    // --- BÔNUS DE HP MÁXIMO POR FASE ---


        int bonusHp = 0;
        if(game.difficulty == 1) bonusHp = 10;      // facil
        else if(game.difficulty == 2) bonusHp = 5;  // media
        else if(game.difficulty == 3) bonusHp = 2;  // dificil

        game.map.player.attributes.hpMax += bonusHp;


    }
    game.next = false;
    game.map.player.key = false;
    if(!game.map.player.fallen){
        new_line("┏","━","┓",30);
        new_line("┃"," ","┃",30);
        new_line("┃"," ","┃",30);
        new_line("┃"," ","┃",30);
        new_line("┃"," ","┃",30);
        new_line("┃"," ","┃",30);
        new_line("┃"," ","┃",30);
        new_line("┃"," ","┃",30);
        new_line("┗","━","┛",30);
        while(true){
            put_attributes(game);
            if(game.map.player.attPoints<=0){
                put_attributes(game);
                break;
            }
        }
    }
    cout<<"\ec";
    cout<<"CARREGANDO...";
    if(game.map.player.attPoints<=0){
        game.map.player.clockSpeed = clock();
        while((clock()-game.map.player.clockSpeed)<1000){}
    }


    for(int y=0;y<MAPSIZEY;y++){
        for(int x=0;x<MAPSIZEX;x++){
            game.map.tiles[y][x] = EMPTY;
            game.map.memory[y][x] = 0;
            if(game.map.floor==6){
                game.map.memory[y][x] = 1;
            }
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
        if(game.map.floor==6){
            tamY = 25;
            tamX = 25;
        }
        for(int y=-tamY;y<tamY;y++){
            for(int x=-tamX;x<tamX;x++){
                if(Y+y>=0 && Y+y<MAPSIZEY && X+x>=0 && X+x<MAPSIZEX){
                    if(!(y==-tamY || y==tamY-1 || x==-tamX || x==tamX-1)){
                        game.map.tiles[Y+y][X+x] = FREEBLOCK;
                        if(game.map.floor==6){
                            if(rand()%10==0){
                                game.map.tiles[Y+y][X+x] = EMPTY;
                            }
                        }
                    }
                    if(game.map.floor!=6){
                        if(game.map.tiles[Y+y][X+x]==EMPTY){
                            if(y==-tamY || y==tamY-1 || x==-tamX || x==tamX-1){
                                game.map.tiles[Y+y][X+x] = SOLIDBLOCK;
                            }
                        }
                    }
                }
            }
        }
        game.map.tiles[game.map.player.pos.Y][game.map.player.pos.X] = FREEBLOCK;
        if(game.map.floor==6){
            break;
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
    if(game.map.floor!=6){
        while(true){
            int y = rand()%MAPSIZEY;
            int x = rand()%MAPSIZEX;
            if(game.map.tiles[y][x]==FREEBLOCK){
                game.map.tiles[y][x] = KEYBLOCK;
                break;
            }
        }
        for(int i=0;i<250000;i++){
            int y = rand()%MAPSIZEY;
            int x = rand()%MAPSIZEX;
            bool livre = (
                game.map.tiles[y-1][x] == FREEBLOCK &&
                game.map.tiles[y+1][x] == FREEBLOCK &&
                game.map.tiles[y][x-1] == FREEBLOCK &&
                game.map.tiles[y][x+1] == FREEBLOCK &&

                game.map.tiles[y-1][x+1] == FREEBLOCK &&
                game.map.tiles[y+1][x-1] == FREEBLOCK &&
                game.map.tiles[y-1][x-1] == FREEBLOCK &&
                game.map.tiles[y+1][x+1] == FREEBLOCK
            );
            if(game.map.tiles[y][x]==FREEBLOCK && (y != game.map.player.pos.Y || x != game.map.player.pos.X) && livre){
                game.map.tiles[y][x] = NPCBLOCK;
                break;
            }
        }
    }

    for(int monster=0;monster<50;monster++){
        MONSTER resetMonsters[50];
        game.map.monsters[monster] = resetMonsters[monster];
    }
    for(int monster=0;monster<50;monster++){
        int floor = game.map.floor*2;
        if(floor>10){
            floor = 10;
        }
        if(rand()%(11-floor)==0 || monster==0){
            int posY;
            int posX;
            bool success = false;
            int attPoints = (game.map.player.nivel+game.map.floor)*2;
            game.map.monsters[monster].id = rand()%floor;
            if(game.map.floor==6 && monster==0){
            game.map.monsters[monster].id = 10;

            game.map.monsters[monster].attributes.hp = game.map.monsters[monster].attributes.hpMax;

            }
            while(attPoints>0){
                int attribute = rand()%5;
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
                    if(rand()%10==0){
                        attPoints++;
                        continue;
                    }
                    if(rand()%2==0){
                        attribute = 2;
                    }
                    if(rand()%2==0){
                        attribute = 0;
                    }
                }
                if(attribute==0){
                    int hp = game.map.monsters[monster].attributes.hpMax;
                    game.map.monsters[monster].attributes.hpMax+=rand()%hp+1;

                if(game.map.monsters[monster].id == 10) {
                    if(game.difficulty == 1) game.map.monsters[monster].attributes.hpMax = 500;
                    else if(game.difficulty == 2) game.map.monsters[monster].attributes.hpMax = 800;
                    else if(game.difficulty == 3) game.map.monsters[monster].attributes.hpMax = 1000;
                }


                }
                if(attribute==1){
                    game.map.monsters[monster].attributes.defense++;
                    if(rand()%10==0){
                        game.map.monsters[monster].attributes.defense++;
                    }
                }
                if(attribute==2){
                    game.map.monsters[monster].attributes.strength++;
                    if(rand()%10==0){
                        game.map.monsters[monster].attributes.strength++;
                    }
                }
                if(attribute==3){
                    game.map.monsters[monster].attributes.intelligence++;
                    if(rand()%10==0){
                        game.map.monsters[monster].attributes.intelligence++;
                    }
                }
                if(attribute==4){
                    game.map.monsters[monster].attributes.dexterity++;
                    if(rand()%10==0){
                        game.map.monsters[monster].attributes.dexterity++;
                    }
                }
                attPoints--;
            }
            for(int i=0;i<250000;i++){
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
                if(success){
                    break;
                }
            }
            if(success){
                game.map.monsters[monster].pos.Y = posY;
                game.map.monsters[monster].pos.X = posX;

                int bonus = game.difficulty - 1; // Aplica o bônus de atributos aos monstros de acordo com a dificuldade.

                game.map.monsters[monster].attributes.hpMax += bonus;
                game.map.monsters[monster].attributes.defense += bonus;
                game.map.monsters[monster].attributes.strength += bonus;
                game.map.monsters[monster].attributes.intelligence += bonus;
                game.map.monsters[monster].attributes.dexterity += bonus;
                game.map.monsters[monster].attributes.hp = game.map.monsters[monster].attributes.hpMax;
                game.map.monsters[monster].alive = true;
            }else{
                game.map.monsters[monster].pos.Y = -1;
                game.map.monsters[monster].pos.X = -1;
                game.map.monsters[monster].alive = false;
            }
        }else{
            continue;
        }
    }
    for(int item=0;item<10;item++){
        game.map.items[item].Y = -1;
        game.map.items[item].X = -1;
    }
    if(game.map.floor!=6){
        for(int item=0;item<10;item++){
            if(rand()%4==0 || item==0){
                if(item+1>game.map.floor){
                    break;
                }
                for(int i=0;i<250000;i++){
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
    }

    // --- GERADOR DE ARMADILHAS ANTI STUN LOCK (ANTI-LOCK / TIMEOUT) ---
    int qtdArmadilhas = (rand()%game.map.floor+1)*3;
    int armadilhasCriadas = 0;
    while(armadilhasCriadas < qtdArmadilhas){
        bool colocou = false;
        for(int i=0;i<250000;i++){
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
        }
        // Se após 1000 tentativas o mapa atual não tiver espaço grande o bastante,
        // quebra o loop principal para impedir que o jogo trave infinitamente.
        if(!colocou){
            armadilhasCriadas++;
            continue;
        }
    }
    if(game.map.player.fallen){
        while(true){
            int y = rand()%MAPSIZEY;
            int x = rand()%MAPSIZEX;
            if(game.map.tiles[y][x]==FREEBLOCK){
                game.map.player.pos.Y = y;
                game.map.player.pos.X = x;
                break;
            }
        }
    }
    game.map.player.fallen = false;
}
