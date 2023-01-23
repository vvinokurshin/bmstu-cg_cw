// Фильтр негатива
void Image::negativeFilter() {
    for (size_t i = 0; i < bmiHeader.biHeight; i++) {
        for (size_t j = 0; j < bmiHeader.biWidth; j++) {
            pixels[i][j].rgbBlue = 255 - 
                pixels[i][j].rgbBlue;
            pixels[i][j].rgbGreen = 255 - 
                pixels[i][j].rgbGreen;
            pixels[i][j].rgbRed = 255 - 
                pixels[i][j].rgbRed;
        }
    }
}

// Фильтр оттенков серого
void Image::grayFilter() {
    for (size_t i = 0; i < bmiHeader.biHeight; i++) {
        for (size_t j = 0; j < bmiHeader.biWidth; j++) {
            double s = (pixels[i][j].rgbBlue + 
                        pixels[i][j].rgbGreen +
                        pixels[i][j].rgbRed) / 3;

            pixels[i][j].rgbBlue = s;
            pixels[i][j].rgbGreen = s;
            pixels[i][j].rgbRed = s;
        }
    }
}

// Фильтр размытия
void Image::blurFilter() {
    double **coeffs = generate_coeff(7, 1.94);
    double tmp_matrix[7][7];

    for (size_t i = 3; i < bmiHeader.biHeight - 3; i++) 
    {
        for (size_t j = 3; j < bmiHeader.biWidth - 3; j++)
        {
            for (size_t k = i - 3, i1 = 0; i1 < 7; 
                k++, i1++)
                for (size_t l = j - 3, j1 = 0; j1 < 7; 
                    l++, j1++)
                    tmp_matrix[i1][j1] = 
                        pixels[k][l].rgbBlue;

            pixels[i][j].rgbBlue = 
                overFlow(maskMatrix(tmp_matrix, coeffs));

            for (size_t k = i - 3, i1 = 0; i1 < 7;
                k++, i1++)
                for (size_t l = j - 3, j1 = 0; j1 < 7;
                    l++, j1++)
                    tmp_matrix[i1][j1] = 
                        pixels[k][l].rgbGreen;

            pixels[i][j].rgbGreen = 
                overFlow(maskMatrix(tmp_matrix, coeffs));

            for (size_t k = i - 3, i1 = 0; i1 < 7; 
                k++, i1++)
                for (size_t l = j - 3, j1 = 0; j1 < 7; 
                    l++, j1++)
                    tmp_matrix[i1][j1] = 
                        pixels[k][l].rgbRed;

            pixels[i][j].rgbRed = 
                overFlow(maskMatrix(tmp_matrix, coeffs));
        }
    }
}

// Фильтр упонимежния шума
void Image::noiseFilter() {
    for (size_t i = 1; i < bmiHeader.biHeight - 1; i++) 
    {
        for (size_t j = 1; j < bmiHeader.biWidth - 1; j++) 
        {
            std::vector<PIX> tmp;

            for (size_t k = i - 1, i1 = 0; i1 < 3; 
                k++, i1++)
                for (size_t l = j - 1, j1 = 0; j1 < 3; 
                    l++, j1++)
                    tmp.push_back({
                        0.3 * pixels[k][l].rgbRed + 
                        0.59 * pixels[k][l].rgbGreen +
                        0.11 * pixels[k][l].rgbBlue, 
                        pixels[k][l]});

            std::sort(tmp.begin(), tmp.end(),
                      [](const PIX &first, 
                         const PIX &second) {
                          return first.value < 
                          second.value;
                      });
                      
            pixels[i][j] = tmp[4].rgb;
        }
    }
}

// Применение маски к матрице
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