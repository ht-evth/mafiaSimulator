#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "game.h"
#include "qgroupbox.h"

#include <QThread>
#include <QTimer>
#include <QMessageBox>
#include <exception>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    this->currentGame = Game::getInstance();
    this->isStarted = false;

    ui->setupUi(this);
    this->currentGame->setSpeachLabel(this->ui->speachLabel);
    this->currentGame->setLogEdit(this->ui->textLog);

    myPixmap = new QPixmap("img/image.png");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    int num;
    try {
        num = ui->NumPlayersEdit->toPlainText().toInt();
    } catch (std::exception) {

        return;
    }


    if (num < 3 || num > 12)
    {
        QMessageBox::warning(this, "Внимание","Количество игроков: 3-12");
        return;
    }

    this->numPlayers = num;
    currentGame->createGame(num);
    ui->spinBoxViewInfo->setRange(1, num);


    updateInfo();
    createImages();

    ui->buttonStart->setEnabled(true);
}

void MainWindow::createImages()
{
    int num = currentGame->GetPlayers()->count();

    for (int i = 0; i < Gbox.count(); i++) {
        delete Gbox[i];
    }

    Gbox.clear();
    images.clear();

    QString name;

    int x = 10, y = 141, width = 141;


    for (int i = 0; i < num; i++)
    {
        if (i == 6) { x = 10; y = 305; }

        Gbox << new QGroupBox(this);

        images << new QLabel("тут картинка", Gbox[i]);
        images[i]->setGeometry(10, 20, 121, 141);
        images[i]->setPixmap(*myPixmap);

        Gbox[i]->setGeometry(x, y, width, 161);
        name = "Player";
        name += QString::number(i+1);
        Gbox[i]->setTitle(name);

        Gbox[i]->show();
        images[i]->show();

        currentGame->GetPlayers()->at(i)->SetImageLabel(images[i]);

        x += width + 10;
    }
}


void MainWindow::updateInfo()
{

    ui->labelInfo->setText("Игроков: " + QString::number(currentGame->numPlayers) +\
                                     "\nМафия: " + QString::number(currentGame->numMafia) +\
                                     "\nМирные жители: " + QString::number(currentGame->numCivilians) +\
                                     "\nДетективы: " + QString::number(currentGame->numDetectives) +\
                                     "\nДоктора: " + QString::number(currentGame->numDoctors) +\
                                     "\nКуртизанки: " + QString::number(currentGame->numCourtesans));
}



void MainWindow::on_ViewInfoButton_clicked()
{
    int index = ui->spinBoxViewInfo->value();

    if(index > currentGame->GetPlayers()->count())
    {
        ui->labelPlayerStatus->setText("Упс, произошла какая-то ошибка.\nНе могу получить доступ к\n игроку с этим номером");
        return;
    }
    Player *currentPlayer = currentGame->GetPlayers()->at(index - 1);

    ui->labelPlayerStatus->setText("Имя: " + currentPlayer->info.name +\
                           "\n\nРоль: " + currentPlayer->getRole() +\
                           "\n\nСостояние: " + currentPlayer->getStatus() +\
                           "\n\nВозраст: " + QString::number(currentPlayer->info.age) +\
                           "\n\nПол: " + currentPlayer->getSex() +\
                           "\n\nКрасноречие: " + QString::number(currentPlayer->info.oratory) +\
                           "\n\nХитрость: " + QString::number(currentPlayer->info.cunning) +\
                           "\n\nВолнение: " + QString::number(currentPlayer->info.excitement) +\
                                   "\n\nДоверие: " + currentPlayer->getTrust());
}

void MainWindow::on_buttonStart_clicked()
{

    if (!this->isStarted)
    {
        ui->buttonStart->setText("Стоп");
        ui->pushButton->setEnabled(false);
        this->isStarted = true;
        currentGame->createGame(numPlayers);
        currentGame->setGameOver(false);
        currentGame->lastVote.clear();
        timer = new QTimer();
        timer->setInterval(100);
        currentGame->startGame();
        connect(timer, SIGNAL(timeout()), this, SLOT(stepByStepForGame()));
        timer->start();
    }
    else
    {
        timer->stop();
        this->isStarted = false;
        ui->pushButton->setEnabled(true);
        ui->speachLabel->setText("Игра была прервана. \nПри нажатии кнопки \"Старт\" будет создана новая игра");
        ui->buttonStart->setText("Старт");
    }

}

void MainWindow::stepByStepForGame()
{
    timer->setInterval(0.5 * 1000);
    if (!currentGame->isGameOver())
    {
        currentGame->makeStep();
        currentGame->updateGameInfo();
        this->updateInfo();
        //Вскрываем роли
        if(currentGame->isGameOver())
            currentGame->setAliveImages();
    }
}
