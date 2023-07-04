#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "lin_al.h"
#include "lin_vec.h"
using namespace LIN;
#define UNHEX(type, c) (type)((uint8_t)((c) >> 8 * 3) & (uint8_t)0xff)\
                      ,(type)((uint8_t)((c) >> 8 * 2) & (uint8_t)0xff)\
                      ,(type)((uint8_t)((c) >> 8)     & (uint8_t)0xff)\
                      ,(type)((uint8_t)((c))          & (uint8_t)0xff)

const size_t MAX_GENERATION = 50;
int main(int argc, char** argv) {
    srand(time(0));
    if (argc >= 4 || argc <= 1) {
        fprintf(stderr, "ERROR: Invalid usage\n");
        printf("Usage: k-means {filename} {k}\n");
        return 1;
    }
    char* end = argv[2] + strlen(argv[2]);
    size_t k = strtoul(argv[2], &end, 10);
    printf("Image path: %s\n", argv[1]);
    printf("K: %zu\n", k);

    int x, y, depth;
    unsigned int* temp = (unsigned int*)stbi_load(argv[1], &x, &y, &depth, 4);
    if (temp == NULL) {
        printf("ERROR: Can't load image %s\n", argv[1]);
        return 1;
    }
    printf("Width: %d\nHeight: %d\nDepth: %d\n", x, y, depth);
    MatrixXX<unsigned char> data(x * y, 4, [&](size_t i, size_t j) -> size_t {
    unsigned char temp_arr[] = {UNHEX(unsigned char, temp[i])};
    return temp_arr[3 - j]; // RGBA because little endian so it becomes ABGR -_- maybe...
    });
    stbi_image_free(temp);
    MatrixXX<unsigned char> means(k, 4, [&]() -> size_t {
        return data[rand()*1.0f/RAND_MAX*x][rand()*1.0f/RAND_MAX*4];
    });
    MatrixXX<unsigned char> classifies(x * y, 4);
    MatrixXX<float> new_means(k, 4);
    char output[128] {};

    for (size_t i = 0; i < MAX_GENERATION; i++) {
        size_t* count = new size_t[k] {};
        bool change = false;
        sprintf(output, "output/image-out-%02zu.png", 0ul);
        for (size_t i = 0; i < size_t(x*y); i++) {
            float min = 9999999;
            unsigned char min_idx = 0;
            for (size_t j = 0; j < k; j++) {
                const VectorX<unsigned char> color_diff{ data[i] - means[j] };
                float dis = color_diff.dot(color_diff);
                if (dis < min) {
                    min = dis;
                    min_idx = j;
                }
            }
            count[i]++;
            new_means[min_idx] = new_means[min_idx] + data[i];
            classifies[i] = means[min_idx];
        }
        for (size_t i = 0; i < k; i++) {
            new_means[i] = new_means[i] / k;
            for (size_t j = 0; j < 4; j++) {
                if ((unsigned char)new_means[i][j] != means[i][j]) change = true;
                means[i][j] = (unsigned char)new_means[i][j];
            }
        }
        stbi_write_png(output, x, y, 4, classifies.raw(), x * sizeof(unsigned int));

        delete[] count;
        if (!change) break;
    }
    return 0;
}
