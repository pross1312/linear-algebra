#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "lin_al.h"
#include "lin_vec.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
using namespace LIN;
#define UNHEX(type, c) (type)((uint8_t)((c) >> 8 * 3) & (uint8_t)0xff)\
                      ,(type)((uint8_t)((c) >> 8 * 2) & (uint8_t)0xff)\
                      ,(type)((uint8_t)((c) >> 8)     & (uint8_t)0xff)\
                      ,(type)((uint8_t)((c))          & (uint8_t)0xff)

int main(int argc, char** argv) {
    srand(time(0));
    if (argc > 4 || argc < 2) {
        fprintf(stderr, "ERROR: Invalid usage\n");
        printf("Usage: k-means {filename} {fileout = IMAGE_OUTPUT.png} {k = 3} {MAX_ITER = 10}\n");
        return 1;
    }
    const char* file_in   = argv[1];
    const char* file_out  = argc >= 3 ? argv[1] : "IMAGE_OUTPUT.png";
    size_t k_clusters     = argc >= 4 ? (size_t)std::stoi(argv[2]) : 3;
    const size_t MAX_ITER = argc >= 5 ? (size_t)std::stoi(argv[3]) : 10;

    int width, height, depth;
    unsigned int* temp = (unsigned int*)stbi_load(file_in, &width, &height, &depth, 4);
    if (temp == NULL) {
        printf("ERROR: Can't load image %s\n", file_in);
        return 1;
    }
    MatrixXX<unsigned char> data(width * height, 4, [&](size_t i, size_t j) -> size_t {
        unsigned char temp_arr[] = {UNHEX(unsigned char, temp[i])};
        return temp_arr[3 - j]; // RGBA because little endian so it becomes ABGR -_- maybe...
    });
    stbi_image_free(temp);

    MatrixXX<unsigned char> centroids(k_clusters, 4, [&](size_t _) {
        return data[rand() * 1.f / RAND_MAX * (width - 1)];
    });

    MatrixXX<unsigned char> classifies(width * height, 4);
    MatrixXX<float>         sum_data(k_clusters, 4);

    for (size_t img = 0; img < MAX_ITER; img++) {
        size_t *count = new size_t[k_clusters] {};
        bool change   = false;

        for (size_t px = 0; px < (size_t)width * (size_t)height; px++) {
            float min_diff = 1e10;
            size_t min_idx = 0;
            for (size_t mean_idx = 0; mean_idx < k_clusters; mean_idx++) {
                 float temp_min = .0f;
                 for (size_t comp = 0; comp < 4; comp++) {
                     temp_min += (data[px][comp] - centroids[mean_idx][comp]) * (data[px][comp] - centroids[mean_idx][comp]);
                 }
                 if (temp_min < min_diff) {
                     min_diff = temp_min;
                     min_idx = mean_idx;
                 }
            }
            count[min_idx]++;
            sum_data[min_idx].binary_op<unsigned char>(data[px], [](auto x, auto y) {
                return x + (float)y;
            });
            classifies[px] = centroids[min_idx];
        }
        for (size_t mean_idx = 0; mean_idx < k_clusters; mean_idx++) {
            for (size_t comp = 0; comp < 4; comp++) {
                unsigned char temp = (unsigned char)(sum_data[mean_idx][comp] / count[mean_idx]);
                if (temp != centroids[mean_idx][comp]) change = true;
                centroids[mean_idx][comp] = temp;
                sum_data[mean_idx][comp] = 0.f;
            }
        }
        delete[] count;
        if (!change) break;
    }
    stbi_write_png(file_out, width, height, 4, classifies.raw(), width * sizeof(unsigned int));
    return 0;
}
