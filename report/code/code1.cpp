// Изменение яркости
void Image::changeBrightness(const int &value) {
    for (size_t i = 0; i < bmiHeader.biHeight; i++) {
        for (size_t j = 0; j < bmiHeader.biWidth; j++) {
            pixels[i][j].rgbRed += value;
            pixels[i][j].rgbGreen += value;
            pixels[i][j].rgbBlue += value;
        }
    }
}

// Изменение контрастности
void Image::changeContrast(const int &value) {
    RGBQUAD lAB = {0, 0, 0, 0};

    for (size_t i = 0; i < bmiHeader.biHeight; i++) {
        for (size_t j = 0; j < bmiHeader.biWidth; j++) {
            lAB.rgbRed += pixels[i][j].rgbRed;
            lAB.rgbGreen += pixels[i][j].rgbGreen;
            lAB.rgbBlue += pixels[i][j].rgbBlue * 0.114;
        }
    }

    lAB.rgbRed /= bmiHeader.biHeight * 
        bmiHeader.biWidth * 0.299;
    lAB.rgbGreen /= bmiHeader.biHeight * 
        bmiHeader.biWidth * 0.587;
    lAB.rgbBlue /= bmiHeader.biHeight * 
        bmiHeader.biWidth * 0.114;
    double k = 1.0 + value / 100.0;

    for (size_t i = 0; i < bmiHeader.biHeight; i++) {
        for (size_t j = 0; j < bmiHeader.biWidth; j++) {
            double deltaB = pixels[i][j].rgbBlue - 
                lAB.rgbBlue;
            pixels[i][j].rgbBlue = overFlow(lAB.rgbBlue + 
                k * deltaB);

            double deltaG = pixels[i][j].rgbGreen - 
                lAB.rgbGreen;
            pixels[i][j].rgbGreen = overFlow(lAB.rgbGreen
                + k * deltaG);

            double deltaR = pixels[i][j].rgbRed - 
                lAB.rgbRed;
            pixels[i][j].rgbRed = overFlow(lAB.rgbRed + 
                 * deltaR);
        }
    }
}