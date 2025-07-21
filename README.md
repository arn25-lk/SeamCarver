# Seam Carving Image Resizer

This project implements **content-aware image resizing** using the **seam carving algorithm** in C++. Seam carving reduces image dimensions by removing vertical or horizontal seams (paths of least visual importance), preserving important visual features better than standard resizing methods.

The energy of each pixel is calculated using the **dual-gradient energy function**, and seams are computed via dynamic programming. You can also highlight seams for visualization before removing them.

```bash
g++ -std=c++20 main.cpp picture.cpp -o seam_carver
```

### Usage

```bash
./seam_carver [PATH_NAME] -s=[WIDTH]x[HEIGHT] -t=[TARGET_WIDTH]x[TARGET_HEIGHT] [-h]
```
The resized imaged will be called "e-[PATH_NAME]". The highlighted seams before removal at each iteration will be visualized in the "h-[PATH_NAME]" file. 
