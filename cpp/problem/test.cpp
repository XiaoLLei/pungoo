#include <iostream>
#include <map>
using namespace std;

int main()
{
    typedef map<int,int> MapIntInt;
    MapIntInt mapX;
    ++mapX[0];
    ++mapX[0];
    ++mapX[0];
    ++mapX[0];
    ++++mapX[1];
    ++++++mapX[2];
    ++++++mapX[3];
    for (MapIntInt::iterator it = mapX.begin(); it != mapX.end(); ++it)
    {
        cout << it-> first << " : " << it->second << endl;
    }
    return 0;
}