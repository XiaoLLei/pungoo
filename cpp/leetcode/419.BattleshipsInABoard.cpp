/*************************************************************************
	> File Name: 419.BattleshipsInABoard.cpp
	> Author: XiaoLLei
	> Mail: Errant.Xia@gmail.com
	> Created Time: Tue 24 Jan 2017 05:05:25 PM CST
 ************************************************************************/

#include <iostream>
#include <vector>
using namespace std;

class Solution {
    typedef vector< vector<char> >::iterator ROIT;
    typedef vector<char>::iterator COIT;
public:
    int countBattleships(vector< vector<char> >& board) {
        for (ROIT it = board.begin(); it != board.end(); ++it) {
            int flag = 0;
            COIT pre_coit = it->begin();
            for (COIT coit = it->begin(); coit != it->end(); ++coit) {
                cout << *coit << "    ";
            }
            
            cout << endl;
        }
        
        
        int count = 0;
        for (ROIT it = board.begin(); it != board.end(); ++it) {
            int flag = 0;
            COIT pre_coit = it->begin();
            for (COIT coit = it->begin(); coit != it->end(); ++coit) {
                if (*coit == 'x') {
                    if (flag == 0) {
                        flag = 1;
                        pre_coit = coit;
                    } else if (flag == 1) {
                        flag = 2;
                        *pre_coit = '.';
                        *coit = '.';
                        ++count;
                    } else if (flag == 2) {
                        *coit = '.';
                    }
                } else {
                    flag = 0;
                }
            }
        }
        
        ROIT first_row = board.begin();
        int len = first_row->size();
        for (int offset = 0; offset < len; ++offset) {
            int flag = 0;
            for (ROIT it = board.begin(); it != board.end(); ++it) {
                COIT coit = it->begin() + offset;
                if (*coit == 'x') {
                    if (flag == 0) {
                        flag = 1;
                        ++count;
                    } else if (flag == 1) {
                        flag = 2;
                    }
                } else {
                    flag = 0;
                }
            }   
        }
        
        return count;
    }
};

int main() {
    std::vector< std::vector<char> > data;

    std::vector<char> row;
    row.push_back('x');
    row.push_back('.');
    row.push_back('.');
    row.push_back('x');
    data.push_back(row);

    row.clear();
    row.push_back('.');
    row.push_back('.');
    row.push_back('.');
    row.push_back('x');
    data.push_back(row);
    
    row.clear();
    row.push_back('.');
    row.push_back('.');
    row.push_back('.');
    row.push_back('x');
    data.push_back(row);
    
    Solution s;
    cout << s.countBattleships(data) << endl;
    
    return 0;
}
