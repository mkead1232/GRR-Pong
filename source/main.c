#include <grrlib.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <stdio.h>

#include "smiley_bmp.h"
#include "font_png.h"
#define GRRLIB_BLACK 0x000000FF
#define GRRLIB_WHITE 0xFFFFFFFF

void updatePaddles();
void updateBall();
void drawPaddles();
void drawBall();
void resetBall();
void PrintCenteredText(int y, const char *text, GRRLIB_texImg *font, u32 color, float scale);
int paddle1_y = 100;  
int paddle2_y = 100;  

#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 100

int ball_x = 240, ball_y = 160;  // Starting position of ball
int ball_dx = 4, ball_dy = 4;    // Ball speed

int player1_score = 0;
int player2_score = 0;

int main() {
    GRRLIB_Init();
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

    GRRLIB_texImg *smiley = GRRLIB_LoadTexture(smiley_bmp);
    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(font_png);
    GRRLIB_InitTileSet(tex_font, 16, 16, 32);

    if (!smiley) {
        printf("Error: Failed to load texture!\n");
        return -1;
    }

    while (1) {
        WPAD_ScanPads();
        const u32 wpaddown = WPAD_ButtonsDown(0);

        GRRLIB_FillScreen(GRRLIB_BLACK);  // Clear the screen
        updatePaddles();
        updateBall();

        // GRRLIB_DrawImg(100, 100, smiley, 0, 4, 4, GRRLIB_WHITE);

        drawPaddles();
        drawBall();
        
        if (wpaddown & WPAD_BUTTON_HOME) {
            break;
        }
        
        GRRLIB_2dMode();
        
        PrintCenteredText(20, "GRRLIB PONG BY ADEN KIRK", tex_font, GRRLIB_WHITE, 1);
        
        char score_text[20];
        sprintf(score_text, "%d - %d", player1_score, player2_score);
        PrintCenteredText(60, score_text, tex_font, GRRLIB_WHITE, 3);


        GRRLIB_Render();  // Render everything to the screen

    }

    GRRLIB_FreeTexture(smiley);  // Free the texture

    GRRLIB_Exit();  // Exit and clean up
    return 0;
}


void drawPaddles() {
    // Left paddle at X = 30
    GRRLIB_Rectangle(30, paddle1_y, PADDLE_WIDTH, PADDLE_HEIGHT, GRRLIB_WHITE, true);
    
    // Right paddle at the correct position
    GRRLIB_Rectangle(640 - 30 - PADDLE_WIDTH, paddle2_y, PADDLE_WIDTH, PADDLE_HEIGHT, GRRLIB_WHITE, true);
}

void updatePaddles() {
    if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) {
        paddle1_y -= 5;  // Move paddle 1 up
    }
    if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) {
        paddle1_y += 5;  // Move paddle 1 down
    }

    if (WPAD_ButtonsHeld(1) & WPAD_BUTTON_UP) {
        paddle2_y -= 5;  // Move paddle 2 up
    }
    if (WPAD_ButtonsHeld(1) & WPAD_BUTTON_DOWN) {
        paddle2_y += 5;  // Move paddle 2 down
    }

    // Constrain paddles within screen bounds
    if (paddle1_y < 0) paddle1_y = 0;
    if (paddle1_y > 480 - PADDLE_HEIGHT) paddle1_y = 480 - PADDLE_HEIGHT;
    if (paddle2_y < 0) paddle2_y = 0;
    if (paddle2_y > 480 - PADDLE_HEIGHT) paddle2_y = 480 - PADDLE_HEIGHT;
}

void drawBall() {
    GRRLIB_Circle(ball_x, ball_y, 10, 0xFFFFFFFF, true);  // Draw the ball
}

void updateBall() {
    ball_x += ball_dx;
    ball_y += ball_dy;

    // Bounce off top and bottom
    if (ball_y <= 0 || ball_y >= 480) {
        ball_dy = -ball_dy;  // Reverse ball direction
    }

    // Bounce off paddles
    if (ball_x <= 50 && ball_y >= paddle1_y && ball_y <= paddle1_y + PADDLE_HEIGHT) {
        ball_dx = -ball_dx;  // Reverse ball direction
    }
    if (ball_x >= (640 - 30 - PADDLE_WIDTH) && ball_y >= paddle2_y && ball_y <= paddle2_y + PADDLE_HEIGHT) {
        ball_dx = -ball_dx;  // Reverse ball direction
    }

    // Scoring system
    if (ball_x <= (0 + 30)) {  // Ball passed the left side → Player 2 scores
        player2_score++;
        resetBall();
    }
    if (ball_x >= (640 - 30)) {  // Ball passed the right side → Player 1 scores
        player1_score++;
        resetBall();
    }
}

void resetBall() {
    ball_x = 320;  // Center of screen
    ball_y = 240;
    ball_dx = (ball_dx > 0) ? -4 : 4;  // Reverse direction to serve to the other player
}

void PrintCenteredText(int y, const char *text, GRRLIB_texImg *font, u32 color, float scale) {
    int char_width = 16 * scale;  // Assuming each character is 16px wide at scale 1
    int text_width = char_width * strlen(text);
    int x = (640 - text_width) / 2;  // Center based on screen width (640px)
    
    GRRLIB_Printf(x, y, font, color, scale, text);
}