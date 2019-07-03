#include "player.h"
#include "civilian.h"
#include "QRandomGenerator"


int Player::count = 0;

Player::Player()
{
  this->myIndex = count;
  count++;
  info.name = "Player" + QString::number(count);
  info.alive = true;
  info.isWakeUp = true;
  info.sex = QRandomGenerator::global()->bounded(0,1);
  info.age = QRandomGenerator::global()->bounded(18,36);
  info.oratory = QRandomGenerator::global()->bounded(3,8);
  info.cunning = QRandomGenerator::global()->bounded(3,8);
  info.excitement = QRandomGenerator::global()->bounded(1,5);
  this->role = nullptr;
}

Player::~Player()
{
    count--;
    //delete this;
}



void Player::setRole(Card* role)
{
    this->role = role;
}

QString Player::getRole()
{
    if(this->role != nullptr)
        return this->role->whatIsTheRole();
    return "без роли";
}

Card* Player::getCard()
{
    return this->role;
}


QString Player::getStatus()
{
    if(this->info.alive)
    {
        QString status = "В игре ";
        if(this->info.isWakeUp)
            status += "(не спит)";
        else
            status += "(спит)";
        return status;
    }

    return "Выбыл";
}

QString Player::getSex()
{
    if(info.sex)
        return "Жен.";
    return "Муж.";

}

int Player::getIndex()
{
    return myIndex;
}

void Player::setDeadImage()
{
    this->image->setPixmap(*this->role->dead);
}

void Player::setAliveImage()
{
    this->image->setPixmap(*this->role->alive);
}

void Player::setIncognitoImage()
{
    this->image->setPixmap(*this->role->incognito);
}



QString Player::getTrust()
{
    QString text = "";
    if(this->info.trust.count() == 0)
        return "nothing";

    for (int i = 0; i < info.trust.count(); i++) {
        QString name = "\nPlayer";
        name += QString::number(i+1) + " : ";
        text += name + QString::number(info.trust[i]);

    }

    return text;

}

void Player::SetImageLabel(QLabel *image)
{
    this->image = image;
}
