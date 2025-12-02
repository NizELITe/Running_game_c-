// runner.cpp
#include <bits/stdc++.h>
#include <conio.h>
#include <windows.h>
using namespace std;

const int MAP_WIDTH = 3;       // lanes
const int MAP_HEIGHT = 20;     // vertical size shown
const int PLAYER_ROW = MAP_HEIGHT - 3; // base player row
const int INITIAL_LIVES = 3;
const int INITIAL_DELAY = 140; // milliseconds between frames
const int COIN_SCORE = 10;

struct Entity {
    int lane;   // 0..2
    int row;    // 0..MAP_HEIGHT-1 (0 at top)
    bool isCoin;
    Entity(int l=0,int r=0,bool c=false):lane(l),row(r),isCoin(c){}
};

struct HighScore {
    string name;
    int score;
};

void clamp_lane(int &l){ if(l<0) l=0; if(l>MAP_WIDTH-1) l=MAP_WIDTH-1; }

void draw_screen(const vector<Entity>& ents, int playerLane, int playerRow, int lives,
                 int score, int distance, bool showChaser, int chaserLane, int chaserRow, bool paused){
    // Clear console (Windows)
    system("cls");
    // Top HUD
    cout << "Lives: " << lives << "   Score: " << score << "   Distance: " << distance << "m";
    if(paused) cout << "   [PAUSED]";
    cout << "\n";
    // draw map rows
    for(int r=0;r<MAP_HEIGHT;r++){
        string line;
        for(int lane=0;lane<MAP_WIDTH;lane++){
            char c = ' ';
            // check entities
            for(const auto &e: ents){
                if(e.row==r && e.lane==lane){
                    c = e.isCoin ? 'o' : 'X';
                    break;
                }
            }
            // player
            if(r==playerRow && lane==playerLane) c = 'P';
            // chaser
            if(showChaser && r==chaserRow && lane==chaserLane && !(r==playerRow && lane==playerLane)) c = 'C';
            // side walls char spacing
            cout << "|" << c;
        }
        cout << "|\n";
    }
    cout << "Controls: A-left D-right  W-forward  S-backward  P-pause  R-resume  Q-quit\n";
}

void save_highscore(const string &name, int score){
    // append to file
    ofstream ofs("highscores.txt", ios::app);
    if(!ofs) return;
    ofs << name << " " << score << "\n";
    ofs.close();
}

vector<HighScore> load_highscores(){
    vector<HighScore> out;
    ifstream ifs("highscores.txt");
    if(!ifs) return out;
    while(!ifs.eof()){
        HighScore h; ifs >> h.name >> h.score;
        if(ifs) out.push_back(h);
    }
    sort(out.begin(), out.end(), [](const HighScore&a,const HighScore&b){ return a.score>b.score;});
    return out;
}

int main_menu(){
    while(true){
        system("cls");
        cout << "=== RUNNER GAME ===\n1. Start Game\n2. Show High Scores\n3. Quit\nEnter choice: ";
        char c = getch();
        if(c=='1') return 1;
        if(c=='2') return 2;
        if(c=='3' || c=='q' || c=='Q') return 3;
    }
}

int main(){
    srand((unsigned)time(nullptr));
    while(true){
        int choice = main_menu();
        if(choice==3) break;
        if(choice==2){
            system("cls");
            auto hs = load_highscores();
            cout << "High Scores:\n";
            for(size_t i=0;i<hs.size() && i<10;i++){
                cout << i+1 << ". " << hs[i].name << " - " << hs[i].score << "\n";
            }
            cout << "Press any key to return...\n";
            getch();
            continue;
        }

        // START GAME
        system("cls");
        cout << "Enter your name (no spaces): ";
        string name;
        cin >> name;

        int playerLane = 1;
        int playerRow = PLAYER_ROW;
        int lives = INITIAL_LIVES;
        int score = 0;
        int distance = 0;
        int delay = INITIAL_DELAY;
        vector<Entity> ents;
        int frameCount = 0;
        bool paused = false;
        bool showChaser = false;
        int chaserLane = 1, chaserRow = 0;
        int chaserTimer = 0; // controls chaser movement frame rate

        // Game loop
        bool running = true;
        while(running){
            // input handling (non-blocking)
            if(kbhit()){
                char key = getch();
                if(key=='a' || key=='A'){ playerLane--; clamp_lane(playerLane); }
                if(key=='d' || key=='D'){ playerLane++; clamp_lane(playerLane); }
                if(key=='w' || key=='W'){ if(playerRow>0) playerRow--; }
                if(key=='s' || key=='S'){ if(playerRow<PLAYER_ROW) playerRow++; }
                if(key=='p' || key=='P'){ paused = true; }
                if(key=='r' || key=='R'){ paused = false; }
                if(key=='q' || key=='Q'){ running=false; break; }
            }

            if(paused){
                draw_screen(ents, playerLane, playerRow, lives, score, distance, showChaser, chaserLane, chaserRow, paused);
                Sleep(100);
                continue;
            }

            // spawn new entities occasionally
            if(frameCount % 8 == 0){
                int spawnChance = rand()%100;
                if(spawnChance < 60){
                    // obstacle
                    int lane = rand()%MAP_WIDTH;
                    ents.emplace_back(lane, 0, false);
                } else {
                    // coin
                    int lane = rand()%MAP_WIDTH;
                    ents.emplace_back(lane, 0, true);
                }
            }

            // move entities down
            for(auto &e: ents) e.row++;
            // remove off-map & count distance when obstacle passes bottom
            vector<Entity> next;
            for(auto &e: ents){
                if(e.row >= MAP_HEIGHT){
                    // entity passed player: if it was coin and not collected -> do nothing
                    distance += 5; // count some meters for each passed row batch
                } else next.push_back(e);
            }
            ents.swap(next);

            // detect collisions & collect coins
            bool collided = false;
            for(size_t i=0;i<ents.size();){
                if(ents[i].row==playerRow && ents[i].lane==playerLane){
                    if(ents[i].isCoin){
                        score += COIN_SCORE;
                        // remove coin
                        ents.erase(ents.begin()+i);
                        continue;
                    } else {
                        // obstacle hit
                        lives--;
                        collided = true;
                        // remove obstacle
                        ents.erase(ents.begin()+i);
                        if(lives<=0) running=false;
                        continue;
                    }
                } else ++i;
            }

            // speed increase by milestones
            if(distance >= 1000 && delay>100) delay = 110;
            if(distance >= 2000 && delay>80) delay = 85;
            if(distance >= 5000 && delay>60) delay = 60;

            // introduce chaser after 2000m
            if(distance >= 2000) {
                showChaser = true;
                // chaser appears at top gradually
                if(chaserRow < PLAYER_ROW - 6) chaserRow++;
                // make chaser follow player's lane slowly
                chaserTimer++;
                if(chaserTimer % 6 == 0){
                    if(chaserLane < playerLane) chaserLane++;
                    else if(chaserLane > playerLane) chaserLane--;
                }
                // if chaser reaches player row and same lane => collision
                if(chaserRow == playerRow && chaserLane == playerLane){
                    lives--;
                    // push chaser back a bit
                    chaserRow = max(0, chaserRow - 5);
                    if(lives<=0) running=false;
                }
            }

            // increase distance gradually each frame
            distance += 1;

            // draw
            draw_screen(ents, playerLane, playerRow, lives, score, distance, showChaser, chaserLane, chaserRow, paused);

            // small auto-step: slowly move player forward each few frames (auto-run)
            if(frameCount % 40 == 0 && playerRow > 0) playerRow--; // small forward drift
            // but keep within base row
            if(playerRow < PLAYER_ROW) {
                // gradually move player back down (simulate running forward)
                if(frameCount % 6 == 0) playerRow++;
                if(playerRow > PLAYER_ROW) playerRow = PLAYER_ROW;
            }

            frameCount++;
            Sleep(delay);
        } // end game loop

        // Game over
        system("cls");
        cout << "Game Over!\nScore: " << score << "   Distance: " << distance << "m\n";
        cout << "Enter name to save score or press ENTER to skip: ";
        string saveName;
        // flush any trailing newline
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        getline(cin, saveName);
        if(saveName.size()>0){
            save_highscore(saveName, score);
            cout << "Saved!\n";
        } else {
            cout << "Not saved.\n";
        }
        cout << "Press any key to return to menu...\n";
        getch();
    } // menu loop

    cout << "Thanks for playing!\n";
    return 0;
}

