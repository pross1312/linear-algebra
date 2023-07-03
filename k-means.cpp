#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "lin_al.h"

int main() {
    srand(time(0));
    int x, y, depth;
    const char* img_path = "image.png";
    unsigned char *temp = (unsigned char*)stbi_load(img_path, &x, &y, &depth, 1);
    if (temp == NULL) {
        printf("ERROR: Can't load image %s\n", img_path);
        return 1;
    }
    printf("Width: %d\nHeight: %d\nDepth: %d\n", x, y, depth);
    LIN::VectorX<unsigned char> data(x * y, temp);
    stbi_image_free(temp);
    stbi_write_png("image-data.png", x, y, 1, data.raw(), x * sizeof(data[0]));

    const size_t k = 30;
    const size_t max_generation = 50;
    LIN::VectorX<unsigned char> means(k, [&]() { return data[rand()*1.0f/RAND_MAX * x * y]; });
    for (size_t i = 0; i < max_generation; i++) {
        char output[128] {};
        size_t* count = new size_t[k] {};
        float* new_means = new float[k] {};
        bool change = false;
        sprintf(output, "output/image-out-%02zu.png", i);
        LIN::VectorX<unsigned char> classifies(x * y, [&](size_t index) -> unsigned char {
            float min = 9999999;
            unsigned char min_idx = 0;
            for (size_t j = 0; j < k; j++) {
                float dis = (data[index] - means[j]) * (data[index] - means[j]);
                if (dis < min) {
                    min = dis;
                    min_idx = j;
                }
            }
            new_means[min_idx] += data[index];
            count[min_idx]++;
            return means[min_idx];
        });
        stbi_write_png(output, x, y, 1, classifies.raw(), x * sizeof(classifies[0]));
        for (size_t i = 0; i < k; i++) {
            new_means[i] /= count[i];
            unsigned char new_k = (unsigned char)(std::round(new_means[i]));
            if (new_k != means[i]) change = true;
            means[i] = new_k;
        }
        delete[] new_means;
        delete[] count;
        if (!change) break;
    }
    return 0;
}
