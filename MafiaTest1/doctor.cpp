#include "doctor.h"
#include "QRandomGenerator"
#include "QSet"


Doctor::Doctor()
{
    this->incognito = new QPixmap("img/image.png");
    this->alive = new QPixmap("img/doctor.png");
    this->dead = new QPixmap("img/deaddoctor.png");
    this->isLastNightHimself = false;
}


Doctor::~Doctor()
{
    delete this;
}

QString Doctor::whatIsTheRole()
{
    return "Доктор";
}

Player* Doctor::nightChoice(QList<Player *> *players, int index, QList<QPair<int,int>> *lastVote)
{
   //Доктор выбирает кого вылечить
    QString text = "";
    int rnd = QRandomGenerator::global()->bounded(1, 100);
    Player *choice = nullptr;


    int himSelf = 66;           //Число от himself до 100 будет влиять на выбор доктора (лечение себя)

    //Если пытались выгнать доктора в прошлый раз, то увеличиваем шанс вылечить самого себя
    for (int i = 0; i < lastVote->count(); i++) {
        if (lastVote->at(i).second == index)
        {
            himSelf = 49;
            break;
        }
    }


   //Если он выбирал себя прошлой ночью, в эту ночь выбирать кого-то из остальных
    if (isLastNightHimself == true)
    {
        text += "Доктор вылечил себя прошлой ночью. На этот раз так не получится!\n";
        himSelf = 101;
    }

    if (rnd > himSelf)
    {
        choice = players->at(index);
        text += "\nДоктор выбрал сам себя (" + choice->info.name + ")";
        isLastNightHimself = true;
    }
    else if (rnd < himSelf && !lastVote->isEmpty() && choice == nullptr)
    {
        text += "Доктор решает вылечить одного из тех, кого пытались выгнать днём.\n \
                Он выбрал игрока, которому больше всех доверял - ";


        //Составляем список игроков с помощью индексов second в lastVote
        QSet<Player *> list;
        for(Player* curPlayer: *players)
        {
            if(curPlayer->getIndex() != index && curPlayer->info.alive == true)
            {
                int curIndex = curPlayer->getIndex();

                //Если за этого игрока голосовали, добавляем
                for (int i = 0; i < lastVote->count(); i++)
                {
                    if(lastVote->at(i).second == curIndex)
                    {
                        list << curPlayer;
                        break;
                    }
                }
            }
        }

        int maxTrust = -1;

         //Поиск того, кому доктор доверяет больше всего
        for (Player* curPlayer : list)
        {
            //Получаем индекс
            int curIndex = curPlayer->getIndex();
            if(players->at(index)->info.trust[curIndex] > maxTrust)
            {
                choice = curPlayer;
                maxTrust = players->at(index)->info.trust[curIndex];
            }
        }

        text += choice->info.name;

    }


    if (choice == nullptr)  //Если выпало число от 1 до 20, то выбираем случайного игрока
    {

        text += "Доктор решил довериться интуиции и выбрал случайного игрока - ";

        //Составим список игроков без доктора
        QList<Player *> list;


        for(Player* curPlayer: *players)
            if(curPlayer->getIndex() != index && curPlayer->info.alive == true)
                list << curPlayer;
        //Выберем случайный индекс:

        int indexForReturn = QRandomGenerator::global()->bounded(0, list.size());
        choice = list.at(indexForReturn);
        text += choice->info.name;
    }

    if(choice != players->at(index))
        isLastNightHimself = false;

    players->at(index)->reason = text;

    return choice;


}

Player* Doctor::dayChoice(QList<Player *> *players, int myIndex, QList<QPair<int, int> > *lastVote)
{
    Player* choice = nullptr;
    QString text = "";
    //Выбор самого худшего по доверию
    int minTrust = 1000;
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
