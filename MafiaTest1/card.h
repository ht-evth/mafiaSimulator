#ifndef CARD_H
#define CARD_H

#include "player.h"
#include "QPixmap"
#include "QLabel"

class Card
{
public:
    Card(); 
    virtual ~Card();

    void getIncognitoImage();


    virtual Player* dayChoice(QList<Player*> *players, int myIndex, QList<QPair<int,int>> *lastVote) = 0;
    virtual Player* nightChoice(QList<Player *> *players, int index, QList<QPair<int,int>> *lastVote) = 0;


    virtual QString whatIsTheRole() = 0;

    QPixmap *alive;
    QPixmap *dead;
    QPixmap *incognito;

};

#endif // CARD_H
