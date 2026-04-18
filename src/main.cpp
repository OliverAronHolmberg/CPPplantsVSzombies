#include <raylib.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <random>




Camera2D OpenWindow(int gridW, int gridH, int tileSize){

    float mapW = gridW * tileSize;
    float mapH = gridH * tileSize;
    

    
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
    textures.Load("PEASHOOTER", "resources/peashooter.png");
    textures.Load("ZOMBIE", "resources/zombie.png");
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
    float hp;
    


    public: 
    bool isDead = false;
    
    std::string typeID;
    Rectangle rec;
    Entity(int posX, int posY, std::string ID, Rectangle rectangle, float health) : x(posX), y(posY), typeID(ID), rec(rectangle), hp(health){}

    void DrawEntity(Texture2D texture ){
        Rectangle sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height};
        Vector2 origin = {0, 0};
        
        DrawTexturePro(texture, sourceRec, rec, origin, 0.0f, WHITE);
    }

    virtual void TakeDamage(float amount){
        hp -= amount;
        if(hp <= 0){
            isDead = true;
        }
    }

    virtual ~Entity() = default;
};

class Plant : public Entity{
    int sunCost;

    public: 
    Plant(int posX, int posY, std::string ID, int cost, Rectangle rec, float hp) : Entity(posX, posY, ID, rec, hp){
        sunCost = cost;
    }

};




class Card{
    protected:
    int width;
    int height;
    Texture2D texture;
    Rectangle rec;
    std::string typeID;
    public:
    std::string getTypeID() const {return typeID;};
    int getWidth() const {return width;}
    int getHeight() const {return height;}
    Rectangle getRec() const {return rec;}

    Card(int Width, int Height, Texture2D Texture, std::string ID){
        width = Width;
        height = Height;
        texture = Texture;
        typeID = ID;
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
    Rectangle rec;
    public:
    Rectangle getRec() const {return rec;}
    std::vector<Card> getCards() const {return cards;}
    Inventory(int posX, int posY){
        x = posX;
        y = posY;
        
    }

    void AddCardToInventory(Card card){
        cards.push_back(card);
        width = card.getWidth()*cards.size() + (spacing*(cards.size()-1)) + 20;
        height = card.getHeight() + 20;
        rec = {(float)x, (float)y, (float)width, (float)height};
    }

    void DrawInventory(){
        DrawRectangleRec(rec, WHITE);
        DrawRectangle(x, y, width, height, GREEN);
        for (int i = 0; i < cards.size(); i++){
            cards[i].DrawCard(this->x + 10 + (i*(cards[i].getWidth() + spacing)), this->y+10);
        }
    }
    
};

class Zombie{
    int x;
    int y;
    int width;
    int height;
    Texture2D texture;
    std::string typeID;
    Rectangle rec;
    Rectangle collisionRec;
    float movementSpeed;
    float hp;

    int moveIDX = 0;
    bool isBlocked = false;
    float attackDamage;
    float attackTimer = 0.0f;
    float attackInterval = 1.0f;
    

    public:
    Zombie(int posX, int posY, int Width, int Height, Texture2D Texture, std::string type){
        x = posX;
        y = posY;
        width = Width;
        height = Height;
        texture = Texture;
        typeID = type;
        SetTypeVariables();
    }
    
    void SetTypeVariables(){
        if(typeID == "Normal"){
            movementSpeed = 1;
            hp = 10.0f;
            attackDamage = 20.0f;
        }
    }

    void DrawZombie(){
        Vector2 pos = {(float)x, (float)y};
        Rectangle source = {0.0f, 0.0f, (float)texture.width, (float)texture.height};
        rec = {(float)x, (float)y, (float)width, (float)height};
        Vector2 origin {0,0};
        DrawTexturePro(texture, source, rec, origin, 0.0f, WHITE);
    }

    

    void Move(std::vector<std::unique_ptr<Entity>>& plants){
        collisionRec = {(float)x - 5, (float)y, (float)width/2, (float)height};
        isBlocked = false;
        for (int i = 0; i < plants.size(); i++){
            if (CheckCollisionRecs(collisionRec, plants[i]->rec)){
                isBlocked = true;
            
                attackTimer += GetFrameTime();
                if(attackTimer >= 1.0f){
                    plants[i]->TakeDamage(attackDamage);
                    attackTimer = 0.0f;
                }
            }
        }
        if(!isBlocked){
            moveIDX += 1;
                if (moveIDX >= 10){
                    x -= movementSpeed*1;
                    moveIDX = 0;
                }
        }
    }
};

class Sunflower : public Plant{

    public:
    Sunflower(int posX, int posY, int cost, Rectangle rec) : Plant(posX, posY, "SUNFLOWER", cost, rec, 100.0f){
        
    }
};

class Peashooter : public Plant{

    public:
    Peashooter(int posX, int posY, int cost, Rectangle rec) : Plant(posX, posY, "PEASHOOTER", cost, rec, 100.0f){
        
    }

    void CheckForZombie(std::vector<Zombie> zombieList){

    }

    
};


class Player{

    std::string selectedType;
    TextureLoader& textures;
    std::vector<std::unique_ptr<Entity>>& entityVector;
    Camera2D camera;
    public:
    Player(std::string Type, TextureLoader& textureloader, std::vector<std::unique_ptr<Entity>>& entityList, Camera2D playerCam)
        : selectedType(Type), textures(textureloader), entityVector(entityList), camera(playerCam){}
    
    void Interaction(std::vector<Tile>& tileVector, Inventory& inventory){
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
            if(CheckCollisionPointRec(mousePos, inventory.getRec())){
                for (int i = 0; i < inventory.getCards().size(); i++){
                    if (CheckCollisionPointRec(mousePos, inventory.getCards()[i].getRec())){
                        selectedType = inventory.getCards()[i].getTypeID();
                    }
                }
            }else{
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
    }

    void PlantPlant(int x, int y, Rectangle rec){
        if(textures.textures.find(selectedType) != textures.textures.end()){
            if(selectedType == "SUNFLOWER"){
                entityVector.push_back(std::make_unique<Sunflower>(x, y, 100, rec));
            }
            if(selectedType == "PEASHOOTER"){
                entityVector.push_back(std::make_unique<Peashooter>(x, y, 100, rec));
            }
            else{

            }
        }
    }
};




class Level{
    
    std::vector<Card> levelCards;
    std::vector<Zombie> activeZombies;
    int waves = 5;
    float spawnTimer = 0.0f;
    float spawnInterval = 5.0f;
    Texture2D zombieTex;
    
    public:
    Level(std::vector<std::string> CardIDList, TextureLoader& textures){
        for (auto item : CardIDList){
            Card card(100, 100, textures.Get(item), item);
            levelCards.push_back(card);
        }
        zombieTex = textures.Get("ZOMBIE");
    }

    void Update(float deltaTime, int gridW, int gridH, int tileSize, std::vector<std::unique_ptr<Entity>>& plants){
        spawnTimer += deltaTime;

        if(spawnTimer >= spawnInterval){
            int randomRow = GetRandomValue(0, gridH-1);

            int spawnX = gridW * tileSize;
            int spawnY = randomRow * tileSize;


            activeZombies.push_back(Zombie(spawnX, spawnY, tileSize, tileSize, zombieTex, "Normal"));

            spawnTimer = 0.0f;
        }

        for (int i = 0; i < activeZombies.size(); i++){
            activeZombies[i].Move(plants);
        }
    }

    void DrawZombie(){
        
        for (auto& z : activeZombies){
            z.DrawZombie();
            
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
    InitWindow(0, 0, "Plants VS Zombies");
    int FPS = 60;
    SetTargetFPS(FPS);
    int gridW = 9;
    int gridH = 5;
    float targetW = 2620.0f;
    float scale = (float)GetScreenWidth() / targetW;
    int tileSize = 100* scale;
    Camera2D camera = OpenWindow(gridW, gridH, tileSize);

    TextureLoader textures;
    LoadTextures(textures);

    Inventory inventory(camera.target.x- camera.offset.x + 10, camera.target.y - camera.offset.y + 10);


    Zombie zombie();

    // Level Intitializer
    
    Level* level1 = new Level({"SUNFLOWER", "PEASHOOTER"}, textures);
    std::vector<Zombie> level1Zombies = {Zombie(tileSize*gridW, tileSize*gridH, tileSize, tileSize, textures.Get("ZOMBIE"), "Normal")};
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


        float dt = GetFrameTime();

        level1->Update(dt, gridW, gridH, tileSize, entityVector);
        player.Interaction(tileVector, inventory);
        
        BeginDrawing();

        ClearBackground(WHITE);
        
        BeginMode2D(camera);

        for (int i = 0; i < tileVector.size(); i++){
            tileVector[i].DrawTile();
        }
        for (int i = 0; i < entityVector.size(); i++){
            if(entityVector[i]->isDead){
                entityVector.erase(entityVector.begin() + i);
                i--;
            }else{
                std::string ID = entityVector[i]->typeID;
                Texture2D tex = textures.Get(ID);
                entityVector[i]->DrawEntity(tex);
            }
            
        }
        
        
        level1->DrawZombie();
        inventory.DrawInventory();

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();



    return 0;
}