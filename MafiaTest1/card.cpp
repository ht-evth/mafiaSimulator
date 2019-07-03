#include "card.h"
#include "QList"
#include "QPixmap"
#include "player.h"

Card::Card()
{
    this->alive = new QPixmap("");
    this->dead = new QPixmap("");
    this->incognito = new QPixmap("");
}

Card::~Card()
{
    delete this;
}



