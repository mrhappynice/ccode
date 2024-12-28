#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Terminal dimensions
#define WIDTH 80
#define HEIGHT 24
#define BASE_ITER 50
#define MAX_GRADIENT_SIZE 64

// Different character sets for random selection
const char* PATTERN_SETS[] = {
    " .-:=+*#%@",                                         // Classic simple
    ".,-~:;=!*#$@",                                       // Detailed dots
    " ░▒▓█",                                             // Block elements
    "· ○ ◎ ● ◐ ◑ ◒ ◓ ◔ ◕ ◖ ◗ ◌",                        // Circles
    " ⡀ ⡄ ⡆ ⡇ ⣇ ⣧ ⣷ ⣿",                                // Braille patterns
    " ╱╲╳━│┃┆┇┈┉┊┋╌╍╎╏┄┅┆┇",                           // Lines
    ".,┌┐└┘├┤┬┴┼━┃",                                    // Boxes
    " ⠂⠅⠇⠏⠋⠉⠙⠹⠽⠿",                                    // More braille
};
#define NUM_PATTERN_SETS (sizeof(PATTERN_SETS) / sizeof(char*))

typedef struct {
    char gradient[MAX_GRADIENT_SIZE];
    int size;
    int color_mode;  // 0: no color, 1: random colors
} VisualPattern;

// Structure to hold view boundaries
typedef struct {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
    double zoom_level;
} ViewPort;

// Function to generate a random visual pattern
VisualPattern generate_random_pattern() {
    VisualPattern pattern;
    
    // Select a random pattern set
    int set_index = rand() % NUM_PATTERN_SETS;
    const char* chosen_set = PATTERN_SETS[set_index];
    
    // Copy and potentially reverse or shuffle the pattern
    pattern.size = strlen(chosen_set);
    if (pattern.size > MAX_GRADIENT_SIZE) pattern.size = MAX_GRADIENT_SIZE;
    
    memcpy(pattern.gradient, chosen_set, pattern.size);
    
    // Randomly reverse the pattern
    if (rand() % 2) {
        for (int i = 0; i < pattern.size / 2; i++) {
            char temp = pattern.gradient[i];
            pattern.gradient[i] = pattern.gradient[pattern.size - 1 - i];
            pattern.gradient[pattern.size - 1 - i] = temp;
        }
    }
    
    // Randomly decide to use colors
    pattern.color_mode = rand() % 2;
    
    return pattern;
}

// Function to calculate if a point is in the Mandelbrot set
int mandelbrot(double complex c, int max_iter) {
    double complex z = 0;
    int iter;
    
    for (iter = 0; iter < max_iter; iter++) {
        z = z * z + c;
        if (cabs(z) > 2.0)
            break;
    }
    
    return iter;
}

// Function to clear the screen
void clear_screen(void) {
    printf("\033[2J\033[H");
}

// Function to set terminal color
void set_color(int iter, int max_iter) {
    int color = 31 + (iter % 7);  // Random between 31-37 (ANSI colors)
    printf("\033[%dm", color);
}

// Function to reset terminal color
void reset_color(void) {
    printf("\033[0m");
}

// Function to draw the Mandelbrot set
void draw_mandelbrot(ViewPort view, VisualPattern pattern) {
    double dx = (view.x_max - view.x_min) / WIDTH;
    double dy = (view.y_max - view.y_min) / HEIGHT;
    
    int current_max_iter = BASE_ITER + (int)(view.zoom_level * 2);
    
    for (int y = 0; y < HEIGHT; y++) {
        double im = view.y_max - y * dy;
        
        for (int x = 0; x < WIDTH; x++) {
            double re = view.x_min + x * dx;
            double complex c = re + im * I;
            
            int iter = mandelbrot(c, current_max_iter);
            int gradient_index = (iter == current_max_iter) ? 
                               0 : ((iter % pattern.size) * pattern.size / current_max_iter);
            
            if (pattern.color_mode) {
                set_color(iter, current_max_iter);
            }
            
            putchar(pattern.gradient[gradient_index]);
            
            if (pattern.color_mode) {
                reset_color();
            }
        }
        putchar('\n');
    }
}

// Function to zoom into a specific point
void zoom_to_point(ViewPort* view, double target_x, double target_y) {
    double zoom_factor = 1.05;
    
    double width = (view->x_max - view->x_min) / zoom_factor;
    double height = (view->y_max - view->y_min) / zoom_factor;
    
    view->x_min = target_x - width / 2;
    view->x_max = target_x + width / 2;
    view->y_min = target_y - height / 2;
    view->y_max = target_y + height / 2;
    view->zoom_level += 0.1;
}

int main() {
    srand(time(NULL));
    
    // Generate random visual pattern for this run
    VisualPattern pattern = generate_random_pattern();
    
    // Initialize view at an interesting area
    ViewPort view = {
        .x_min = -2.0,
        .x_max = 1.0,
        .y_min = -1.5,
        .y_max = 1.5,
        .zoom_level = 0
    };
    
    // Target point to zoom towards
    double target_x = -0.743643887037158;
    double target_y = 0.131825904205311;
    
    printf("Mandelbrot Visualization with Random Patterns\n");
    printf("Press Ctrl+C to exit\n");
    printf("Starting zoom sequence in 3 seconds...\n");
    sleep(3);
    
    while (1) {
        clear_screen();
        draw_mandelbrot(view, pattern);
        
        zoom_to_point(&view, target_x, target_y);
        
        // Occasionally generate new random pattern
        if (rand() % 100 == 0) {
            pattern = generate_random_pattern();
        }
        
        usleep(150000);
    }
    
    return 0;
}
