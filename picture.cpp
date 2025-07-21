#include <vector> 
#include <string>
#include <iostream>
#include <array>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "picture.hpp"

using namespace util;


Picture::Picture(const char* path, const int width, const int height) : width_(width), height_(height){
    int w(width), h(height), c(CHANNELS);
    unsigned char* data = stbi_load(path, &w, &h, &c, 0);
    for(int i = 0; i < w*h*c; i++){
        data_.push_back(data[i]);
        prev_data_.push_back(data[i]); 
    }
    std::string p(path);
    path_ = p; 
}
int Picture::width() const{
    return width_;
}
int Picture::height() const{
    return height_;
}
unsigned char Picture::r(int x, int y) const {
    return data_[(x*width_ + y) * CHANNELS];
}
unsigned char Picture::g(int x, int y) const {
    return data_[(x*width_ + y) * CHANNELS + 1];
}
unsigned char Picture::b(int x, int y) const {
    return data_[(x*width_ + y) * CHANNELS + 2];
}
int Picture::a(int x, int y ) const{
    if(CHANNELS == 4){
        return data_[(x*width_ + y) * CHANNELS + 3];
    }else{
        return -1;
    }
}
void Picture::set_wx(int x, int y){
    char c = static_cast<char>(255);
    prev_data_[(x*width_ + y) * CHANNELS] = c;
    prev_data_[(x*width_ + y) * CHANNELS + 1] = c;
    prev_data_[(x*width_ + y) * CHANNELS + 2] = c;
} // Once we set this, this seam will no longer be considered.
int Picture::remove_vseam(int w, int h, std::vector< int > v_seam){
    std::vector<unsigned char> new_data;
    new_data.resize(w*h*CHANNELS);
    for(int i = 0; i < height_; i++){ 
        int x = 0; 
        int seam_j = v_seam[i];
        for(int j = 0; j < width_; j++){
            if(j != seam_j){
                new_data[(i*w + x) * CHANNELS] = r(i,j);
                new_data[(i*w + x) * CHANNELS + 1] = g(i,j);
                new_data[(i*w + x) * CHANNELS + 2] = b(i,j);
                x++;
            }
        }
    }
    data_ = new_data;
    prev_data_ = data_;
    width_ = w;
    height_ = h; 
    return 0;
}
int Picture::remove_hseam(int w, int h, std::vector< int > h_seam){
    std::vector<unsigned char> new_data;
    new_data.resize(w*h*CHANNELS);
    for(int j = 0; j < width_; j++){ 
        int y = 0; 
        int seam_j = h_seam[j];
        for(int i = 0; i < height_; i++){
            if(i != seam_j){
                new_data[(y*w + j) * CHANNELS] = r(i,j);
                new_data[(y*w + j) * CHANNELS + 1] = g(i,j);
                new_data[(y*w + j) * CHANNELS + 2] = b(i,j);
                y++;
            }
        }
    }
    
    data_ = new_data;
    prev_data_ = new_data;
    width_ = w;
    height_ = h; 
    return 0;
}
int Picture::highlight_vseam(std::vector< int> seam){
    for(int i=0; i < height_; i++){
        set_wx(i, seam[i]);
    }
    return 0;
}
int Picture::highlight_hseam(std::vector<int> seam){
    for(int i=0; i < width_; i++){
        set_wx(seam[i], i);
    }
    return 0;
}
int Picture::write_img(){
    std::string p = "e-" + path_;
    unsigned char data [width_*height_*CHANNELS];
    for(int i = 0; i < width_*height_*CHANNELS; i++){
        data[i] = data_[i]; 
    }
    return stbi_write_png(p.c_str(), width_, height_, 3, data, width_ * 3);
}
int Picture::write_himg(){
    std::string p = "h-" + path_;
    unsigned char data [width_*height_*CHANNELS];
    for(int i = 0; i < width_*height_*CHANNELS; i++){
        data[i] = prev_data_[i]; 
    }
    return stbi_write_png(p.c_str(), width_, height_, 3, data, width_ * 3);
}





SeamCarver::SeamCarver(util::Picture picture) : picture(picture) {
}
double SeamCarver::energy(int x, int y, bool debug){
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
std::vector<int> SeamCarver::findVerticalSeam( bool debug){
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
        all_seams[0][j] = energy(0,j);
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
            all_seams[i][j] = energy(i,j) + min_val;
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
std::vector<int> SeamCarver::findHorizontalSeam( bool debug){
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
        all_seams[i][0] = energy(i,0);
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
            all_seams[i][j] = energy(i,j) + min_val;
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


