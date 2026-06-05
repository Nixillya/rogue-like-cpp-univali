#include "game.h"

// Teste

int menu(GAME &game){
    return 0;
}

int play(GAME &game){
    create_map(game);
    while(game.play==1){
        render_map(game);
        player_input(game);
    }
    return 0;
}
int main(){
    SetConsoleOutputCP(CP_UTF8);
    GAME game;
    srand(time(0)); 
    while(game.exit==0){
        if(game.play==0){
            menu(game);
        }
        if(game.play==1){
            play(game);
        }
    }
    return 0;
}