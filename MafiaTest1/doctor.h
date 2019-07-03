#ifndef DOCTOR_H
#define DOCTOR_H

#include "card.h"

class Doctor : public Card
{
public:
    Doctor();
    ~Doctor();

    Player* dayChoice(QList<Player*> *players, int myIndex, QList<QPair<int,int>> *lastVote) override;
    Player* nightChoice(QList<Player *> *players, int index, QList<QPair<int,int>> *lastVote) override;
    QString whatIsTheRole() override;

private:
    bool isLastNightHimself;
};

#endif // DOCTOR_H
