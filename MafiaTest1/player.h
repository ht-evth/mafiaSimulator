#ifndef PLAYER_H
#define PLAYER_H


#include <QApplication>
#include "QLabel"

class Card;

struct Info
{
    QString name;		// < 15
    bool alive;			//false - dead, true - alive
    bool isWakeUp;		//false - sleeps, true - can play
    int sex;			// 0 - male, 1 - female
    int age;			// from 18 to 40
    int oratory;		// from 0 to 10 (for male 70% from 6 to 10; for female 70% from 1 to 5;)
    int cunning;		// from 0 to 10
    int excitement;     // волнение from 0 to 10
    QList<int> trust;
};

class Player
{
public:
    Player();
    ~Player();

    QString reason;


    void setRole(Card* role);
    void setAliveImage();
    void setDeadImage();
    void setIncognitoImage();
    void SetImageLabel(QLabel *image);

    QString getRole();
    QString getStatus();
    QString getSex();
    QString getTrust();
    Card* getCard();

    int getIndex();


    Info info;
private:
    static int count;

    int myIndex;
    Card *role;
    QLabel *image;


};

#endif // PLAYER_H
