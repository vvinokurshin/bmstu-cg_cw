#include <algorithm>
#include <cmath>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "imageProcessing.h"

Image::Image(const Image &other) : pixels(0) {
    header = other.header;
    bmiHeader = other.bmiHeader;

    pixels = new RGBQUAD *[bmiHeader.biHeight];

    for (int i = 0; i < bmiHeader.biHeight; ++i) {
        pixels[i] = new RGBQUAD[bmiHeader.biWidth];

        std::copy(other.pixels[i], other.pixels[i] + bmiHeader.biWidth,
                  pixels[i]);
    }
}

Image Image::operator=(const Image &other) {
    if (this != &other) {
        header = other.header;
        bmiHeader = other.bmiHeader;

        pixels = new RGBQUAD *[bmiHeader.biHeight];

        for (int i = 0; i < bmiHeader.biHeight; ++i) {
            pixels[i] = new RGBQUAD[bmiHeader.biWidth];

            std::copy(other.pixels[i], other.pixels[i] + bmiHeader.biWidth,
                      pixels[i]);
        }
    }

    return *this;
}

Image::~Image() { free_pixels(); }

void Image::free_pixels() {
    for (int i = 0; i < bmiHeader.biHeight; ++i) {
        delete[] pixels[i];
    }

    delete[] pixels;
}

bool Image::load(std::string &filename) {
    FILE *pFile = fopen(filename.c_str(), "rb");

    if (!pFile) {
        return false;
    }

    // считываем заголовок файла
    header.bfType = read_u16(pFile);
    header.bfSize = read_u32(pFile);
    header.bfReserved1 = read_u16(pFile);
    header.bfReserved2 = read_u16(pFile);
    header.bfOffBits = read_u32(pFile);

    // считываем заголовок изображения
    bmiHeader.biSize = read_u32(pFile);
    bmiHeader.biWidth = read_s32(pFile);
    bmiHeader.biHeight = read_s32(pFile);
    bmiHeader.biPlanes = read_u16(pFile);
    bmiHeader.biBitCount = read_u16(pFile);
    bmiHeader.biCompression = read_u32(pFile);
    bmiHeader.biSizeImage = read_u32(pFile);
    bmiHeader.biXPelsPerMeter = read_s32(pFile);
    bmiHeader.biYPelsPerMeter = read_s32(pFile);
    bmiHeader.biClrUsed = read_u32(pFile);
    bmiHeader.biClrImportant = read_u32(pFile);

    // инициализация массива пикселей
    pixels = new RGBQUAD *[bmiHeader.biHeight];

    // читаем байты цветов
    for (int i = 0; i < bmiHeader.biHeight; i++) {
        pixels[i] = new RGBQUAD[bmiHeader.biWidth];

        for (int j = 0; j < bmiHeader.biWidth; j++) {
            pixels[i][j].rgbBlue = getc(pFile);
            pixels[i][j].rgbGreen = getc(pFile);
            pixels[i][j].rgbRed = getc(pFile);
        }
    }

    fclose(pFile);

    return true;
}

bool Image::save(std::string &filename) {
    FILE *oFile = fopen(filename.c_str(), "wb");

    if (!oFile) {
        return false;
    }

    // записываем заголовок файла
    write_u16(header.bfType, oFile);
    write_u32(header.bfSize, oFile);
    write_u16(header.bfReserved1, oFile);
    write_u16(header.bfReserved2, oFile);
    write_u32(header.bfOffBits, oFile);

    // записываем заголовок изображения
    write_u32(bmiHeader.biSize, oFile);
    write_s32(bmiHeader.biWidth, oFile);
    write_s32(bmiHeader.biHeight, oFile);
    write_u16(bmiHeader.biPlanes, oFile);
    write_u16(bmiHeader.biBitCount, oFile);
    write_u32(bmiHeader.biCompression, oFile);
    write_u32(bmiHeader.biSizeImage, oFile);
    write_s32(bmiHeader.biXPelsPerMeter, oFile);
    write_s32(bmiHeader.biYPelsPerMeter, oFile);
    write_u32(bmiHeader.biClrUsed, oFile);
    write_u32(bmiHeader.biClrImportant, oFile);

    for (int i = 0; i < bmiHeader.biHeight; i++) {
        for (int j = 0; j < bmiHeader.biWidth; j++) {
            putc(pixels[i][j].rgbBlue, oFile);
            putc(pixels[i][j].rgbGreen, oFile);
            putc(pixels[i][j].rgbRed, oFile);
        }
    }

    fclose(oFile);
    return true;
}

QImage Image::DataToQImage() {
    QImage image(bmiHeader.biWidth, bmiHeader.biHeight, QImage::Format_RGB32);

    std::cout << bmiHeader.biHeight << " " << bmiHeader.biWidth << std::endl;
    std::cout << pixels[0][0].rgbBlue << std::endl;

    for (size_t i = 0; i < bmiHeader.biHeight; i++) {
        for (size_t j = 0; j < bmiHeader.biWidth; j++) {
            QRgb rgb = qRgb(pixels[i][j].rgbRed, pixels[i][j].rgbGreen,
                            pixels[i][j].rgbBlue);
            image.setPixel(j, bmiHeader.biHeight - i - 1, rgb);
        }
    }

    return image;
}

void Image::negativeFilter() {
    for (size_t i = 0; i < bmiHeader.biHeight; i++) {
        for (size_t j = 0; j < bmiHeader.biWidth; j++) {
            pixels[i][j].rgbBlue = 255 - pixels[i][j].rgbBlue;
            pixels[i][j].rgbGreen = 255 - pixels[i][j].rgbGreen;
            pixels[i][j].rgbRed = 255 - pixels[i][j].rgbRed;
        }
    }
}

void Image::grayFilter() {
    for (size_t i = 0; i < bmiHeader.biHeight; i++) {
        for (size_t j = 0; j < bmiHeader.biWidth; j++) {
            double s = (pixels[i][j].rgbBlue + pixels[i][j].rgbGreen +
                        pixels[i][j].rgbRed) /
                       3;

            pixels[i][j].rgbBlue = s;
            pixels[i][j].rgbGreen = s;
            pixels[i][j].rgbRed = s;
        }
    }
}

void Image::blurFilter() {
    double **coeffs = generate_coeff(7, 1.94);
    double tmp_matrix[7][7];

    for (size_t i = 3; i < bmiHeader.biHeight - 3; i++) {
        for (size_t j = 3; j < bmiHeader.biWidth - 3; j++) {
            for (size_t k = i - 3, i1 = 0; i1 < 7; k++, i1++)
                for (size_t l = j - 3, j1 = 0; j1 < 7; l++, j1++)
                    tmp_matrix[i1][j1] = pixels[k][l].rgbBlue;

            pixels[i][j].rgbBlue = overFlow(maskMatrix(tmp_matrix, coeffs));

            for (size_t k = i - 3, i1 = 0; i1 < 7; k++, i1++)
                for (size_t l = j - 3, j1 = 0; j1 < 7; l++, j1++)
                    tmp_matrix[i1][j1] = pixels[k][l].rgbGreen;

            pixels[i][j].rgbGreen = overFlow(maskMatrix(tmp_matrix, coeffs));

            for (size_t k = i - 3, i1 = 0; i1 < 7; k++, i1++)
                for (size_t l = j - 3, j1 = 0; j1 < 7; l++, j1++)
                    tmp_matrix[i1][j1] = pixels[k][l].rgbRed;

            pixels[i][j].rgbRed = overFlow(maskMatrix(tmp_matrix, coeffs));
        }
    }
}

void Image::noiseFilter() {
    for (size_t i = 1; i < bmiHeader.biHeight - 1; i++) {
        for (size_t j = 1; j < bmiHeader.biWidth - 1; j++) {
            std::vector<PIX> tmp;

            for (size_t k = i - 1, i1 = 0; i1 < 3; k++, i1++)
                for (size_t l = j - 1, j1 = 0; j1 < 3; l++, j1++)
                    tmp.push_back({0.3 * pixels[k][l].rgbRed +
                                       0.59 * pixels[k][l].rgbGreen +
                                       0.11 * pixels[k][l].rgbBlue,
                                   pixels[k][l]});

            std::sort(tmp.begin(), tmp.end(),
                      [](const PIX &first, const PIX &second) {
                          return first.value < second.value;
                      });
            pixels[i][j] = tmp[4].rgb;
        }
    }
}

void Image::changeBrightness(const int &value) {
    for (size_t i = 0; i < bmiHeader.biHeight; i++) {
        for (size_t j = 0; j < bmiHeader.biWidth; j++) {
            pixels[i][j].rgbRed += value;
            pixels[i][j].rgbGreen += value;
            pixels[i][j].rgbBlue += value;
        }
    }
}

void Image::changeContrast(const int &value) {
    RGBQUAD lAB = {0, 0, 0, 0};

    for (size_t i = 0; i < bmiHeader.biHeight; i++) {
        for (size_t j = 0; j < bmiHeader.biWidth; j++) {
            lAB.rgbRed += pixels[i][j].rgbRed * 0.299;
            lAB.rgbGreen += pixels[i][j].rgbGreen * 0.587;
            lAB.rgbBlue += pixels[i][j].rgbBlue * 0.114;
        }
    }

    lAB.rgbRed /= bmiHeader.biHeight * bmiHeader.biWidth * 0.299;
    lAB.rgbGreen /= bmiHeader.biHeight * bmiHeader.biWidth * 0.587;
    lAB.rgbBlue /= bmiHeader.biHeight * bmiHeader.biWidth * 0.114;

    double k = 1.0 + value / 100.0;

    for (size_t i = 0; i < bmiHeader.biHeight; i++) {
        for (size_t j = 0; j < bmiHeader.biWidth; j++) {
            double deltaB = pixels[i][j].rgbBlue - lAB.rgbBlue;
            pixels[i][j].rgbBlue = overFlow(lAB.rgbBlue + k * deltaB);

            double deltaG = pixels[i][j].rgbGreen - lAB.rgbGreen;
            pixels[i][j].rgbGreen = overFlow(lAB.rgbGreen + k * deltaG);

            double deltaR = pixels[i][j].rgbRed - lAB.rgbRed;
            pixels[i][j].rgbRed = overFlow(lAB.rgbRed + k * deltaR);
        }
    }
}

void Image::rotate_counterclockwise() {
    RGBQUAD **newPixels = new RGBQUAD *[bmiHeader.biWidth];

    for (size_t j = 0; j < bmiHeader.biWidth; j++) {
        newPixels[j] = new RGBQUAD[bmiHeader.biHeight];
    }

    for (int j = bmiHeader.biHeight - 1; j >= 0; j--) {
        for (size_t i = 0; i < bmiHeader.biWidth; i++) {
            newPixels[i][bmiHeader.biHeight - 1 - j] = pixels[j][i];
        }
    }

    free_pixels();
    std::swap(bmiHeader.biWidth, bmiHeader.biHeight);
    pixels = newPixels;
}

void Image::rotate_clockwise() {
    RGBQUAD **newPixels = new RGBQUAD *[bmiHeader.biWidth];

    for (size_t j = 0; j < bmiHeader.biWidth; j++) {
        newPixels[j] = new RGBQUAD[bmiHeader.biHeight];
    }

    for (int j = bmiHeader.biHeight - 1; j >= 0; j--) {
        for (size_t i = 0; i < bmiHeader.biWidth; i++) {
            newPixels[i][j] = pixels[j][bmiHeader.biWidth - 1 - i];
        }
    }

    free_pixels();
    std::swap(bmiHeader.biWidth, bmiHeader.biHeight);
    pixels = newPixels;
}

bool Image::isNone() { return pixels == 0; }

unsigned short Image::read_u16(FILE *fp) {
    unsigned char b0, b1;

    b0 = getc(fp);
    b1 = getc(fp);

    return ((b1 * 256) + b0);
}

unsigned int Image::read_u32(FILE *fp) {
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((((((b3 * 256) + b2) * 256) + b1) * 256) + b0);
}

int Image::read_s32(FILE *fp) {
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((int)(((((b3 * 256) + b2) * 256) + b1) * 256) + b0);
}

void Image::write_u16(unsigned short input, FILE *fp) {
    putc(input, fp);
    putc((input / 256) % 256, fp);
}

void Image::write_u32(unsigned int input, FILE *fp) {
    putc(input, fp);
    putc((input / 256) % 256, fp);
    putc((input / 256 / 256) % 256, fp);
    putc((input / 256 / 256 / 256) % 256, fp);
}

void Image::write_s32(int input, FILE *fp) {
    putc(input, fp);
    putc((input / 256) % 256, fp);
    putc((input / 256 / 256) % 256, fp);
    putc((input / 256 / 256 / 256) % 256, fp);
}

double Image::maskMatrix(double A[7][7], double **B) {
    double res = 0;

    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
            res += A[i][j] * B[i][j];

    return res;
}

double Image::overFlow(double midpix) {
    if (midpix > 255) {
        return 255;
    } else if (midpix < 0) {
        return 0;
    } else {
        return midpix;
    }
}

double Image::gaussianModel(double x, double y, double sigma) {
    return 1. / exp(-(x * x + y * y) / (2 * sigma * sigma));
}

double **Image::generate_coeff(int radius, double sigma) {
    double **coeff = new double *[radius];

    double sum = 0;
    for (int i = 0; i < radius; i++) {
        coeff[i] = new double[radius];
        for (int j = 0; j < radius; j++) {
            coeff[i][j] = gaussianModel(i - radius / 2, j - radius / 2, sigma);
            sum += coeff[i][j];
        }
    }

    for (int i = 0; i < radius; i++)
        for (int j = 0; j < radius; j++)
            coeff[i][j] /= sum;

    return coeff;
}
