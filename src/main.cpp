#include <iostream>
#include <raylib.h>
#include <vector>




class Tile{
    int x;
    int y;
    int rectSizeLength;
    Rectangle rec;
    Color color = RED;
    bool occupied = false;
    
    public:
    Tile(int posX, int posY, int rectLength) {
        x = posX;
        y = posY;
        rectSizeLength = rectLength; 
        rec = {(float)x*100, (float)y*100, 100, 100};
        
    }

    void DrawTile(){ 
        DrawRectangleRounded(rec, 0.3f, 3.0f, color);
        if(occupied){
            color = GREEN;
            
        }
        else{
            color = RED;
        }
       
    }

    void Interaction(){
        Vector2 mousePos = GetMousePosition();
        if(CheckCollisionPointRec(mousePos, rec)){
            
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                occupied = true;
            }
            
        }
        
        
    }
    

};




int main(){
    int winW = 1080;
    int winH = 720;
    int rectSideLength = 100;
    int gridW = 9;
    int gridH = 5;
    int targetFPS = 60;
    SetTargetFPS(targetFPS);
    InitWindow(winW, winH, "Plants VS Zombies");

    

    std::vector<Tile> tileVector = {};
    for (int y = 0; y <= gridH; y++){
        for (int x = 0; x <= gridW; x++){
            Tile tile = Tile(x, y, rectSideLength);
            tileVector.push_back(tile);
        }
            
    }





    


    while(!WindowShouldClose()){
        BeginDrawing();
        
        ClearBackground(WHITE);
        
        
        for (size_t i = 0; i < tileVector.size(); i++){
            tileVector[i].DrawTile();
            tileVector[i].Interaction();
        }
        
        DrawFPS(10, 10);
        
        

        
        

        EndDrawing();
    }
    CloseWindow();


    return 0;
}