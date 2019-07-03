#include "mainwindow.h"
#include "game.h"
#include "mafia.h"
#include "detective.h"
#include "civilian.h"
#include "doctor.h"
#include "courtesan.h"
#include "player.h"
#include <QPair>
#include <QVector>
#include "qgroupbox.h"
#include "qlabel.h"

Game::Game()
{
}

Game::~Game()
{
    delete this;
}

Game* Game::currentGame = nullptr;

Game* Game::getInstance()
{
    if (currentGame == nullptr)
        currentGame = new Game();
    return currentGame;
}

//Создать игру
void Game::createGame(int numberOfPlayers)
{
    numPlayers = numberOfPlayers;
    numMafia = numberOfPlayers / 3;
    numDoctors = numDetectives = numCourtesans = 1;
    if (numberOfPlayers == 12) numMafia--;
    if (numMafia == 1)  numCourtesans = 0;

    numCivilians = numPlayers - numMafia - numDoctors - numDetectives - numCourtesans;

    //Новое число игроков в игре
    currentGame->numPlayers = numberOfPlayers;

    //Сколько нужно добавить/удалить игроков
    numberOfPlayers -= this->currentGame->GetPlayers()->count();

    if (numberOfPlayers > 0)
        for(int i = 0; i < numberOfPlayers; i++)
            currentGame->addPlayer();
    else
    {
        if (Players.isEmpty())
            return;
        for(int i = 0; i > numberOfPlayers; i--)
            Players.pop_back();
    }
}

QList<Player*> *Game::GetPlayers()
{
    return &(this->Players);    //Получить ссылку на список игроков
}

Game::Game(int numPlayers){
    this->numPlayers = numPlayers;
    this->step = 1;
    this->GameOver = false;
    this->lastVote.clear();
}

//Добавить игрока
void Game::addPlayer()
{
    this->Players.push_back(new Player());
}

//Удалить всех игроков
void Game::deleteAllPlayers()
{
    int num = Players.count();
    for(int i = 0; i < num; i++)
        Players[i]->~Player();

    for(int i = 0; i < num; i++)
        Players.removeAt(i);
}

//Раздать карты
void Game::distributeCards()
{
    QList<Player*> queue;       //Временная "очередь" для выдачи карточек

    //Очищаем списки текущих игроков
    this->courtesan = nullptr;
    this->doctor = nullptr;
    this->detective = nullptr;
    this->mafias.clear();

    for (Player* var : Players)
        queue << var;

        for (int i = 0; i < numMafia; i++)
        {
            Mafia *role = new Mafia();
            int rnd = random.global()->bounded(0, queue.count());
            queue.at(rnd)->setRole(role);
            this->mafias << queue.at(rnd);
            queue.removeAt(rnd);
        }

        for (int i = 0; i < numCivilians; i++)
        {
            Civilian *role = new Civilian();
            int rnd = qrand() % queue.count();
            queue.at(rnd)->setRole(role);
            queue.removeAt(rnd);
        }

        for (int i = 0; i < numDetectives; i++)
        {

            Detective *role = new Detective();
            int rnd = qrand() % queue.count();
            queue.at(rnd)->setRole(role);
            this->detective = queue.at(rnd);
            queue.removeAt(rnd);
        }
        for (int i = 0; i < numDoctors; i++)
        {
            Doctor *role = new Doctor();
            int rnd = random.global()->bounded(0, queue.count());
            queue.at(rnd)->setRole(role);
            this->doctor = queue.at(rnd);
            queue.removeAt(rnd);
        }
        for (int i = 0; i < numCourtesans; i++)
        {
            Courtesan *role = new Courtesan();
            queue.at(0)->setRole(role);
            this->courtesan = queue.at(0);
            queue.removeAt(0);
        }
}

void Game::updateGameInfo()
{
    int count = 0, courtesans_ = 0, doctors_ = 0, civilians_ = 0, detectives_ = 0, mafias_ = 0;
    for (auto var : Players) {
        if(var->info.alive)
        {
            count++;
            if(var->getRole() == "Мафия")           mafias_++;
            else if (var->getRole() == "Мирный")    civilians_++;
            else if (var->getRole() == "Доктор")    doctors_++;
            else if (var->getRole() == "Детектив")  detectives_++;
            else if (var->getRole() == "Куртизанка")courtesans_++;
        }
    }
    currentGame->numMafia = mafias_;
    currentGame->numCivilians = civilians_;
    currentGame->numPlayers = count;
    currentGame->numDoctors = doctors_;
    currentGame->numCourtesans = courtesans_;
    currentGame->numDetectives = detectives_;
}

bool Game::isGameOver()
{
    return GameOver;
}

//Установить состояние игры
void Game::setGameOver(bool status)
{
    GameOver = status;
}


//Начать игру
void Game::startGame()
{
    setGameOver(false);
    distributeCards();

    for (auto var : Players)
    {
        var->setIncognitoImage();
        var->info.isWakeUp = false;
        var->info.alive = true;
    }
    this->updateGameInfo();
    makeTrustList();

    this->step = 1;
}

void Game::setSpeachLabel(QLabel *label)
{
    this->speachLabel = label;
}

void Game::setLogEdit(QTextEdit *log)
{
    this->log = log;
}

QTextEdit* Game::getLogEdit()
{
    return this->log;
}


void Game::setAliveImages()
{
    for (Player* var: Players) {
        if(var->info.alive == true)
            var->setAliveImage();
    }
}

void Game::updateTrust()
{
    if (!lastVote.isEmpty())
        //Для каждого игрока просмотр последнего голосования
        for (Player* curPlayer : Players) {
            //За кого голосовали?
            int myVote = 0, myIndex = curPlayer->getIndex();
            for(auto vote : lastVote)
                if(vote.first == myIndex) { myVote = vote.second; break; }
            for (auto vote : lastVote) {
                if (vote.second == myVote && vote.first != myIndex) curPlayer->info.trust[vote.first] += 2; //Увеличиваем доверие, если совпал голос
                if (vote.second == myIndex) curPlayer->info.trust[vote.first] -= 2; //Уменьшаем, если проголосовали против меня
                if (curPlayer->info.trust[vote.first] > 10) curPlayer->info.trust[vote.first] = 10;
            }

        }
}


void Game::makeTrustList()
{
    //Заполняем список доверия
    for (int i = 0; i < Players.count(); i++) {
        Players[i]->info.trust.clear();
        int curLvl = 0;
        for(int j = 0; j < Players.count(); j++)
            Players[i]->info.trust << curLvl;
    }

    //Определяем доверие
    for (int i = 0; i < Players.count(); i++) {
        for (int j = 0; j < Players.count(); j++)
        {
        //К самому себе доверие 10/10
        if (i == j)
        {
            Players[i]->info.trust[i] = 10;
            continue;
        }
        //Другого игрока оцениваем по красноречию и волнению
        int currentTrust = QRandomGenerator::global()->bounded(0,5);;

        if(Players[i]->info.sex == Players[j]->info.sex) currentTrust++;
        else currentTrust += QRandomGenerator::global()->bounded(-1, 1);

        if(Players[j]->info.oratory > 5) currentTrust++;
        if(Players[j]->info.oratory > 7) currentTrust++;
        if(Players[j]->info.oratory < 3) currentTrust--;
        if(Players[j]->info.age - Players[i]->info.age < 3 || Players[i]->info.age - Players[j]->info.age < 3)
            currentTrust++;
        else currentTrust--;

        if(Players[j]->info.excitement > 2) currentTrust--;

        if(currentTrust < 1) currentTrust = 1;
        if(currentTrust > 10) currentTrust = 10;

        //совпадает пол +1, не совпадает = +1 или -1 или +0
        //красноречие больше 5: +1, больше 7: +1,  меньше 3: -1
        //разница в возрасте не более 2 лет +1
        //Волнение больше 2: -1

        Players[i]->info.trust[j] = currentTrust;
        }
    }
}

void Game::makeStep()
{
    switch (step) {
    case 1:
        //Усыпление всех игроков
        for (int i = 0; i < Players.count(); i++)
        {
            Player* current = Players.at(i);
            current->info.isWakeUp = false;
        }

        this->log->append("\n\n\n..........................Начата новая игра......................");

        // приветствие
        greeting();
        // мы смотрим, если куртизанок нет -значит переходим сразу к мафии
        if (this->numCourtesans > 0) step = 2; else step = 5;
        break;

    case 2:
       WakeUp(courtesan);
       step = 3;
       break;
    case 3:
       courtesan_2();
       step = 4;
       break;
    case 4:
       FallAsleep(courtesan);
       step++;
       break;
    case 5:
      //мафия-1
      mafia_1();
      step++;
      break;
    case 6:
       mafia_2();
       step++;
       break;
    case 7:
       mafia_3();
       if(doctor->info.alive) step++;
       else if(detective->info.alive) step = 11;
       else step = 14;
       break;
    case 8:
        WakeUp(doctor);
        step++;
        break;
    case 9:
        doctor_2();
        step++;
        break;
    case 10:
        FallAsleep(doctor);
        if(detective->info.alive == true) step++;   else step = 14;
        break;
    case 11:
        WakeUp(detective);
        step++;
        break;
    case 12:
        detective_2();
        step++;
        break;
    case 13:
        FallAsleep(detective);
        step++;
        break;
    case 14:
        day_1();
        if(!checkGameOver())
        step++;
        break;
    case 15:
        day_2();
        if (!checkGameOver())
            step++;
        break;
    case 16:
        day_3();
        break;
    default: break;
    }
}

void Game::greeting()
 {
    speachLabel->setText("Давно не виделись! Пока тебя не было мы сделали симулятор Мафии.\n "
                         "Мы автоматизировали игру, чтобы ты не утруждался(-лась).\nПросто наблюдай со стороны. \n"
                         "Итак, наступает ночь... Все засыпают...");
 }

void Game::WakeUp(Player *player)
{
    QString text = "Просыпается ";
    text += player->getRole() + "и делает свой выбор";
    player->info.isWakeUp = true;
    player->setAliveImage();
}

void Game::FallAsleep(Player *player)
{
    QString text = player->getRole();
    text += "засыпает";
    player->info.isWakeUp = true;
    player->setIncognitoImage();
}




void Game::courtesan_2()
{
    QString text = "Она сделала свой выбор. (Им оказался ";

    this->courtesansChoice = this->courtesan->getCard()->nightChoice(currentGame->GetPlayers(), this->courtesan->getIndex(), &lastVote);

    text +=  this->courtesansChoice->info.name + ")";
    currentGame->log->append(courtesan->reason);
    speachLabel->setText(text);
}

void Game::mafia_1()
{
    speachLabel->setText("Просыпается мафия...");

    //Для списка мафий
    for (int i = 0; i < currentGame->mafias.count(); i++) {
        if(mafias.at(i)->info.alive)
        {
            this->mafias.at(i)->info.isWakeUp = true;
            this->mafias.at(i)->setAliveImage();
        }
    }
}

void Game::mafia_2()
{
    QString text = "Мафия сделала свой выбор...\n";

    QList<QPair<int,int>> votes;

    votes.clear();
    //Для списка мафий
    for (int i = 0; i < currentGame->mafias.count(); i++) {
        Player *curMafia = mafias.at(i);
        if(curMafia->info.alive)
        {
            Player* curVote = curMafia->getCard()->nightChoice(&Players, this->mafias.at(i)->getIndex(), &lastVote);
            QPair<int, int> vote(curMafia->getIndex(), curVote->getIndex());
            votes << vote;
            currentGame->log->append(curMafia->reason);

        }
    }

    if (votes.count() > 1)
        this->mafiasChoice = combiningVotesVafia(&votes);
    else
        this->mafiasChoice = Players.at(votes.at(0).second);

    text += mafiasChoice->info.name;
    speachLabel->setText(text);

}

Player* Game::combiningVotesVafia(QList<QPair<int,int>> *votes)
{
    Player* choice = nullptr;
    QString text = "\nМафия посовещалась и решила выбрать ";
    int indexForReturn = 0;

    //Если мафий меньше 3, то выбираем случайно.

    if(votes->count() > 0 && votes->count() < 3)
    {
       int i = QRandomGenerator::global()->bounded(1,100);
        if(i % 2 == 0)
            i = 0;
        else i = 1;


        indexForReturn = votes->at(i).first;
        for (int i = 0; i < votes->count();i++) {
            if(indexForReturn == votes->at(i).first)
            {
                text += currentGame->GetPlayers()->at(votes->at(i).second)->info.name;
                currentGame->log->append(text);
                return currentGame->GetPlayers()->at(votes->at(i).second);
            }
        }

    }
    else
    {
        //Случайный выбор: довериться красноречивой мафии или той, что доверяю
        int rnd = QRandomGenerator::global()->bounded(1, 100);


        if(rnd % 3 == 0)
        {
            //Доверяем красноречивой мафии
            int indexOfBestMafia = 0, maxOratory = -1;
            //Выберем самого красноречивого среди живых мафий
            for (auto curMaf : mafias) {
                if(curMaf->info.oratory > maxOratory && curMaf->info.alive == true)
                {
                    maxOratory = curMaf->info.oratory;
                    indexOfBestMafia = curMaf->getIndex();
                }
            }
            indexForReturn = indexOfBestMafia;


        }
        else
        {
            //Той, что доверяю
            QList<QPair<int, int>> trust;

            //Сделаем список пар. Кто кому доверяет больше.
            for (auto curMaf_i : mafias)
            {
                int maxTrust = -1;
                int indexOfTrust;
                for (auto curMaf_j : mafias)
                {
                    if(curMaf_i != curMaf_j && curMaf_i->info.trust[curMaf_j->getIndex()] > maxTrust)
                    {
                        maxTrust = curMaf_i->info.trust[curMaf_j->getIndex()];
                        indexOfTrust = curMaf_j->getIndex();
                    }
                }

                trust << *new QPair<int,int>(curMaf_i->getIndex(), indexOfTrust);
            }

            //Выбираем самый часто встречаемый индекс в curTrust.second
            QVector<int> count(Players.count(), 0);
            for (auto curTrust : trust)
                count[curTrust.second]++;

            int max = -1;
            for (int i = 0; i < count.count(); i++)
                if(count[i] > max)
                {
                    max = count[i];
                    indexForReturn = i;
                }
        }
    }


        for (auto var : *votes) {
            if(var.first == indexForReturn)
            {
                text += Players.at(var.second)->info.name;
                currentGame->log->append(text);
                choice = Players.at(var.second);
                return choice;
            }
        }
         return choice;
}

void Game::mafia_3()
{
    speachLabel->setText("Мафия засыпает...");
    //Для списка мафий
    for (int i = 0; i < currentGame->mafias.count(); i++)
        if(mafias.at(i)->info.alive)
        {
            this->mafias.at(i)->info.isWakeUp = false;
            this->mafias.at(i)->setIncognitoImage();
        }
}

void Game::doctor_2()
{
    QString text = "Он сделал свой выбор - ";
    this->doctorsChoice = this->doctor->getCard()->nightChoice(currentGame->GetPlayers(), this->doctor->getIndex(), &lastVote);
    text +=  this->doctorsChoice->info.name;

    currentGame->log->append(doctor->reason);
    speachLabel->setText(text);
}

void Game::detective_2()
{
    QString text = "Он сделал свой выбор - ";
    this->detectivesChoice = this->detective->getCard()->nightChoice(currentGame->GetPlayers(), this->detective->getIndex(), &lastVote);
    text +=  this->detectivesChoice->info.name;
    currentGame->log->append(detective->reason);
    speachLabel->setText(text);
}

void Game::day_1()
{
    QString text = "Просыпаются все, кроме: ";
    //Если куртизанка выбрала того, кого выбрала мафия - выбор мафии не применяется
    //Если доктор выбрал того, кого выбрала мафия - выбор мафии не применяется
    //Если детектив выбрал мафию
    //Варианты:
    //Мафия была одна - выбор мафии не применяется.
    //Мафий было больше одной - выбор мафии применяется
    //В любом случае мафия выбывает
    bool courtisanSafe = false, courtesanStun = false;
    bool doctorSafe = false;
    bool detectiveSafe = false;
    bool jail = false;
    bool mafiaKill = true;

    if (courtesan != nullptr)
    {
        if(mafiasChoice == courtesansChoice && courtesan->info.alive)   { mafiaKill = false; courtisanSafe = true; }
        if (numMafia == 1 && courtesan->info.alive && courtesansChoice->getRole() == "Мафия") { mafiaKill = false; courtesanStun = true; }
    }
    if(mafiasChoice == doctorsChoice && doctor->info.alive)      { mafiaKill = false; doctorSafe = true; }

    if(detectivesChoice->getRole() == "Мафия" && detective->info.alive)
    {
        jail = true;
        if(currentGame->numMafia == 1)
        {
            mafiaKill = false;
            detectiveSafe = true;
        }
    }

    log->append("\nЧто произошло ночью:");
    QString textLog;
    if(mafiaKill)
    {

        textLog +="\nМафия убила " + mafiasChoice->info.name;
        text += "\n" + mafiasChoice->info.name;
        mafiasChoice->setDeadImage();
        mafiasChoice->info.alive = false;
    }

    if(jail)
    {
        if(detectiveSafe)
            textLog += "\nДетектив отправил за решетку последнюю мафию - " + detectivesChoice->info.name + "\n      - и не дал ей совершить задуманное";
        else
        {
            textLog += "\nДетектив отправил за решетку мафию - " + detectivesChoice->info.name;
            text += "\n" + detectivesChoice->info.name;
        }

        detectivesChoice->setDeadImage();
        detectivesChoice->info.alive = false;

    }

    if(doctorSafe)       textLog += "\nДоктор спас " + doctorsChoice->info.name;

    if (courtisanSafe)   textLog += "\nКуртизанка провела ночь с - " + courtesansChoice->info.name + ", чем спасла ему жизнь";

    if (courtesanStun)   textLog += "\nКуртизанка оглушила единственную мафию - " + courtesansChoice->info.name + ". Мафия не совершила затеянного.";

    speachLabel->setText(text);
    updateGameInfo();

    log->append(textLog);
}

void Game::day_2()
{
    for (auto var : Players) {
        if(var->info.alive)
            var->info.isWakeUp = true;
    }

    if(!isGameOver())
    {

    QString text = "Этап дневного голосования...";
    log->append("\n\nДневное голосование");
    currentGame->lastVote.clear();

    for (auto var : Players) {
        if (var->info.alive)
        {
            var->getCard()->dayChoice(currentGame->GetPlayers(), var->getIndex(), &lastVote);
            log->append(var->reason);
        }
    }
    //тут объединение голосов
    Player* kick = combiningVotesDay();
    text += "\nПо итогам голосования выгоняем " + kick->info.name;
    speachLabel->setText(text);
    kick->setDeadImage();
    kick->info.alive = false;
    }

    updateGameInfo();
    updateTrust();
}

Player* Game::combiningVotesDay()
{
    Player* choice = nullptr;

    //Подсчитаем число голосов
    QVector<int> count(Players.count() + 1, 0);
    for (auto var : lastVote)
        count[var.second]++;

    int max = -1;
    //Находим максимум и возвращаем этого игрока
    for (int i = 0; i < count.count(); i++)
    {
        if (count[i] > max)
        {
            max = count[i];
            choice = Players.at(i);
        }
    }

    QString text = "\n........................На голосовании было принято выгнать ";
    text += choice->info.name;
    log->append(text);
    return choice;
}

void Game::day_3()
{
    speachLabel->setText("Город засыпает...");
    if(GameOver == false)
    {
        if (courtesan == nullptr)
            step = 5;
        else
             if(courtesan->info.alive) step = 2; else step = 5;
    }
}

bool Game::checkGameOver()
{
    if(numMafia == 0)
    {
        speachLabel->setText("Победа горожан");
        log->append("\nВся мафия была выгнана из города. Победа горожан.\n\n");
        GameOver = true;
        return GameOver;
    }
    if (numPlayers - numMafia < 2) {
        GameOver = true;
        speachLabel->setText("Победа мафии");
        log->append("\nПобеда мафии\n\n");
        return GameOver;
    }
    return false;
}
