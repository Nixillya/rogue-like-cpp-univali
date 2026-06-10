#include "game.h"

void menu(GAME &game){
    menu_render(game);
    if (game.codex == true) {
        game.menu.optionVertical = 4;
        while (game.codex == true) {
            codex_render(game);
        }
    }
}

void play(GAME &game){
    srand(time(0)); 
    create_map(game);
    game.next = false;
    if(game.map.level==1){
        clear_inventory(game);
    }
    while(game.play==true && game.next==false){
        render_map(game);
        player_verifiers(game);
        player_input(game);
        move_monsters(game);
    }
}

int main(){
    SetConsoleOutputCP(CP_UTF8);
    GAME game;
    
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