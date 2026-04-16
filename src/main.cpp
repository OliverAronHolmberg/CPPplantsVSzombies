#include <raylib.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>




Camera2D OpenWindow(int gridW, int gridH, int tileSize){

    float mapW = gridW * tileSize;
    float mapH = gridH * tileSize;
    

    InitWindow(0, 0, "Plants VS Zombies");
    int monitor = GetCurrentMonitor();
    // int monitorW = GetMonitorWidth(monitor);
    // int monitorH = GetMonitorHeight(monitor);
    int monitorW = 1080*2;
    int monitorH = 720*2;
    int winW = monitorW/2;
    int winH = monitorH/2;

    SetWindowSize(winW, winH); 
    SetWindowPosition(monitorW/6, monitorH/8);

    Camera2D camera = { 0 };
    camera.target.x = mapW / 2.0f;
    camera.target.y = mapH / 2.0f;
    camera.offset.x = winW/2.0f;
    camera.offset.y = winH/2.0f-tileSize/2.0f;
    
    
    camera.zoom = 1.0f;

    return camera;
}

class TextureLoader{

    
    

    public:
    std::map<std::string, Texture2D> textures = {};
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

class Entity{
    protected:
    int x;
    int y;
    


    public: 
    std::string typeID;
    Rectangle rec;
    Entity(int posX, int posY, std::string ID, Rectangle rectangle) : x(posX), y(posY), typeID(ID), rec(rectangle){}

    void DrawEntity(Texture2D texture ){
        Rectangle sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height};
        Vector2 origin = {0, 0};
        
        DrawTexturePro(texture, sourceRec, rec, origin, 0.0f, WHITE);
    }

    virtual ~Entity() = default;
};

class Plant : public Entity{
    int sunCost;

    public: 
    Plant(int posX, int posY, std::string ID, int cost, Rectangle rec) : Entity(posX, posY, ID, rec){
        sunCost = cost;
    }

};

class Sunflower : public Plant{
    int hp;

    public:
    Sunflower(int posX, int posY, int cost, int health, Rectangle rec) : Plant(posX, posY, "SUNFLOWER", cost, rec){
        hp = health;
    }
};


class Card{
    protected:
    int width;
    int height;
    Texture2D texture;
    Rectangle rec;
    public:

    int getWidth() const {return width;}
    int getHeight() const {return height;}

    Card(int Width, int Height, Texture2D Texture){
        width = Width;
        height = Height;
        texture = Texture;
    }

    void DrawCard(int x, int y){
        rec = {(float)x, (float)y, (float)width, (float)height};
        Rectangle sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height};
        Vector2 origin = {0, 0};
        DrawRectangle(x, y, width, height, BLACK);
        DrawTexturePro(texture, sourceRec, rec, origin, 0.0f, WHITE);
    }
};



class Inventory{
    int x;
    int y;
    int width;
    int height;
    int spacing = 10;
    std::vector<Card> cards = {};
    
    public:
    Inventory(int posX, int posY){
        x = posX;
        y = posY;
        
    }

    void AddCardToInventory(Card card){
        cards.push_back(card);
        width = card.getWidth()*cards.size() + (spacing*(cards.size()-1)) + 20;
        height = card.getHeight() + 20;
    }

    void DrawInventory(){
        DrawRectangle(x, y, width, height, GREEN);
        for (int i = 0; i < cards.size(); i++){
            cards[i].DrawCard(this->x + 10 + (i*(cards[i].getWidth() + spacing)), this->y+10);
        }
    }
    
};



class Player{

    std::string selectedType;
    TextureLoader textures;
    std::vector<std::unique_ptr<Entity>>& entityVector;
    Camera2D camera;
    public:
    Player(std::string Type, TextureLoader textureloader,std::vector<std::unique_ptr<Entity>>& entityList, Camera2D playerCam)
        : selectedType(Type), textures(textureloader), entityVector(entityList), camera(playerCam){}
    
    void Interaction(std::vector<Tile>& tileVector){
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
            for(int i = 0; i < tileVector.size(); i++){
                if(CheckCollisionPointRec(mousePos, tileVector[i].rec)){
                    tileVector[i].color = BLUE;
                    float x = tileVector[i].rec.x;
                    float y = tileVector[i].rec.y;
                    PlantPlant(x, y, tileVector[i].rec);
                }
            }
            
        }
    }

    void PlantPlant(int x, int y, Rectangle rec){
        if(textures.textures.find(selectedType) != textures.textures.end()){
            if(selectedType == "SUNFLOWER"){
                entityVector.push_back(std::make_unique<Sunflower>(x, y, 100, 100, rec));
            }
            else{

            }
        }
    }
};




class Level{
    
    std::vector<Card> levelCards;
    
    public:
    Level(std::vector<std::string> CardIDList, TextureLoader& textures){
        for (auto item : CardIDList){
            Card card(100, 100, textures.Get(item));
            levelCards.push_back(card);
        }
    }

    void LoadInventory(Inventory& inventory){
        for (int i = 0; i < levelCards.size(); i++)
        inventory.AddCardToInventory(levelCards[i]);
        
    }
    
    
};

class LevelHandler{
    std::vector<Level> levels;
    public:


    void AddLevel(Level level){
        levels.push_back(level);
    }

    Level LoadLevel(int levelNumber){
        Level level = levels[levelNumber-1];
        return level;
    }

    


};





int main(){

    int gridW = 9;
    int gridH = 5;
    int tileSize = 100;
    Camera2D camera = OpenWindow(gridW, gridH, tileSize);

    TextureLoader textures;
    LoadTextures(textures);

    Inventory inventory(camera.target.x- camera.offset.x + 10, camera.target.y - camera.offset.y + 10);


    // Level Intitializer
    
    Level* level1 = new Level({"SUNFLOWER", "SUNFLOWER"}, textures);
    level1->LoadInventory(inventory);

    
    

    std::vector<Tile> tileVector = {};
    for (int y = 0; y <= gridH; y++){
       for (int x = 0; x < gridW; x++){
        Tile tile(x*tileSize, y*tileSize, tileSize);
        tileVector.push_back(tile);
       }
    }
     
    

    std::vector<std::unique_ptr<Entity>> entityVector = {};
    

    Player player("SUNFLOWER", textures, entityVector, camera);
    
    

    while(!WindowShouldClose()){
        BeginDrawing();

        ClearBackground(WHITE);
        
        BeginMode2D(camera);

        for (int i = 0; i < tileVector.size(); i++){
            tileVector[i].DrawTile();
        }
        for (int i = 0; i < entityVector.size(); i++){
            std::string ID = entityVector[i]->typeID;
            Texture2D tex = textures.Get(ID);
            entityVector[i]->DrawEntity(tex);
        }

        player.Interaction(tileVector);

        inventory.DrawInventory();

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();



    return 0;
}