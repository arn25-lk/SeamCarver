#include "picture.hpp"
#include <vector>
int main(int argc, char *argv[]){
    if(argc < 4 || argc > 5){
        return -1;
    }
    int w,h;
    int to_width, to_height;
    char path[1024]; 
    bool hilight_first = false;
    std::sscanf(argv[1], "%s", path);
    std::sscanf(argv[2], "-s=%dx%d", &w, &h);
    std::sscanf(argv[3], "-t=%dx%d", &to_width, &to_height);
    if(argc == 5){
        hilight_first = true;
    }

    util::Picture p(path, w, h);
    SeamCarver sc(p);
    
    for(int i = 0; i < w-to_width; i++){
        SeamCarver sc(p);  
        auto x = sc.findVerticalSeam();
        if(hilight_first){ p.highlight_vseam(x); p.write_himg();}
        p.remove_vseam(p.width()-1, h, x);
    }
    for(int i = 0; i < h-to_height; i++){
        SeamCarver sc(p);  
        auto x = sc.findHorizontalSeam();
        if(hilight_first){ p.highlight_hseam(x); p.write_himg();}
        p.remove_hseam(p.width(), p.height()-1, x);
    }
    p.write_img();
    return 0;
}