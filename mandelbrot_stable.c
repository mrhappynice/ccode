#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

// Terminal dimensions
#define WIDTH 80
#define HEIGHT 24

// Base iteration count that will increase with zoom
#define BASE_ITER 50

// Characters for different iteration counts, creating a gradient
const char GRADIENT[] = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
#define GRADIENT_SIZE (sizeof(GRADIENT) - 1)

// Structure to hold view boundaries
typedef struct {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
    double zoom_level;
} ViewPort;

// Function to generate a random double between min and max
double random_double(double min, double max) {
    double scale = rand() / (double) RAND_MAX;
    return min + scale * (max - min);
}

// Function to check if a point is interesting (has enough iteration variance)
int is_interesting_point(double x, double y) {
    double complex c = x + y * I;
    int base_iter = mandelbrot(c, BASE_ITER);
    
    // Check surrounding points for variance
    double delta = 0.001;
    int surrounding_iters = 0;
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            if(i == 0 && j == 0) continue;
            complex double test_point = (x + i*delta) + (y + j*delta) * I;
            surrounding_iters += mandelbrot(test_point, BASE_ITER);
        }
    }
    
    // Point is interesting if there's significant variance
    return abs(base_iter * 8 - surrounding_iters) > BASE_ITER * 2;
}

// Function to find a random interesting point
void find_random_interesting_point(double *x, double *y) {
    do {
        *x = random_double(-2.0, 0.5);
        *y = random_double(-1.2, 1.2);
    } while (!is_interesting_point(*x, *y));
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

// Function to draw the Mandelbrot set with increasing complexity
void draw_mandelbrot(ViewPort view) {
    double dx = (view.x_max - view.x_min) / WIDTH;
    double dy = (view.y_max - view.y_min) / HEIGHT;
    
    // Calculate max iterations based on zoom level, but with a slower increase
    int current_max_iter = BASE_ITER + (int)(view.zoom_level * 2);
    
    for (int y = 0; y < HEIGHT; y++) {
        double im = view.y_max - y * dy;
        
        for (int x = 0; x < WIDTH; x++) {
            double re = view.x_min + x * dx;
            double complex c = re + im * I;
            
            int iter = mandelbrot(c, current_max_iter);
            // Use a more dynamic mapping for the gradient
            int gradient_index = (iter == current_max_iter) ? 
                               0 : ((iter % GRADIENT_SIZE) * GRADIENT_SIZE / current_max_iter);
            
            putchar(GRADIENT[gradient_index]);
        }
        putchar('\n');
    }
}

// Function to zoom into a specific point
void zoom_to_point(ViewPort* view, double target_x, double target_y) {
    double zoom_factor = 1.05; // Slower zoom for better visualization
    
    double width = (view->x_max - view->x_min) / zoom_factor;
    double height = (view->y_max - view->y_min) / zoom_factor;
    
    // Calculate new boundaries while maintaining aspect ratio
    view->x_min = target_x - width / 2;
    view->x_max = target_x + width / 2;
    view->y_min = target_y - height / 2;
    view->y_max = target_y + height / 2;
    view->zoom_level += 0.1; // Slower zoom level increase
}

int main() {
    // Initialize random seed
    srand(time(NULL));
    
    // Find a random interesting starting point and target
    double target_x, target_y;
    find_random_interesting_point(&target_x, &target_y);
    
    // Start with a view centered on the random point
    ViewPort view = {
        target_x - 1.5,  // Start slightly zoomed out from target
        target_x + 1.5,
        target_y - 1.5,
        target_y + 1.5,
        0
    };
    
    printf("Random Mandelbrot Zoom Visualization\n");
    printf("Starting coordinates: x=%.6f, y=%.6f\n", target_x, target_y);
    printf("Press Ctrl+C to exit\n");
    printf("Starting zoom sequence in 3 seconds...\n");
    sleep(3);
    
    while (1) {
        clear_screen();
        draw_mandelbrot(view);
        
        zoom_to_point(&view, target_x, target_y);
        
        // Longer delay for better visualization
        usleep(150000);  // 150ms delay
        
        // Occasionally shift the target slightly for more varied patterns
        if (rand() % 100 == 0) {
            target_x += random_double(-0.0001, 0.0001);
            target_y += random_double(-0.0001, 0.0001);
        }
    }
    
    return 0;
}
