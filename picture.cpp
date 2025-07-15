#include <vector> 
#include <string>
#define STB_IMAGE_IMPLEMENTATION
# define CHANNELS 3
#include "stb_image.h"


class Picture{
    private:
        const int width_;
        const int height_;
        unsigned char* data_;
        
    public:
        Picture(const char* path, const int width, const int height) : width_(width), height_(height){
            int w(width), h(height), c(CHANNELS);
            data_ = stbi_load(path, &w, &h, &c, 0);
        }
        int const width(){
            return this->width_;
        }
        int height() const{
            return this->height_;
        }
        int r(int x, int y) const {
            return data_[(y*width_ + x) * CHANNELS];
        }
        int g(int x, int y) const {
            return data_[(y*width_ + x) * CHANNELS + 1];
        }
        int b(int x, int y) const {
            return data_[(y*width_ + x) * CHANNELS + 2];
        }
        int a(int x, int y ) const{
            if(CHANNELS == 4){
                return data_[(y*width_ + x) * CHANNELS + 3];
            }else{
                return -1;
            }
        }

};

class SeamCarver{
    public:
        const Picture picture;
        SeamCarver(const Picture picture) : picture(picture) {
        }
        double energy(int x, int y){
            if(x < picture.height())

            return 0;
        }
};

int main(){

    return 0;
}