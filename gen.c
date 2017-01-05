#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#ifdef _WIN32
    #define _USE_MATH_DEFINES
#endif
#include <math.h>

// #define SHOW_IMAGE

#define PI            (3.14159265359)
#define width         (500)
#define height        (500)
#define COMP          (4)

typedef unsigned char byte;
struct                pixel;

void                  write_texture(struct pixel *pixels, const char *path, int w, int h);
void                  generate_noise();
double                smooth_noise(double x, double y);
double                turbulence(double x, double y, double size);
byte                  *to_bytearray(struct pixel *pixels);

struct pixel {
    byte r;
    byte g;
    byte b;
    byte a;
};

double noise[width * height];

void write_texture(struct pixel *pixels, const char *path, int w, int h) {
    stbi_write_png(path, w, h, COMP, to_bytearray(pixels), w * COMP);
}

// Uses bilinear-interpolation
double smooth_noise(double x, double y) {
    double f_x = x - (int)x;
    double f_y = y - (int)y;

    int x1 = (int)((int)x + width) % width;
    int y1 = (int)((int)y + height) % height;

    int x2 = (int)(x1 + width - 1) % width;
    int y2 = (int)(y1 + height - 1) % height;
    
    double value = 0.0;
    
    value += f_x * f_y * noise[y1 + x1 * width];
    value += (1 - f_x) * f_y * noise[y1 + x2 * width];
    value += f_x * (1 - f_y) * noise[y2 + x1 * width];
    value += (1 - f_x) * (1 - f_y) * noise[y2 + x2 * width];

    return value;
}

double turbulence(double x, double y, double size) {
    double value = 0.0, inital_size = size;
    double result;
    
    while (size >= 1) {
        value += smooth_noise(x / size, y / size) * size;
        size = size / 2.0;
    }

    result = 128 * value / inital_size;
    return result;
}

byte *to_bytearray(struct pixel *pixels) {
    return (byte*)(pixels);
}

void generate_noise() {
    srand((unsigned int) time(0));
    int x, y;
    for (x = 0; x < width; x++) {
        for (y = 0; y < height; y++) {
            noise[x + y * width] = (rand() % 32768) / 32768.0;
        }
    }
}

int main() {
    generate_noise();
    struct pixel *pixels = malloc(width * height * sizeof(struct pixel));

    double x_period = 0;
    double y_period = 1;
    double turbulence_power = 2.5;
    double turbulence_size = 50;

    int x, y;
    for (x = 0; x < width; x++) {
        for (y = 0; y < height; y++) {
            struct pixel *pix = &(pixels[x + y * width]);
            
            double x_value = x * x_period / width;
            double y_value = y * y_period / height;
            double turb_value = turbulence_power * turbulence(x, y, turbulence_size) / 256.0;

            double xy_value = x_value + y_value + turb_value;
            uint8_t sin_value =(uint8_t)(256 * fabs(sin(xy_value * PI)) * 1.5);
            
            if(sin_value < (255/4)) {
                pix->r = sin_value * 2;
            } else {
                pix->r = sin_value / 8;
            }
            
            pix->g = (double) sin_value / 1.5;
            if(sin_value > 255 - (255/6)) {
                pix->b = sin_value * 2;
            } else {
                pix->b = sin_value / 4;
            }
            pix->a = 255;
        }
    }

    write_texture(pixels, "output.png", width, height);

#ifdef SHOW_IMAGE
    system("output.png");
#endif

    free(pixels);
    return 0;
}
