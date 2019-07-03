#include "detective.h"
#include "QRandomGenerator"

Detective::Detective()
{
    this->incognito = new QPixmap("img/image.png");
    this->alive = new QPixmap("img/detective.png");
    this->dead = new QPixmap("img/deaddetective.png");
    this->check = false;
}

Detective::~Detective()
{
    delete this;
}

QString Detective::whatIsTheRole()
{
    return "Детектив";
}

void Detective::setNotCheckedPlayers(QList<Player *> *players, int index)
{
    this->notCheckedPlayers.clear();
    for (int i = 0; i < players->count(); i++) {
        if(players->at(i)->getIndex() != index)
            this->notCheckedPlayers << players->at(i);
    }
}

Player* Detective::nightChoice(QList<Player *> *players, int index, QList<QPair<int,int>> *lastVote)
{
    //Детектив выбирает одного игрока и проверяет его.
    //Мирный или мафия. Мирный - ничего не делает. Мафия - садит за решетку


    //Выбираем самого красноречивого
    //Самого худшего по доверию
    //Самого лучшего по хитрости
    //Самого неволнующегося

    Player* choice = nullptr;
    QString text = "\n";


    if(!check)
    {
        setNotCheckedPlayers(players, index);
        check = true;
    }

    QList<Player*> copy;
    //Удалить из списка непросмотренных тех, кто не жив
    for (auto var: notCheckedPlayers) {
        if(var->info.alive == true)
            copy << var;
    }

    Player* first, *second, *third, *fourth;
    first = second = third = fourth = nullptr;


    //Выбор самого красноречивого
    int maxOratory = -1;
    for (auto curPlayer : copy) {
        if(curPlayer->info.oratory > maxOratory)
        {
            maxOratory = curPlayer->info.oratory;
            first = curPlayer;
        }
    }

    //Выбор самого худшего по доверию
    int minTrust = 11;
    for (auto curPlayer : copy) {
        if(players->at(index)->info.trust[curPlayer->getIndex()] < minTrust)
        {
            minTrust = players->at(index)->info.trust[curPlayer->getIndex()];
            second = curPlayer;
        }
    }

    //Выбор самого хитрого
    int maxCunning = -1;
    for (auto curPlayer : copy) {
        if(curPlayer->info.cunning > maxCunning)
        {
            maxCunning = curPlayer->info.cunning;
            third = curPlayer;
        }
    }

    //Выбор самого не волнующегося
    int minExcitement = 11;
    for (auto curPlayer : copy) {
        if(curPlayer->info.excitement < minExcitement)
        {
            minExcitement = curPlayer->info.excitement;
            fourth = curPlayer;
        }
    }


    int rndIndex = QRandomGenerator::global()->bounded(1, 100);
    // 1-25: first
    // 26-50: second
    // 51-75: third
    //76-100: fourth

    if (rndIndex < 26)
    {
        text += "Детектив решил проверить самого красноречивого - ";
        choice = first;
    }
    else if (rndIndex < 51) {
        text += "Детектив решил проверить того, кому не очень то и доверяет - ";
        choice = second;
    }
    else if (rndIndex < 76) {
        text += "Детектив решил проверить самого хитрого - ";
        choice = third;
    }
    else {
        text += "Детектив решил самого волнующегося - ";
        choice = fourth;
    }

    text += choice->info.name;

    players->at(index)->reason = text;
    this->notCheckedPlayers.removeOne(choice);
    return choice;
}

Player* Detective::dayChoice(QList<Player *> *players, int myIndex, QList<QPair<int, int> > *lastVote)
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
