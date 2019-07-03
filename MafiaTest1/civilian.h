#ifndef CIVILIAN_H
#define CIVILIAN_H

#include "card.h"

class Civilian: public Card
{
public:
    Civilian();
    ~Civilian();

    Player* dayChoice(QList<Player *> *players, int myIndex,QList<QPair<int,int>> *lastVote) override;
    Player* nightChoice(QList<Player *> *players, int index, QList<QPair<int,int>> *lastVote) override;
    QString whatIsTheRole() override;
private:

    int a;
};

#endif // CIVILIAN_H
