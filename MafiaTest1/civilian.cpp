#include "civilian.h"
#include <iostream>

Civilian::Civilian()
{
    this->incognito = new QPixmap("img/image.png");
    this->alive = new QPixmap("img/civilian.png");
    this->dead = new QPixmap("img/deadcivilian.png");
}

Civilian::~Civilian()
{
    delete this;
}

Player* Civilian::dayChoice(QList<Player *> *players, int myIndex, QList<QPair<int,int>> *lastVote)
{
    Player* choice = nullptr;
    QString text = "";
    //Выбор самого худшего по доверию
    int minTrust = 11;
    for (auto var : *players) {
        if(var->info.alive == true && players->at(myIndex)->info.trust[var->getIndex()] < minTrust)
        {
            minTrust = players->at(myIndex)->info.trust[var->getIndex()];
            choice = var;
        }

    }

    text += "\n" + players->at(myIndex)->info.name + " проголосовал против " + choice->info.name;
    players->at(myIndex)->reason = text;

    QPair<int,int> pair(myIndex, choice->getIndex());
    *lastVote << pair;
    return choice;
}

Player* Civilian::nightChoice(QList<Player *> *players, int index, QList<QPair<int,int>> *lastVote)
{
    return nullptr;
}

QString Civilian::whatIsTheRole()
{
    return "Мирный";
}





