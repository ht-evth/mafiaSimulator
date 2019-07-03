#ifndef DETECTIVE_H
#define DETECTIVE_H

#include "card.h"

class Detective : public Card
{
public:
    Detective();
    ~Detective();

    Player* dayChoice(QList<Player*> *players, int myIndex, QList<QPair<int,int>> *lastVote) override;
    Player* nightChoice(QList<Player *> *players, int index, QList<QPair<int,int>> *lastVote) override;
    QString whatIsTheRole() override;

    void setNotCheckedPlayers(QList<Player *> *players, int index);

private:

    bool check;
    QList<Player*> checkedPlayers;
    QList<Player*> notCheckedPlayers;

};

#endif // DETECTIVE_H
