#include "game.h"

void menu(GAME &game){
        menu_render(game);
}

void play(GAME &game){
    create_map(game);
    game.next = false;
    while(game.play==true && game.next==false){
        render_map(game);
        player_verifiers(game);
        player_input(game);
    }
}
int main(){
    SetConsoleOutputCP(CP_UTF8);
    GAME game;
    srand(time(0)); 
    while(game.exit==false){
        if(game.play==false){
            menu(game);
        }
        if(game.play==true){
            play(game);
        }
    }
    return 0;
}