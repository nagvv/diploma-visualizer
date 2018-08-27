#ifndef DATA_H
#define DATA_H

struct bot
{
    float   posX, posY,
            dirX, dirY,
            radius, v,  //what is v?
            bX, bY,     //what is b?
            clX, clY;   //what is cl?
};

struct data
{
    int N = 0;
    bool isExist = false;
    bot *bots = nullptr;

    ~data()
    {
        if(bots)
            delete[] bots;
    }
};

#endif // DATA_H
