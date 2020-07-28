#include "mbed.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#include <string>
#include <ctime>
#include <vector>

#define MAXX 220
#define MAXY 230
#define ballRadius 3

TS_StateTypeDef TS_State = { 0 }; 

AnalogIn potentiometer(p15);

bool beginGame;
bool firstPage;
bool authorsPage;
bool scorePage;
bool pausePage;

int paddleWidth = 36;
int paddleHeight = 10;
double paddleX = 110;
int paddleY = 225;

int ballOldX;
int ballOldY;
int ballX = 127;
int ballY = 220;

int dir = 1; // 1 - TopRight, 2 - TopLeft, 3 - BottomLeft, 4 - BottomRight
int bricksLeft = 42;
int win = 0;
int lose = 0;
int score = 0;
int lives = 3;
int level;

std::vector<int> scores;

void startMenu();

int visibleBricks[42] = {1,1,1,1,1,1,
                         1,1,1,1,1,1,
                         1,1,1,1,1,1,
                         1,1,1,1,1,1,
                         1,1,1,1,1,1,
                         1,1,1,1,1,1,
                         1,1,1,1,1,1};
                         
int bricks[42][2] = {
    {6, 20},{6, 40},{6, 60},{6, 80},{6, 100},{6, 120},{6, 140},             
    {46, 20},{46, 40},{46, 60},{46, 80},{46, 100},{46, 120},{46, 140},      
    {86, 20},{86, 40},{86, 60},{86, 80},{86, 100},{86, 120},{86, 140},      
    {126, 20},{126, 40},{126, 60},{126, 80},{126, 100},{126, 120},{126, 140},                                                                       
    {166, 20},{166, 40},{166, 60},{166, 80},{166, 100},{166, 120},{166, 140},                                                                       
    {206, 20},{206, 40},{206, 60},{206, 80},{206, 100},{206, 120},{206, 140}
};    

void printScore(){ 
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font12);
    std::string s1 = "Score: ";
    s1 += std::to_string(int(score));
    char c1[s1.size() + 1];
    strcpy(c1, s1.c_str());
    BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)c1, RIGHT_MODE);
}

void initializeBricks(){
   srand (time(NULL));
   BSP_LCD_Clear(LCD_COLOR_WHITE);
   int boja = 0;
   
   for (int i = 0; i < 42; i++){
       
           boja = rand() % 8;
           
           switch(boja){
               case 0: BSP_LCD_SetTextColor(LCD_COLOR_RED); break;
               case 1: BSP_LCD_SetTextColor(LCD_COLOR_GREEN); break;
               case 2: BSP_LCD_SetTextColor(LCD_COLOR_BLUE); break;
               case 3: BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE); break;
               case 4: BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN); break;
               case 5: BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA); break;
               case 6: BSP_LCD_SetTextColor(LCD_COLOR_YELLOW); break;
               case 7: BSP_LCD_SetTextColor(LCD_COLOR_ORANGE); break;
           }
           
          if (visibleBricks[i]) BSP_LCD_FillRect (bricks[i][0], bricks[i][1], 30, 10);  //x pos, y pos, sirina, visina
          
       }
    
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font12);
    std::string s1 = "Lives: ";
    s1 += std::to_string(int(lives));
    char c1[s1.size() + 1];
    strcpy(c1, s1.c_str());
    BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)c1, LEFT_MODE);
    
    std::string s2 = "Level: ";
    s2 += std::to_string(int(level));
    char c2[s2.size() + 1];
    strcpy(c2, s2.c_str());
    BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)c2, CENTER_MODE);
    
    printScore();
    
}

void paddle() {
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillRect(paddleX-2, paddleY, paddleWidth+5, paddleHeight+5);
    
    if (beginGame == true) {
        if (potentiometer.read() != 0) beginGame = false;
        paddleX = 110;
    }
    else paddleX = potentiometer.read()*200;
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_FillRect(paddleX, paddleY, paddleWidth, paddleHeight);   
}

void moveBall() {
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillCircle(ballOldX, ballOldY, ballRadius);
       
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_FillCircle(ballX, ballY, ballRadius);
}

void ballHitSlider(){
    if (ballX >= paddleX && ballX <= paddleX + paddleWidth-2) {
        if (ballY == paddleY - paddleHeight) {
            if (dir == 3) dir = 2;
            else if (dir == 4) dir = 1;
        }
     }
}

void removeBrick(int i){
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillRect (bricks[i][0], bricks[i][1], 30, 10);
}

void ballHitBrick(){
     for (int i = 0; i < 42; i++){
          if (visibleBricks[i] == 1) {
              if (ballY >= bricks[i][1] && ballY <= bricks[i][1] + 10) {
                  if (ballX >= bricks[i][0] && ballX <= bricks[i][0] + 30) {
                      visibleBricks[i] = 0;
                      removeBrick(i);
                      bricksLeft--;
                      score += 10;
                      printScore();
                      break;
                  }
              }
          }
     }
}

bool isBrickHit() {
    for (int i = 0; i < 42; i++){
        if (visibleBricks[i] == 1) {
            if (ballY >= bricks[i][1] && ballY <= bricks[i][1] + 10) {
                  if (ballX >= bricks[i][0] && ballX <= bricks[i][0] + 30) {
                      return true;
                  }
              }
        }
    }
    return false;
}

void viewScore() {
    bool printed = true;    
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(0, 15, (uint8_t *)"Scores:", CENTER_MODE);
    
    BSP_LCD_DrawHLine(10,230,210);
    BSP_LCD_DrawVLine(10,20,210);
    int x = 30, y = 230, previous = 0;
    
    BSP_LCD_SetBackColor(LCD_COLOR_RED);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(0, 210, (uint8_t *)"Go back", RIGHT_MODE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    
    while(1) {
        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected) {
          uint16_t y = TS_State.touchY[0];
          if (y >= 190) {
            scorePage = false;
            firstPage = true;
            startMenu();
            break;
          }
        }
        if (printed == true){
          for (int i = 0; i < scores.size(); i++){
          float score = scores[i];
          BSP_LCD_SetTextColor(LCD_COLOR_RED);
          BSP_LCD_FillCircle(x, y - score / 10, 2);
          BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
          BSP_LCD_DrawLine(x-20, 230 - previous / 10, x, y - score / 10);
        
          std::string s1;
          s1 += std::to_string(int(scores[i]));
          char c1[s1.size() + 1];
          strcpy(c1, s1.c_str());
          BSP_LCD_SetFont(&Font12);
          BSP_LCD_DisplayStringAt(x, y - score / 10 + 5, (uint8_t *)c1, LEFT_MODE);
     
          x += 20;
          previous = score;
          }
        printed = false;
       }
       wait_ms(100);
    }
}

void authors() {
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    
    BSP_LCD_SetFont(&Font16);
    
    BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)"Esma Karahodza and", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 70, (uint8_t *)"Dzenana Huseinspahic", CENTER_MODE);
    
    BSP_LCD_SetFont(&Font12);
    
    BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Embedded systems, ETF", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 110, (uint8_t *)"june 2020.", CENTER_MODE);
    
    BSP_LCD_SetBackColor(LCD_COLOR_RED);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(0, 210, (uint8_t *)"Go back", RIGHT_MODE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_TS_GetState(&TS_State);
    if(TS_State.touchDetected) {
        uint16_t y = TS_State.touchY[0];
        if (y >= 190) {
            authorsPage = false;
            firstPage = true;
            startMenu();
        }
    }
}

void startMenu() {
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    BSP_LCD_FillRect(0, 40, BSP_LCD_GetXSize(), 33);
    BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)"Start", CENTER_MODE);
    
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_FillRect(0, 90, BSP_LCD_GetXSize(), 0);
    BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Score", CENTER_MODE);
    BSP_LCD_FillRect(0, 123, BSP_LCD_GetXSize(), 0);
    
    BSP_LCD_FillRect(0, 140, BSP_LCD_GetXSize(), 0);
    BSP_LCD_DisplayStringAt(0, 150, (uint8_t *)"Authors", CENTER_MODE);
    BSP_LCD_FillRect(0, 173, BSP_LCD_GetXSize(), 0);
    
    BSP_TS_GetState(&TS_State);
    if(TS_State.touchDetected) {
        uint16_t y = TS_State.touchY[0];
        if (y >= 40 && y <= 73) {
            beginGame = true;
            level = 1;
            score = 0;
            firstPage = false;
        }
        else if (y >= 90 && y <= 123) {
            scorePage = true;
            firstPage = false;
        }
        else if (y >= 140 && y <= 173) {
            firstPage = false;
            authorsPage = true;
        }
    }
}

void play() {
    initializeBricks();
    if (level == 1) {
        BSP_LCD_SetFont(&Font24);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Level 1", CENTER_MODE);
        wait_ms(2000);
        initializeBricks();
    }
    
    while(1) {
        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected) {
            pausePage = !pausePage;
            if (pausePage == false) initializeBricks();
        }
        if (pausePage == true) {
            BSP_LCD_SetFont(&Font24);
            BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
            BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Pause", CENTER_MODE);
            wait_ms(200);
            continue;
        }
        
        ballOldX = ballX;
        ballOldY = ballY;
        if(dir == 1) { //top right
            ballY -= 10;
            ballX += 20;
            if (ballX > MAXX) dir = 2;
            else if (ballY <= 15 || isBrickHit()) 
             dir = 4;
        }
        else if (dir == 2) { //top left
           ballY -= 10;
           ballX -= 20;
           if (ballY <= 15 || isBrickHit()) 
           dir = 3;
           else if (ballX < 5) dir = 1;
        }
        else if (dir == 3) { //bottom left
           ballY += 10;
           ballX -= 20;
           if (ballY > MAXY) {
               lose = 1;
               break;
           }
           else if ((ballY > paddleY && paddleX < ballX 
                    && paddleX+36 > ballX) || isBrickHit()) 
                    dir = 2;
           else if (ballX < 5) dir = 4;
        }
        else if (dir == 4) { //bottom right
           ballY += 10;
           ballX += 20;
           if (ballX > MAXX) dir = 3;
           else if (ballY > MAXY) {
            lose = 1;
            break;
        }
        else if ((ballY > paddleY && paddleX < ballX 
               && paddleX+36 > ballX) || isBrickHit()) 
               dir = 1;
        }
        
        ballHitSlider();
        ballHitBrick();
        moveBall();
        paddle();
        if (bricksLeft == 0) {
            win = 1;
            break;
        }
        
        if (level == 1) wait_ms(350);
        else if (level == 2) wait_ms(250);
        else if (level == 3) wait_ms(200);
        else if (level == 4) wait_ms(150);
        else if (level == 5) wait_ms(100);
    }
        
    
    if (lose == 1 || win == 1) {
        BSP_LCD_SetFont(&Font24);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        ballX = 127;
        ballY = 220;
        beginGame = true;
        dir = 1;
        
    }
    
    if (lose == 1) {
        lose = 0;
        if (lives == 0) {
            BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Game over", CENTER_MODE);
            scores.push_back(score);
            firstPage = true;
            lives = 4;
            level = 1;
            for (int i = 0; i < 42; i++)  visibleBricks[i] = 1;
            bricksLeft = 42;
            
        }
        else BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"You lose", CENTER_MODE);
        lives--;
        wait_ms(1000);
    }
    
    if (win == 1) {
        win = 0;
        level++;
        bricksLeft = 42;
        for (int i = 0; i < 42; i++) visibleBricks[i] = 1;
        if (level == 6) {
            firstPage = true;
            BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Winner!!!", CENTER_MODE);
            scores.push_back(score);
            level = 1;
        } else {
            std::string s1 = "Level ";
            s1 += std::to_string(level);
            char c1[s1.size() + 1];
            strcpy(c1, s1.c_str());
            BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)c1, CENTER_MODE);
        }
        wait_ms(1000);
    }


        
        if (level == 2) {
            for (int i = 0; i < 42; i++){
                if (i % 2 == 0) {
                    removeBrick(i);
                    visibleBricks[i] = 0;
                }
            }
            bricksLeft = 21;
        }
        else if (level == 3) {
            for (int i = 0; i < 42; i++){
                if (i % 2 == 1) {
                    removeBrick(i);
                    visibleBricks[i] = 0;
                }
            }
            bricksLeft = 21;
        } else if (level == 4) {
            for (int i = 0; i < 42; i++){
                if (i % 3 == 0) {
                    removeBrick(i);
                    visibleBricks[i] = 0;
                }
            }
            bricksLeft = 28;
        }
        else if (level == 5) {
            for (int i = 0; i < 42; i++){
                if (i % 3 != 0) {
                    removeBrick(i);
                    visibleBricks[i] = 0;
                }
            }
            bricksLeft = 14;
        }
        
        wait_ms(1000);
    }


int main() {
    BSP_LCD_Init();
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    
    beginGame = true;     
    firstPage = true;
    scorePage = false;
    authorsPage = false;
    pausePage = false;
    
    initializeBricks();

    
    while (1) {
         if (firstPage == true) startMenu();
         else if (scorePage == true) viewScore();
         else if (authorsPage == true) authors();
         else play();
        wait_ms(10);
   }
    
}
