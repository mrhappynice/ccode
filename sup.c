#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 60
#define HEIGHT 20
#define NUM_STARS 40

// Structure to represent a star
typedef struct {
    int x;
    int y;
    char symbol;
    int speed;
} Star;

// Function to clear the screen (works on Unix-like systems)
void clearScreen() {
    printf("\033[H\033[J");
}

// Function to initialize a star with random properties
void initStar(Star *star) {
    star->x = rand() % WIDTH;
    star->y = rand() % HEIGHT;
    star->speed = (rand() % 2) + 1;
    
    // Randomly choose star symbol
    char symbols[] = {'.', '*', '+', 'o'};
    star->symbol = symbols[rand() % 4];
}

// Function to update star position
void updateStar(Star *star) {
    star->x -= star->speed;
    
    // If star goes off screen, reset it
    if (star->x < 0) {
        star->x = WIDTH - 1;
        star->y = rand() % HEIGHT;
    }
}

// Function to draw the frame
void drawFrame(Star stars[], int numStars) {
    // Create empty display buffer
    char display[HEIGHT][WIDTH];
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            display[i][j] = ' ';
        }
    }
    
    // Place stars in buffer
    for (int i = 0; i < numStars; i++) {
        if (stars[i].x >= 0 && stars[i].x < WIDTH && 
            stars[i].y >= 0 && stars[i].y < HEIGHT) {
            display[stars[i].y][stars[i].x] = stars[i].symbol;
        }
    }
    
    // Draw top border
    printf("+");
    for (int i = 0; i < WIDTH; i++) printf("-");
    printf("+\n");
    
    // Draw frame content
    for (int i = 0; i < HEIGHT; i++) {
        printf("|");
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", display[i][j]);
        }
        printf("|\n");
    }
    
    // Draw bottom border
    printf("+");
    for (int i = 0; i < WIDTH; i++) printf("-");
    printf("+\n");
}

int main() {
    // Initialize random seed
    srand(time(NULL));
    
    // Create and initialize stars
    Star stars[NUM_STARS];
    for (int i = 0; i < NUM_STARS; i++) {
        initStar(&stars[i]);
    }
    
    // Animation loop
    printf("Press Ctrl+C to exit\n");
    while (1) {
        clearScreen();
        
        // Update and draw frame
        drawFrame(stars, NUM_STARS);
        
        // Update star positions
        for (int i = 0; i < NUM_STARS; i++) {
            updateStar(&stars[i]);
        }
        
        // Control animation speed
        usleep(100000);  // 100ms delay
    }
    
    return 0;
}
