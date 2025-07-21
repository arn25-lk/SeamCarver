#ifndef PICTURE_H
#define PICTURE_H

#include <vector>
#include <string>

#define CHANNELS 3

namespace util
{
    class Picture
    {
    private:
        int prev_w;
        int prev_h;
        std::vector<unsigned char> prev_data_;
        int width_;
        int height_;
        std::vector<unsigned char> data_;
        std::string path_;

    public:
        Picture(const char *path, const int width, const int height);
        int width() const;
        int height() const;
        unsigned char r(int x, int y) const;
        unsigned char g(int x, int y) const;

        unsigned char b(int x, int y) const;
        int a(int x, int y) const;
        void set_wx(int x, int y);
        int remove_vseam(int w, int h, std::vector<int> v_seam);
        int remove_hseam(int w, int h, std::vector<int> h_seam);
        int highlight_vseam(std::vector<int> seam);
        int highlight_hseam(std::vector<int> seam);

        int write_img();
        int write_himg();
    };
}
class SeamCarver
{
private:
    util::Picture picture;

public:
    SeamCarver(util::Picture picture);
    double energy(int x, int y, bool debug = false);
    std::vector<int> findVerticalSeam(bool debug = false);
    std::vector<int> findHorizontalSeam(bool debug = false);
};

#endif