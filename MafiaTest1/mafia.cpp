#include "mafia.h"
#include "QRandomGenerator"

Mafia::Mafia()
{
    this->incognito = new QPixmap("img/image.png");
    this->alive = new QPixmap("img/mafia.png");
    this->dead = new QPixmap("img/deadmafia.png");
}


Mafia::~Mafia()
{
    delete this;
}

QString Mafia::whatIsTheRole()
{
    return "Мафия";
}


Player* Mafia::nightChoice(QList<Player *> *players, int myIndex, QList<QPair<int,int>> *lastVote)
{
    //Мафия проверяет голосовал ли кто-то конкретно за нее.

    QString text = "Мафия ";
    text += "(" + players->at(myIndex)->info.name + ") ";
    Player *choice = nullptr;

    //Запомним индексы всех мафий
    QList<int> indexes;
    indexes.clear();
    for (auto curPlayer: *players) {
        if(curPlayer->getCard()->whatIsTheRole() == this->whatIsTheRole())
            indexes << curPlayer->getIndex();

    }


    QList<Player *> versusMe, versusMafias;
    for (auto curVote : *lastVote)
    {
        if (curVote.first < players->count() && curVote.second < players->count()){
            //Если проголосовали против меня и этот игрок ещё жив, запоминаем его
            if (curVote.second == myIndex && players->at(curVote.first)->info.alive == true)
                versusMe << players->at(curVote.first);

            //Если проголосовали против мафии, запомним его
            if (isItMafia(curVote.second, &indexes) && players->at(curVote.first)->info.alive == true)
                versusMafias << players->at(curVote.first);
        }
    }

    int firstChance = 50, secondChance = 80;


    int rnd = QRandomGenerator::global()->bounded(1, 100);

    if(rnd <= firstChance && !versusMe.isEmpty())
    {
        text += "решает отомстить ";

        //Выбор игрока среди versusMe
        int rndIndex = QRandomGenerator::global()->bounded(0, versusMe.count());
        choice = versusMe.at(rndIndex);


        text += choice->info.name + " за то, что он пытался его выгнать";
    }
    else if (rnd <= secondChance && !versusMafias.isEmpty()) {
        text += "решает отомстить за себя и своих братьев. Выбор пал на ";

        //Выбор игрока среди versusMafias

        int rndIndex = QRandomGenerator::global()->bounded(0, versusMafias.count());
        choice = versusMafias.at(rndIndex);
        text += choice->info.name;

    }
    else
    {
        //Выбор самого худшего по доверию
        text += "выбирает того, котому меньше доверяет - ";
        int minTrust = 11;
        for (auto curPlayer : *players)
        {
            if(curPlayer->info.alive == false || curPlayer->getCard()->whatIsTheRole() == whatIsTheRole())
                continue;
            else
                if(players->at(myIndex)->info.trust[curPlayer->getIndex()] < minTrust)
                {
                    minTrust = players->at(myIndex)->info.trust[curPlayer->getIndex()];
                    choice = curPlayer;
                }
        }

        text += choice->info.name;

    }



    players->at(myIndex)->reason = text;
    return choice;
}

bool Mafia::isItMafia(int curIndex, QList<int> *indexes)
{

    for (int var : *indexes) {
        if(var == curIndex)
            return true;
    }

    return false;
}

Player* Mafia::dayChoice(QList<Player *> *players, int myIndex, QList<QPair<int, int> > *lastVote)
{
    Player* choice = nullptr;
    QString text = "";

    int rnd = QRandomGenerator::global()->bounded(1, 100);

    //Мафия выбирает случайного
    if (rnd % 3 == 0)
    {
        int indexForReturn;
        do
        {
            indexForReturn = QRandomGenerator::global()->bounded(0, players->count());
        }while(indexForReturn == myIndex);
        choice = players->at(indexForReturn);
    }
    else {
        //Формируем список без мафий
        QList<Player*> list;
        for (auto curPlayer : *players) {
            if (curPlayer->getRole() != this->whatIsTheRole())
                list << curPlayer;
        }

        //Выбор самого худшего по доверию
        int minTrust = 11;
        for (auto var : list) {
            if(var->info.alive == true && players->at(myIndex)->info.trust[var->getIndex()] < minTrust)
            {
                minTrust = players->at(myIndex)->info.trust[var->getIndex()];
                choice = var;
            }

        }
    }



    text += "\n" + players->at(myIndex)->info.name + " проголосовал против " + choice->info.name;
    players->at(myIndex)->reason = text;

    QPair<int,int> pair(myIndex, choice->getIndex());
    *lastVote << pair;
    return choice;
}
