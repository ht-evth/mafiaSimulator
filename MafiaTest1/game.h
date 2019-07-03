#ifndef GAME_H
#define GAME_H

#include "qlist.h"
#include "player.h"
#include "QLabel"
#include "QTextEdit"
#include "qpair.h"
#include "QTimer"

#include "QRandomGenerator"


class Game
{
    //singleton
public:
    ~Game();
    static Game* getInstance();

    void setSpeachLabel(QLabel *speachLabel);       //Установить поле для вывода сообщений
    void setLogEdit(QTextEdit *log);                //Установить поле лога

    QTextEdit* getLogEdit();

    void addPlayer();                               //Добавить игрока
    void deleteAllPlayers();                        //Удалить всех игроков
    void makeTrustList();                           //Сформировать список симпатий / доверий

    QList<Player*> *GetPlayers();                   //Получить ссылку на список игроков

    void createGame(int numberOfPlayers);           //Создать игру (создать объекты игрков)
    void startGame();                               //Старт игры

    bool isGameOver();
    void setGameOver(bool status);               //Установить статус игры
    void makeStep();                                //Сделать игровой шаг
    void updateGameInfo();
    void setAliveImages();
    void updateTrust();

    QList<QPair<int, int>> lastVote;                //Тут храним результаты дневного голосования

    int numPlayers;			// кол-во тех или иных игроков
    int numMafia;
    int numCivilians;
    int numDetectives;
    int numDoctors;
    int numCourtesans;

    QTimer timer;

    static QRandomGenerator random;
private:
    Game();
    Game(int num_players);

    int step = 0;

    void distributeCards(); //Распределить роли
    bool checkGameOver();

    //Шаги игры
    void greeting();
    void courtesan_2();
    void mafia_1();
    void mafia_2();
    Player* combiningVotesVafia(QList<QPair<int,int>> *votes);
    void mafia_3();
    void doctor_2();
    void detective_2();

    void WakeUp(Player *player);
    void FallAsleep(Player *player);

    void day_1();
    void day_2();
    Player* combiningVotesDay();
    void day_3();

    QList<Player*> Players; //Список игроков
    QList<Player*> mafias;
    Player* courtesan;
    Player* doctor;
    Player* detective;

    //Храним выбор каждой роли
    Player* mafiasChoice;
    Player* detectivesChoice;
    Player* doctorsChoice;
    Player* courtesansChoice;
    Player* dayChoice;

    bool GameOver;

    QLabel *speachLabel;       //Указатель на label комментатора из mainform
    QTextEdit *log;
    static Game* currentGame;
};


#endif // GAME_H
