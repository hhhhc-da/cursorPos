#include "cursorstack.h"

CursorStack::CursorStack()
{

}

CursorStack::CursorStack(int X,int Y):x(X),y(Y){

}

CursorStack::~CursorStack(){
    deleteCursor();
}
