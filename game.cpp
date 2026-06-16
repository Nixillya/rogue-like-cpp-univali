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
    while(true){
        if(game.map.player.inventoryOpened && game.map.player.attributes.hp>0){
            show_inventory(game);
        }else{
            render_map(game);
        }
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