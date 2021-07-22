#include <iostream>
#include "Tomasulo.hpp"
#include "execute.hpp"

using namespace std;

unsigned int reg[32], pc;
unsigned char mem[500000];

unsigned char hex(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    else return ch - 'A' + 10;
}

void get_input() {
    unsigned pos = 0;
    char ch;
    cin >> ch;
    while (ch != EOF) {
        if (ch == '@') cin >> hex >> pos;
        else {
            unsigned char x = hex(ch) * 16;
            cin >> ch;
            x += hex(ch);
            mem[pos++] = x;
        }
        ch = cin.get();
        while (ch == ' ' || ch == '\n') ch = cin.get();
    }
}

int main() {
    for (int i = 0; i < 10; ++i)
        fetchQueue_prev.push(i, 2 * i);
    for (int i = 0; i < 10; ++i) {
        cout << fetchQueue_prev.front().inst << ' ' << fetchQueue_prev.front().pc << endl;
        fetchQueue_prev.pop();
    }
}
