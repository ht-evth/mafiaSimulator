#include "courtesan.h"
#include "QRandomGenerator"

Courtesan::Courtesan()
{
    this->incognito = new QPixmap("img/image.png");
    this->alive = new QPixmap("img/lady.png");
    this->dead = new QPixmap("img/deadlady.png");
}

Courtesan::~Courtesan()
{
    delete this;
}

QString Courtesan::whatIsTheRole()
{
    return "Куртизанка";
}

Player* Courtesan::nightChoice(QList<Player *> *players, int index, QList<QPair<int,int>> *lastVote)
{
    // Пусть данный персонаж считает приоритетной целью ту,
    // у которой наименьшее доверие с обоих сторон, чтобы втереться в доверие
    // Или просто случайную

    Player *choice = nullptr;
    QString text = "";

    int decision = QRandomGenerator::global()->bounded(1, 100);

    //Если четное - наименьшее доверие
    if (decision % 2 == 0)
    {
        text += "\nКуртизанка выбрала самого не доверяющего - ";

        //Перебор списка игроков и поиск среди них с наименьшим доверием
        int min = 100000;

        for (int i = 0; i < players->count(); i++)
        {
            Player* curPlayer = players->at(i);
            //Если игрок не выбыл, запоминаем минимум
            if (curPlayer->info.alive)
                if (curPlayer->info.trust[index] < min)
                {
                    min = curPlayer->info.trust[index];
                    choice = curPlayer;
                }

        }


    }
    else
    {
        text += "\nКуртизанка решила довериться считалочке и выбрала ";

        //Создаем список без куртизанки и с живыми игроками
        QList<Player*> goodList;
        for (int i = 0; i < players->count(); i++)
        {
            Player *temp = players->at(i);
            if (temp->info.alive && temp->getCard()->whatIsTheRole() != whatIsTheRole())
                goodList << temp;

        }

        //Выбираем из них рандомного

        if (!goodList.empty())
        {
            decision = QRandomGenerator::global()->bounded(0, goodList.count());
            choice = goodList.at(decision);
        }


    }

    //Выбраный игрок получает иммунитет от остальных выборов

    text += choice->info.name + "\n";
    players->at(index)->reason = text;
    return choice;
}

Player* Courtesan::dayChoice(QList<Player *> *players, int myIndex, QList<QPair<int, int> > *lastVote)
{
    Player* choice = nullptr;
    QString text = "";
    //Выбор самого волнующегося
    int maxExcitement = -1;
    for (auto var : *players) {
        if(var->info.alive == true && players->at(var->getIndex())->info.excitement > maxExcitement)
        {
            maxExcitement = players->at(var->getIndex())->info.excitement;
            choice = var;
        }

    }

    text += "\n" + players->at(myIndex)->info.name + " проголосовал против " + choice->info.name;
    players->at(myIndex)->reason = text;
    QPair<int,int> pair(myIndex, choice->getIndex());
    *lastVote << pair;
    return choice;
}
