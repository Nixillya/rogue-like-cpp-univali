#include "game.h"

int menu(GAME &game){
        menu_render(game);
    return 0;
}

int play(GAME &game){
    create_map(game);
    while(game.play==true){
        render_map(game);
        player_verifiers(game);
        player_input(game);
    }
    return 0;
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