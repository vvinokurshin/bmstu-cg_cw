#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

 private slots:
    void on_loadImageBtn_clicked();

    void on_filterBtn_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_nextBtn_clicked();

    void on_backBtn_clicked();

    void on_saveBtn_clicked();

    void on_settingsBtn_clicked();

    void on_scrollBrightness_actionTriggered(int action);

    void on_counterclockwiseBtn_clicked();

    void on_clockwiseBtn_clicked();

 private:
    Ui::MainWindow *ui;
};

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QWheelEvent>

class View : public QGraphicsView {
    Q_OBJECT
 public:
    explicit View(QWidget *parent = 0) : QGraphicsView(parent) {
        setDragMode(QGraphicsView::ScrollHandDrag);

        QGraphicsPixmapItem *pixmapItem =
            new QGraphicsPixmapItem(QPixmap(":/img/source.bmp"));
        pixmapItem->setTransformationMode(Qt::SmoothTransformation);

        QGraphicsScene *scene = new QGraphicsScene();
        scene->addItem(pixmapItem);
        setScene(scene);
    }

 protected Q_SLOTS:
    void wheelEvent(QWheelEvent *event) {
        if (event->AcceptDropsChange > 0)
            scale(1.25, 1.25);
        else
            scale(0.8, 0.8);
    }

    void keyPressEvent(QKeyEvent *event) {
        if (event->key() == Qt::Key_Left)
            rotate(1);
        else if (event->key() == Qt::Key_Right)
            rotate(-1);
    }
};

#endif // MAINWINDOW_H
