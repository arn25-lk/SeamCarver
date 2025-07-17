#include <vector> 
#include <string>
#include <iostream>
#include <array>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define CHANNELS 3
#include "stb_image.h"
#include "stb_image_write.h"


class Picture{
    private:
        const int width_;
        const int height_;
        unsigned char* data_;
        std::string path_; 
        
        
    public:
        Picture(const char* path, const int width, const int height) : width_(width), height_(height){
            int w(width), h(height), c(CHANNELS);
            data_ = stbi_load(path, &w, &h, &c, 0);
            std::string p(path);
            path_ = p; 
         
        }
        int width() const{
            return this->width_;
        }
        int height() const{
            return this->height_;
        }
        int r(int x, int y) const {
            return data_[(x*width_ + y) * CHANNELS];
        }
        int g(int x, int y) const {
            return data_[(x*width_ + y) * CHANNELS + 1];
        }
        int b(int x, int y) const {
            return data_[(x*width_ + y) * CHANNELS + 2];
        }
        int a(int x, int y ) const{
            if(CHANNELS == 4){
                return data_[(x*width_ + y) * CHANNELS + 3];
            }else{
                return -1;
            }
        }
        void set_wx(int x, int y){
            char c = static_cast<char>(255);
            char z = static_cast<char>(0);
            data_[(x*width_ + y) * CHANNELS] = c;
            data_[(x*width_ + y) * CHANNELS + 1] = c;
            data_[(x*width_ + y) * CHANNELS + 2] = c;
        } // Once we set this, this seam will no longer be considered.


        int highlight_vseam(std::vector< int> seam){
            for(int i=0; i < height_; i++){
                set_wx(i, seam[i]);
            }
            return 0;
        }
        int highlight_hseam(std::vector< int> seam){
            for(int i=0; i < width_; i++){
                set_wx(seam[i], i);
            }
            return 0;
        }
        int write_himg(){
            std::string p = "h-" + path_;
            return stbi_write_png(p.c_str(), width_, height_, 3, data_, width_ * 3);
        }
        

};

class SeamCarver{
    public:
        const Picture picture;
        SeamCarver(const Picture picture) : picture(picture) {
        }
        double energy(int x, int y, bool debug = false){
            if(x >= picture.height() || x  < 0) return -1; 
            if(y >= picture.width() || y < 0) return -1; 
            int x1 = (x + 1) % picture.height();
            int x2 = (x - 1 + picture.height()) % picture.height();
            int y1 = (y + 1) % picture.width();        
            int y2 = (y - 1 + picture.width()) % picture.width();   
        
            int rx = picture.r(x, y1) - picture.r(x, y2); 
            int gx = picture.g(x, y1) - picture.g(x, y2);
            int bx = picture.b(x, y1) - picture.b(x, y2);
            double delta_h = rx * rx + gx * gx + bx * bx;

            int ry = picture.r(x1, y) - picture.r(x2, y); 
            int gy = picture.g(x1, y) - picture.g(x2, y);
            int by = picture.b(x1, y) - picture.b(x2, y);
            double delta_v = ry * ry + gy * gy + by * by;

            double energy = delta_h + delta_v;
            if(debug){ 
                std::cout <<  energy << " " << picture.r(x,y) << "," << picture.g(x,y) << "," << picture.b(x,y) << "    ";
            }
            return energy;
        }
        std::vector<int> findVerticalSeam( bool debug = false){
            int w{picture.width()}, h{picture.height()};
            std::vector< std::vector<int>> all_seams;
            std::vector< std::vector<int>> back;
            all_seams.resize(h);
            back.resize(h);
            for(int i = 0; i < h;i++){
                all_seams[i].resize(w);
                back[i].resize(w);
            }
            for(int j = 0; j < w;j++){
                all_seams[0][j] = this->energy(0,j);
                back[0][j] = j;
            }
            for(int i = 1; i < h;i++){
                for(int j = 0; j < w; j++){
                    double p_x = all_seams[i-1][j];
                    double p_xl = j == 0 ? INT_MAX : all_seams[i-1][j-1];
                    double p_xr = j == w-1 ? INT_MAX : all_seams[i-1][j+1];
                    double min_val = p_xl;
                    int index = j - 1;
                    if (p_x < min_val) { min_val = p_x; index = j; }
                    if (p_xr < min_val) { min_val = p_xr; index = j + 1; }
                    all_seams[i][j] = this->energy(i,j) + min_val;
                    back[i][j] = index;
                }
            }
            if(debug){
                for(auto row : all_seams){
                    for(auto e : row){
                        std::cout << e << " "; 
                    }
                    std::cout << std::endl;
                }
            }
            int min = INT_MAX;
            int idx = 0;
            for(int i = 0; i < w;i++){
                if(all_seams[h-1][i] < min){
                    min = all_seams[h-1][i];
                    idx = i; 
                }
            }
            std::vector<int> seam;
            seam.resize(h);
            for(int i = h-1; i >= 0; i--){
                seam[i] = idx;
                idx = back[i][idx];
            }
            return seam;
        }
        std::vector<int> findHorizontalSeam( bool debug = false){
            int w{picture.width()}, h{picture.height()};
            std::vector< std::vector<int>> all_seams;
            std::vector< std::vector<int>> back;
            all_seams.resize(h);
            back.resize(h);
            for(int i = 0; i < h;i++){
                all_seams[i].resize(w);
                back[i].resize(w);
            }
            for(int i = 0; i < h;i++){
                all_seams[i][0] = this->energy(i,0);
                back[i][0] = i;
            }
            for(int j = 1; j < w; j++){
                for(int i = 0; i < h; i++){
                    double p_x = all_seams[i][j-1];
                    double p_xd = i == h-1 ? INT_MAX : all_seams[i+1][j-1];
                    double p_xu = i == 0 ? INT_MAX : all_seams[i-1][j-1];
                    double min_val = p_xd;
                    int index = i + 1;
                    if (p_x < min_val) { min_val = p_x; index = i; }
                    if (p_xu < min_val) { min_val = p_xu; index = i - 1; }
                    all_seams[i][j] = this->energy(i,j) + min_val;
                    back[i][j] = index;
                }
            }
            if(debug){
                for(auto row : all_seams){
                    for(auto e : row){
                        std::cout << e << " "; 
                    }
                    std::cout << std::endl;
                }
            }
            int min = INT_MAX;
            int idx = 0;
            for(int i = 0; i < h;i++){
                if(all_seams[i][w-1] < min){
                    min = all_seams[i][w-1];
                    idx = i; 
                }
            }
            std::vector<int> seam;
            seam.resize(w);
            for(int i = w-1; i >= 0; i--){
                seam[i] = idx;
                idx = back[idx][i];
            }
            return seam;
        }
};


int main(){
    int w{1024}, h{1024};
    Picture p("arnav.png", w, h);
    int to_width{996}, to_height{996}; 
    SeamCarver sc(p);
    // for(int i = 0;  i < h; i++){
    //     for(int j = 0; j < w; j++){
    //         sc.energy(i,j, true);
    //     }
    //     std::cout << "\n"; 
    // }
    for(int i = to_width; i < w; i++){
        auto x  = sc.findVerticalSeam();
        p.highlight_vseam(x);
    }
    for(int i = to_height; i < h; i++){
        auto x  = sc.findHorizontalSeam();
        p.highlight_hseam(x);
    }
    return p.write_himg();

}