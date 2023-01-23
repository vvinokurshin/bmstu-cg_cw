#include <iostream>

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "imageProcessing.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

static ssize_t curState = -1;
static std::vector<Image> states;

void MainWindow::on_loadImageBtn_clicked()
{
    auto file = QFileDialog::getOpenFileName(nullptr, "Загрузка фотографии", "../coursework/img/");

    if (file.isNull()) {
        QMessageBox::critical(this, "Ошибка","Ошибка при чтении файла.");
        return;
    }

    std::string fileName = file.toStdString();
    Image img;

    if (!img.load(fileName)) {
        QMessageBox::critical(this, "Ошибка","Ошибка при чтении файла.");
        return;
    }

    QPixmap pixmap;

    ui->imagePlace->setPixmap(pixmap.fromImage(img.DataToQImage()).scaled(ui->imagePlace->frameSize(), Qt::KeepAspectRatio));
    ui->imagePlace->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->input_filename->setText(QFileInfo(file.toUtf8().data()).absoluteFilePath());
    
    states.clear();
    curState = 0;
    states.push_back(img);

    ui->saveBtn->setEnabled(true);
    ui->nextBtn->setEnabled(false);
    ui->backBtn->setEnabled(false);
}

void MainWindow::on_filterBtn_clicked()
{
    if (curState == -1) {
        QMessageBox::critical(this, "Ошибка","Сначала загрузите файл.");
        return;
    }

    QButtonGroup group;
    QList<QRadioButton *> allButtons = ui->menuFrame->findChildren<QRadioButton *>();
    
    for(int i = 0; i < allButtons.size(); ++i)
    {
        group.addButton(allButtons[i],i);
    }

    if (states.size() != curState + 1 && !states[curState + 1].isNone()) {
        while (states.size() != curState + 1) {
            states.pop_back();
        }

        ui->nextBtn->setEnabled(false);
    }

    states.push_back(Image(states[curState++]));
    ui->backBtn->setEnabled(true);

    switch (group.checkedId()) {
        case 0:
            states[curState].negativeFilter();
            break;
        case 1:
            states[curState].blurFilter();
            break;
        case 2:
            states[curState].grayFilter();
            break;
        case 3:
            states[curState].noiseFilter();
            break;
    }

     QPixmap pixmap;
    ui->imagePlace->setPixmap(pixmap.fromImage(states[curState].DataToQImage()).scaled(ui->imagePlace->frameSize(),Qt::KeepAspectRatio));
}


void MainWindow::on_backBtn_clicked()
{
    if (curState == 0) {
       QMessageBox::critical(this, "Ошибка","Отменять нечего.");
       return;
    }

    QPixmap pixmap;
    ui->imagePlace->setPixmap(pixmap.fromImage(states[--curState].DataToQImage()).scaled(ui->imagePlace->frameSize(),Qt::KeepAspectRatio));

    if (curState == 0) {
        ui->backBtn->setEnabled(false);
    }

    ui->nextBtn->setEnabled(true);
}


void MainWindow::on_nextBtn_clicked()
{
    if (curState >= states.size() - 1) {
        QMessageBox::critical(this, "Ошибка","Повторять нечего.");
        return;
    }

    ui->backBtn->setEnabled(true);

    QPixmap pixmap;
    ui->imagePlace->setPixmap(pixmap.fromImage(states[++curState].DataToQImage()).scaled(ui->imagePlace->frameSize(),Qt::KeepAspectRatio));

    if (curState >= states.size() - 1) {
        ui->nextBtn->setEnabled(false);
    } else {
        ui->nextBtn->setEnabled(true);
    }
}


void MainWindow::on_saveBtn_clicked()
{
    if (curState == -1) {
        QMessageBox::critical(this, "Ошибка","Сначала загрузите файл.");
        return;
    }

    auto file = QFileDialog::getSaveFileName(nullptr, "Загрузка фотографии", "../coursework/img/");

    if (file.isNull()) {
        QMessageBox::critical(this, "Ошибка","Ошибка при чтении файла.");
        return;
    }

    std::string fileName = file.toStdString();

    if (!states[curState].save(fileName)) {
        QMessageBox::critical(this, "Ошибка","Ошибка при чтении файла.");
        return;
    }
}


void MainWindow::on_settingsBtn_clicked()
{
    if (curState == -1) {
        QMessageBox::critical(this, "Ошибка","Сначала загрузите файл.");
        return;
    }

    if (states.size() != curState + 1 && !states[curState + 1].isNone()) {
        while (states.size() != curState + 1) {
            states.pop_back();
        }

        ui->nextBtn->setEnabled(false);
    }

    int brightness = ui->scrollBrightness->value(), contrast = ui->scrollContrast->value();
    states.push_back(Image(states[curState++]));
    states[curState].changeBrightness(brightness);
    states[curState].changeContrast(contrast);
    
    QPixmap pixmap;
    ui->imagePlace->setPixmap(pixmap.fromImage(states[curState].DataToQImage()).scaled(ui->imagePlace->frameSize(),Qt::KeepAspectRatio));
    ui->backBtn->setEnabled(true);
}


void MainWindow::on_counterclockwiseBtn_clicked()
{
    if (curState == -1) {
        QMessageBox::critical(this, "Ошибка","Сначала загрузите файл.");
        return;
    }

    states.push_back(Image(states[curState++]));
    states[curState].rotate_counterclockwise();
    QPixmap pixmap;
    ui->imagePlace->setPixmap(pixmap.fromImage(states[curState].DataToQImage()).scaled(ui->imagePlace->frameSize(),Qt::KeepAspectRatio));
    ui->backBtn->setEnabled(true);
}


void MainWindow::on_clockwiseBtn_clicked()
{
    if (curState == -1) {
        QMessageBox::critical(this, "Ошибка","Сначала загрузите файл.");
        return;
    }

    states.push_back(Image(states[curState++]));
    states[curState].rotate_clockwise();
    QPixmap pixmap;
    ui->imagePlace->setPixmap(pixmap.fromImage(states[curState].DataToQImage()).scaled(ui->imagePlace->frameSize(),Qt::KeepAspectRatio));
    ui->backBtn->setEnabled(true);
}

