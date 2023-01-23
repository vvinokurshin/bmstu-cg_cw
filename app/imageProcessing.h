#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <iostream>

#include <QImage>

class Image {
 private:
    typedef struct {
        unsigned int bfType;
        unsigned long bfSize;
        unsigned int bfReserved1;
        unsigned int bfReserved2;
        unsigned long bfOffBits;
    } BITMAPFILEHEADER;

    typedef struct {
        unsigned int biSize;
        size_t biWidth;
        size_t biHeight;
        unsigned short biPlanes;
        unsigned short biBitCount;
        unsigned int biCompression;
        unsigned int biSizeImage;
        int biXPelsPerMeter;
        int biYPelsPerMeter;
        unsigned int biClrUsed;
        unsigned int biClrImportant;
    } BITMAPINFOHEADER;

    typedef struct {
        double rgbBlue;
        double rgbGreen;
        double rgbRed;
        double rgbReserved;
    } RGBQUAD;

    typedef struct {
        double value;
        RGBQUAD rgb;
    } PIX;

 public:
    Image() : pixels(0) {}
    Image(const Image &other);
    ~Image();

    Image operator=(const Image &other);

    bool load(std::string &filename);
    bool save(std::string &filename);
    QImage DataToQImage();

    void negativeFilter();
    void grayFilter();
    void blurFilter();
    void noiseFilter();

    void changeBrightness(const int &value);
    void changeContrast(const int &value);

    void rotate_counterclockwise();
    void rotate_clockwise();

    bool isNone();

 private:
    BITMAPFILEHEADER header;
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD **pixels;

    unsigned short read_u16(FILE *fp);
    unsigned int read_u32(FILE *fp);
    int read_s32(FILE *fp);

    void write_u16(unsigned short input, FILE *fp);
    void write_u32(unsigned int input, FILE *fp);
    void write_s32(int input, FILE *fp);

    double **generate_coeff(int radius, double sigma);
    double gaussianModel(double x, double y, double sigma);
    double overFlow(double midpix);
    double maskMatrix(double A[7][7], double **B);

    void free_pixels();
};

#endif // IMAGEPROCESSING_H
