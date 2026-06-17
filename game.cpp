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
    cout<<"\ec";
    srand(time(0)); 
    create_map(game);
    game.map.player.attributes.hp = 999;
    game.map.player.attributes.hpMax = 999;
    game.map.player.attributes.strength = 999;
    while(true){
        render_map(game);
        move_player(game);
        if(!game.play || game.next){
            return;
        }
        move_monsters(game);
    }
}

int main(){
    SetConsoleOutputCP(CP_UTF8);
    static GAME game;
    while(game.exit==false){
        if(game.play==false){
            if(game.next){
                cout<<"\ec";
                game.next = false;
            }
            MAP map;
            game.map = map;
            menu(game);
        }
        if(game.play==true){
            play(game);
        }
    }
    return 0;
}