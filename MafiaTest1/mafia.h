#ifndef MAFIA_H
#define MAFIA_H

#include "card.h"


class Mafia : public Card
{
public:
    Mafia();
    ~Mafia() ;

    Player* dayChoice(QList<Player*> *players, int myIndex, QList<QPair<int,int>> *lastVote) override;
    Player* nightChoice(QList<Player *> *players, int index, QList<QPair<int,int>> *lastVote) override;
    QString whatIsTheRole() override;

    bool isItMafia(int curIndex, QList<int> *indexes);
};

#endif // MAFIA_H
