#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <fstream>

struct Position{
    int X, Y;
};
struct Character{
    std::string name = "player #";
    int lives = 100, armor = 30.0f, damage = 20.0f;
    Position position;
} player;
enum Direction{TOP, RIGHT, BOTTOM, LEFT};

void InitializingPlayingField(std::vector<std::vector<char>>& playing_field){
    for (int y = 0; y < 40; ++y) {
        playing_field.push_back(std::vector<char>());
        for (int x = 0; x < 40; ++x) {
            playing_field[y].push_back('.');
        }
    }
}
void CreateCharacterEnemy(std::vector<std::vector<char>>& playing_field, std::vector<Character>& players){
    std::srand(std::time(nullptr));
    for (auto count_player = 0; count_player < 5; ++count_player){
        players.push_back(player);
        players[count_player] = {"Enemy #" + std::to_string(count_player), rand()%101 + 50,
              rand()%51, rand()%16 + 15};
        players[count_player].position.X = rand()%40, players[count_player].position.Y = rand()%40;
        if (playing_field[players[count_player].position.X][players[count_player].position.Y] != '.'){
            count_player--;
            continue;
        }
        else playing_field[players[count_player].position.Y][players[count_player].position.X] = 'E';
     }
}
void CreateCharacterUser(std::vector<std::vector<char>>& playing_field, Character& user_player){
    std::srand(std::time(nullptr));
    std::cout << "Input name player, lives, armor, damage: ";
    std::cin >> user_player.name >> user_player.lives >> user_player.armor >> user_player.damage;
    while(true){
        user_player.position.X = rand()%40, user_player.position.Y = rand()%40;
        if(playing_field[user_player.position.Y][user_player.position.X] == '.') {
            playing_field[user_player.position.Y][user_player.position.X] = 'P';
            break;
        }
    }

}
void show_playing_field(const std::vector<std::vector<char>>& playing_field){
    for (int y = 0; y < 40; ++y) {
        for (int x = 0; x < 40; ++x) {
            std::cout << playing_field[y][x];
        }
        std::cout << std::endl;
    }
}
bool checking_going_out_field(std::vector<std::vector<char>>& playing_field, const Position* copy_pos){
    return (copy_pos->X < 0 || copy_pos->X > 39) || (copy_pos->Y < 0 || copy_pos->Y > 39);
};
bool Checking_hit_occupied_place(std::vector<std::vector<char>>& playing_field, const Position* copy_pos){
    return playing_field[copy_pos->Y][copy_pos->X] != '.';
}
void Attack(Character& attacking_character, Character defending_character){
    defending_character.armor -= attacking_character.damage;
    defending_character.lives += defending_character.armor;
    if (defending_character.armor < 0) defending_character.armor = 0;
}
int find_characterDefending(const std::vector<Character>& character, const  Position& copy_pos){
    for (int numb_character = 0; numb_character < character.size(); ++numb_character) {
        if (character[numb_character].position.X == copy_pos.X && character[numb_character].position.Y == copy_pos.Y)
            return numb_character;
    }
    return -1;
}
bool isAttacking(std::vector<std::vector<char>>& playing_field, Position copy_pos, Character attacking_character){
    if (attacking_character.name[0] == 'E') return playing_field[copy_pos.Y][copy_pos.X] == 'P';
    return false;
}
void move(std::vector<std::vector<char>>& playing_field, Character& character, Position copy_pos){
    playing_field[character.position.Y][character.position.X] = '.';
    character.position.X = copy_pos.X, character.position.Y = copy_pos.Y;
    playing_field[character.position.Y][character.position.X] = (character.name[0] == 'E' ? 'E' : 'P');
}
void move_user(std::vector<std::vector<char>>& playing_field, Character& user_character,
               std::vector<Character>& enemy_character, const std::string& direction){
    Position new_pos {user_character.position.X, user_character.position.Y};
    if (direction == "top") new_pos.Y -= 1;
    else if(direction == "right") new_pos.X += 1;
    else if(direction == "bottom") new_pos.Y += 1;
    else if (direction == "left") new_pos.X -= 1;
    if (checking_going_out_field(playing_field, &new_pos)){
        std::cout << "Going out of bounds!\n" << std::endl;
        return;
    }
    if(Checking_hit_occupied_place(playing_field, &new_pos)) {
        Attack(user_character, enemy_character[find_characterDefending(enemy_character, new_pos)]);
        return;
    }
    move(playing_field, user_character, new_pos);
}
void move_enemy(std::vector<std::vector<char>>& playing_field, Character& enemy_character, Character& character_user){
    std::srand(std::time(nullptr));
    Position new_pos {enemy_character.position.X, enemy_character.position.Y};
    int direction = rand()%4;
    if (direction == TOP) new_pos.Y -= 1;
    else if(direction == RIGHT) new_pos.X += 1;
    else if(direction == BOTTOM) new_pos.Y += 1;
    else if (direction == LEFT) new_pos.X -= 1;
    if (checking_going_out_field(playing_field, &new_pos)) return;
    if (Checking_hit_occupied_place(playing_field, &new_pos)){
        if(isAttacking(playing_field, new_pos, enemy_character)) Attack(enemy_character, character_user);
        return;
    }
    move(playing_field, enemy_character, new_pos);
}
void SavePlayingField(const std::vector<std::vector<char>>& playing_field, const std::string& path){
    std::ofstream playingFieldTxt(path, std::ios::binary);
    for (int Y = 0; Y < 40; ++Y) {
        for (int X = 0; X < 40; ++X) {
            playingFieldTxt << playing_field[Y][X];
        }
        playingFieldTxt << std::endl;
    }
    playingFieldTxt.close();
}
void SaveDataPlayer(const std::vector<Character>& character, const std::string& path){
    std::ofstream dataEnemyPlayer(path, std::ios::binary);
    for (int numb_character = 0; numb_character < character.size(); ++numb_character) {
        dataEnemyPlayer << "Name: " << character[numb_character].name << std::endl;
        dataEnemyPlayer << "Armor:" << character[numb_character].armor << std::endl;
        dataEnemyPlayer << "Lives: " << character[numb_character].lives << std::endl;
        dataEnemyPlayer << "Damage: " << character[numb_character].damage << std::endl;
        dataEnemyPlayer << "X: " << character[numb_character].position.X << " Y:" <<
                        character[numb_character].position.Y << std::endl;
    }
    dataEnemyPlayer.close();
}
void save(const std::vector<std::vector<char>>& playing_field, const std::vector<Character>& character_enemy,
          const Character& character_user){
    SaveDataPlayer(character_enemy, "C:\\Users\\Malip\\source\\repos\\Turn-Based_Role-Playing_Game\\DataEnemyTeams.bin");
    SaveDataPlayer(std::vector<Character>{character_user},
                   "C:\\Users\\Malip\\source\\repos\\Turn-Based_Role-Playing_Game\\DataUser.bin");
    SavePlayingField(playing_field, "C:\\Users\\Malip\\source\\repos\\Turn-Based_Role-Playing_Game\\PlayingFile.bin");
}
void LoadPlayingField(std::vector<std::vector<char>>& playing_field, const std::string& path){
    std::ifstream playingFieldTxt(path ,std::ios::binary);
    Position pos{0, 0};
    while(!playingFieldTxt.eof() && pos.Y < 40){
        char symbol;
        playingFieldTxt >> symbol;
        if (symbol != ' ' && symbol != '\n') {
            playing_field[pos.Y][pos.X] = symbol;
            pos.X++;
        }
        if (pos.X == 40) pos.X = 0, pos.Y++;
    }
    playingFieldTxt.close();
}
void LoadDataPlayer(Character& character, const std::string& path, const int& numb_character = 0){
    std::ifstream dataEnemyPlayer(path, std::ios::binary);
    std::string unnecessary_inf;
    for(int numb = 0; !dataEnemyPlayer.eof() && numb <= numb_character; ++numb) {
        dataEnemyPlayer >> unnecessary_inf >> character.name >> unnecessary_inf;
        dataEnemyPlayer >> unnecessary_inf >> character.armor >> unnecessary_inf;
        dataEnemyPlayer >> unnecessary_inf >> character.lives >> unnecessary_inf;
        dataEnemyPlayer >> unnecessary_inf >> character.damage >> unnecessary_inf;
        dataEnemyPlayer >> unnecessary_inf >> character.position.X >> unnecessary_inf >>
                        character.position.Y >> unnecessary_inf;
    }
    dataEnemyPlayer.close();
}
void load(std::vector<std::vector<char>>& playing_field, std::vector<Character>& character_enemy,
          Character& character_user){
   LoadPlayingField(playing_field, "C:\\Users\\Malip\\source\\repos\\Turn-Based_Role-Playing_Game\\PlayingFile.bin");
   for (int numb_character = 0; numb_character < character_enemy.size(); ++numb_character){
       LoadDataPlayer(character_enemy[numb_character],
                      "C:\\Users\\Malip\\source\\repos\\Turn-Based_Role-Playing_Game\\DataEnemyTeams.bin", numb_character);
   }
   LoadDataPlayer(character_user,
                  "C:\\Users\\Malip\\source\\repos\\Turn-Based_Role-Playing_Game\\DataUser.bin");
}

int main() {
    std::vector<std::vector<char>> playing_field;
    InitializingPlayingField(playing_field);
    std::vector<Character> enemy_character;
    Character user_character;
    CreateCharacterEnemy(playing_field, enemy_character);
    CreateCharacterUser(playing_field, user_character);
    while (!enemy_character.empty()){
        std::string action;
        std::cout << "Input where you want to move to(top, right, bottom, left) or (load/save/exit): ";
        std::cin >> action;
        if (action == "load"){
            load(playing_field, enemy_character, user_character);
        }else if(action == "save") {
            save(playing_field, enemy_character, user_character);
        }
        else if(action == "exit"){
            return 0;
        }else{
            move_user(playing_field, user_character, enemy_character, action);
            for (auto enemy_numb = 0; enemy_numb < enemy_character.size(); ++enemy_numb)
                move_enemy(playing_field, enemy_character[enemy_numb], user_character);
        }
        show_playing_field(playing_field);

    }

}
