#ifndef CURSORSTACK_H
#define CURSORSTACK_H

#include <string>

using namespace std;

class CursorStack
{
public:
    CursorStack();
    CursorStack(int ,int );
    ~CursorStack();

    inline void deleteCursor(){
        x = 0;
        y = 0;
    }

    inline void writeX(int x){
        this->x = x;
    }

    inline void writeY(int y){
        this->y = y;
    }

    inline int readX(){
        return x;
    }

    inline int readY(){
        return y;
    }

    inline string getString(){
        return string("X pos: ") + to_string(x) + string("\tY pos: ") + to_string(y);
    }
private:
    int x,y;
};

#endif // CURSORSTACK_H
