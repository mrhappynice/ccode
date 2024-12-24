#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 80
#define HEIGHT 20
#define PI 3.14159265
#define DELAY 50000 // microseconds between frames

// Function to clear the screen - works on both Unix and Windows
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Function to get a character based on position in the wave
char getWaveChar(double value) {
    // Array of characters to create density effect
    const char chars[] = " .,-~:;=!*#$@";
    int index = (int)((value + 1) * (strlen(chars) - 1) / 2);
    return chars[index];
}

int main() {
    double time = 0.0;
    double screen[HEIGHT][WIDTH];
    
    // Animation loop
    while(1) {
        // Calculate wave pattern
        for(int y = 0; y < HEIGHT; y++) {
            for(int x = 0; x < WIDTH; x++) {
                // Create multiple overlapping waves
                double wave1 = sin(x * 0.1 + time);
                double wave2 = cos(x * 0.05 + time * 0.5);
                double wave3 = sin(x * 0.02 + time * 0.7);
                
                // Combine waves and adjust for vertical position
                screen[y][x] = sin(
                    (y - HEIGHT/2) * 0.2 +
                    (wave1 + wave2 + wave3) * 0.3
                );
            }
        }
        
        // Clear screen before drawing new frame
        clearScreen();
        
        // Draw the frame
        for(int y = 0; y < HEIGHT; y++) {
            for(int x = 0; x < WIDTH; x++) {
                putchar(getWaveChar(screen[y][x]));
            }
            putchar('\n');
        }
        
        // Update time and add small delay
        time += 0.2;
        usleep(DELAY);
    }
    
    return 0;
}
