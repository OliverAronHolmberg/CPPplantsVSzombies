#include <raylib.h>
#include <iostream>
#include <vector>
#include <map>


void OpenWindow(){

     InitWindow(0, 0, "Plants VS Zombies");
    int monitor = GetCurrentMonitor();
    int monitorW = GetMonitorWidth(monitor);
    int monitorH = GetMonitorHeight(monitor);
    int winW = monitorW/2;
    int winH = monitorH/2;

    SetWindowSize(winW, winH);
    SetWindowPosition(monitorW/4, monitorH/4);
}

class TextureLoader{

    std::map<std::string, Texture2D> textures = {};

    public:
    void Load(std::string ID, std::string Path){
       if(textures.find(ID) == textures.end()){
        Texture2D texture = LoadTexture(Path.c_str());
        textures[ID] = texture;
       }
    }

    Texture2D Get(std::string ID){
        Texture2D tex = textures.at(ID);
        return tex;
    }


};

void LoadTextures(TextureLoader& textures){
    textures.Load("SUNFLOWER", "resources/sunflower.png");
}

class Tile{
    float x;
    float y;
    float RecLength;
    
    public:
    Rectangle rec;
    Color color = RED;

    Tile(float posX, float posY, float RecSideLength){
        x = posX;
        y = posY;
        RecLength = RecSideLength;
        rec = {x, y, RecLength, RecLength};
        
    }

    void DrawTile(){
        DrawRectangleRounded(rec, 0.1f, 1.0f, color);
    }
};

class Player{


    public:
    Player(){

    }
    void Interaction(std::vector<Tile>& tileVector){
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            Vector2 mousePos = GetMousePosition();
            for(int i = 0; i < tileVector.size(); i++){
                if(CheckCollisionPointRec(mousePos, tileVector[i].rec)){
                    tileVector[i].color = BLUE;
                }
            }
            
        }
    }
};


class Entity{
    protected:
    int x;
    int y;


    public: 
    Entity(int posX, int posY){
        x = posX;
        y = posY;
    }

    void DrawEntity(Texture2D texture){
        Vector2 pos = {x, y};
        DrawTextureEx(texture, pos, 0.0f, 1.0f, WHITE);
    }
};

class Plant : public Entity{
    int sunCost;

    public: 
    Plant(int posX, int posY, int cost) : Entity(posX, posY){
        sunCost = cost;
    }

};

class Sunflower : public Plant{
    int hp;
    const std::string ID = "SUNFLOWER";

    public:
    Sunflower(int posX, int posY, int cost, int health) : Plant(posX, posY, cost){
        hp = health;
    }
};









int main(){

    OpenWindow();

    TextureLoader textures;
    LoadTextures(textures);

    Player player;
    std::vector<Tile> tileVector = {};
    Tile tile(100, 100, 100);
    tileVector.push_back(tile);

    while(!WindowShouldClose()){
        BeginDrawing();

        ClearBackground(WHITE);

        for (int i = 0; i < tileVector.size(); i++){
            tileVector[i].DrawTile();
        }
        player.Interaction(tileVector);

        EndDrawing();
    }

    CloseWindow();



    return 0;
}