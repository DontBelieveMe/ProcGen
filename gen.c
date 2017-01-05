#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define SHOW_IMAGE

#define width 128
#define height 128

typedef unsigned char byte;

void write_texture(struct pixel *pixels, const char *path, int w, int h);
void generate_noise();
double smooth_noise(double x, double y);
double turbulence(double x, double y, double size);
inline byte *to_bytearray(struct pixel *pixels);

struct pixel {
    byte r;
    byte g;
    byte b;
    byte a;
};

double noise[width * height];

void write_texture(struct pixel *pixels, const char *path, int w, int h) {
    stbi_write_png(path, w, h, 4, to_bytearray(pixels), w * 4);
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
    
    while (size >= 1) {
        value += smooth_noise(x / size, y / size) * size;
        size = size / 2.0;
    }

    double result = 128 * value / inital_size;
    return result;
}

inline byte *to_bytearray(struct pixel *pixels) {
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
    double turbulence_size = 32;

    int x, y;
    for (x = 0; x < width; x++) {
        for (y = 0; y < height; y++) {
            struct pixel *pix = &(pixels[x + y * width]);
            double xy_value = x * x_period / width + y * y_period / height + turbulence_power * turbulence(x, y, turbulence_size) / 256.0;
            double sin_value = 256 * fabs(sin(xy_value * M_PI));
            pix->r = pix->g = pix->b = (uint8_t)(sin_value);
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