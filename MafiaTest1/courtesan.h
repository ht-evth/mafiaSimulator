#ifndef COURTESAN_H
#define COURTESAN_H

#include "card.h"

class Courtesan : public Card
{
public:
    Courtesan();
    ~Courtesan();

    Player* dayChoice(QList<Player*> *players, int myIndex, QList<QPair<int,int>> *lastVote) override;
    Player* nightChoice(QList<Player *> *players, int index, QList<QPair<int,int>> *lastVote) override;
    QString whatIsTheRole() override;
};

#endif // COURTESAN_H
