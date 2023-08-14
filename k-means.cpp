#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
// #include "lin_al.h"
// #include "lin_vec.h"
#include "lin_al.h"
#include <iostream>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#define UNHEX(type, c) (type)((uint8_t)((c) >> 8 * 3) & (uint8_t)0xff)\
                      ,(type)((uint8_t)((c) >> 8 * 2) & (uint8_t)0xff)\
                      ,(type)((uint8_t)((c) >> 8)     & (uint8_t)0xff)\
                      ,(type)((uint8_t)((c))          & (uint8_t)0xff)

int main(int argc, char** argv) {
    srand(time(0));
    if (argc > 5 || argc < 3) {
        fprintf(stderr, "ERROR: Invalid usage\n");
        printf("Usage: k-means {filename} {fileout} {k = 3} {MAX_ITER = 10}\n");
        return 1;
    }
    const char* file_in   = argv[1];
    const char* file_out  = argv[2];
    size_t k_clusters     = argc >= 4 ? (size_t)std::stoi(argv[3]) : 3;
    const size_t MAX_ITER = argc >= 5 ? (size_t)std::stoi(argv[4]) : 10;

    int width, height, depth;
    unsigned int* temp = (unsigned int*)stbi_load(file_in, &width, &height, &depth, 4);
    if (temp == NULL) {
        printf("ERROR: Can't load image %s\n", file_in);
        return 1;
    }
    MatrixX<uint8_t> data(width * height, 4, [&temp](size_t i, size_t j) -> size_t {
        uint8_t temp_arr[] = {UNHEX(uint8_t, temp[i])};
        return temp_arr[3 - j]; // RGBA because little endian so it becomes ABGR -_- maybe...
    });
    stbi_image_free(temp);

    MatrixX<uint8_t> centroids(k_clusters, 4, [&data, width](size_t _) {
        return data[rand() * 1.f / RAND_MAX * (width - 1) + _*0/* to turn of warning */];
    });

    MatrixX<uint8_t> classifies(width * height, 4);
    MatrixX<float>         sum_data(k_clusters, 4);

    size_t img_idx = 0;
    for (img_idx = 0; img_idx < MAX_ITER; img_idx++) {
        size_t *count = new size_t[k_clusters] {};
        bool change   = false;

        for (size_t px = 0; px < data.nRows; px++) {
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
            sum_data[min_idx] += data[px];
            classifies[px] = centroids[min_idx];
        }
        for (size_t mean_idx = 0; mean_idx < k_clusters; mean_idx++) {
            RowVectorX<float> new_centroid_float(sum_data[mean_idx] / count[mean_idx]);
            const RowVectorX<uint8_t> new_centroid(new_centroid_float.size(),
                    [&new_centroid_float](size_t i) -> uint8_t { return uint8_t(new_centroid_float[i]); });
            if (new_centroid != centroids[mean_idx]) change = true;
            centroids[mean_idx] = new_centroid;
            sum_data[mean_idx] = 0.f;
        }
        delete[] count;
        if (!change) break;
    }
    printf("Finish at %zu iterations\n", img_idx);
    printf("%s, width: %d, height: %d\n", file_out, width, height);
    stbi_write_png(file_out, width, height, 4, classifies.data, 0);
    return 0;
}
