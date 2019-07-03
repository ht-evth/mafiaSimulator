#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QLabel>
#include <QGroupBox>
#include <game.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

        QList<QLabel*> images;
        QList<QGroupBox*> Gbox;
        QPixmap *myPixmap;
        bool isStarted;

        int numPlayers;


        QList<QLabel*> labels_players_speech;
        QTimer *timer;
        Game *currentGame;

        void updateInfo();
        void createImages();
        void make_labels_for_info_about_players(int num_);

    private slots:
        void stepByStepForGame();
        void on_pushButton_clicked();
        void on_ViewInfoButton_clicked();
        void on_buttonStart_clicked();
};

#endif // MAINWINDOW_H
