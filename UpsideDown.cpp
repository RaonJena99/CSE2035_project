#include <stdio.h>
#include <stdlib.h>

#include <png.h>

#define WIDTH 320
#define HEIGHT 320
#define COLOR_DEPTH 8

struct Pixel {
    png_byte r, g, b, a;
};

int main(int argc, char *argv[]) {
    // Open the existing PNG file for reading
    FILE *f = fopen("Example.png", "rb");
    if (!f) {
        fprintf(stderr, "could not open Example.png for reading\n");
        return 1;
    }

    // Initialize png data structures for reading
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "could not initialize png read struct\n");
        fclose(f);
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(f);
        return 1;
    }

    // Set up error handling for reading
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png read\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(f);
        return 1;
    }

    // Initialize png IO for reading
    png_init_io(png_ptr, f);
    png_read_info(png_ptr, info_ptr);

    // Check the dimensions of the image
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    
    if (width != WIDTH || height != HEIGHT) {
        fprintf(stderr, "Input image dimensions do not match expected dimensions\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(f);
        return 1;
    }

    // Read the image data directly into a 1D array
    struct Pixel *image_data = (struct Pixel *)malloc(WIDTH * HEIGHT * sizeof(struct Pixel));
    for (int row = 0; row < HEIGHT; row++) {
        png_read_row(png_ptr, (png_bytep)(image_data + row * WIDTH), NULL);
    }

    // Close the input file
    fclose(f);
    f = NULL;

    // Invert the image data in-place
    for (int row = 0; row < HEIGHT / 2; row++) {
        for (int col = 0; col < WIDTH; col++) {
            struct Pixel temp = image_data[row * WIDTH + col];
            image_data[row * WIDTH + col] = image_data[(HEIGHT - 1 - row) * WIDTH + col];
            image_data[(HEIGHT - 1 - row) * WIDTH + col] = temp;
        }
    }

    // Write the inverted image data to a new PNG file
    FILE *output_file = fopen("Example.png", "wb");
    if (!output_file) {
        fprintf(stderr, "could not open Example.png for writing\n");
        free(image_data);
        return 1;
    }

    // Initialize png data structures for writing
    png_structp png_write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_write_ptr = png_create_info_struct(png_write_ptr);

    png_init_io(png_write_ptr, output_file);
    png_set_IHDR(png_write_ptr, info_write_ptr, WIDTH, HEIGHT, COLOR_DEPTH,
                 PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_write_ptr, info_write_ptr);

    // Write the inverted image data to disk
    for (int row = 0; row < HEIGHT; row++) {
        png_write_row(png_write_ptr, (png_bytep)(image_data + row * WIDTH));
    }
    png_write_end(png_write_ptr, NULL);

    // Clean up memory and close the output file
    png_destroy_write_struct(&png_write_ptr, &info_write_ptr);
    fclose(output_file);
    free(image_data);

    return 0;
}