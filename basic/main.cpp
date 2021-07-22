#include <iostream>
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

unsigned fetch(unsigned pc) {
    unsigned inst = mem[pc] + (mem[pc + 1] << 8) + (mem[pc + 2] << 16) + (mem[pc + 3] << 24);
    return inst;
}



int j = 0;

int main() {
    freopen("tak.data", "r", stdin);
    get_input();
    pc = 0;
    while (true) {
        unsigned inst = fetch(pc);
        if (inst == 267388179) {
            cout << (reg[10] & 255u);
            break;
        } else execute(inst);

//        if (pc < 4096 || j < 50){
//            for (int i = 0; i < 32; ++i)
//                cout << reg[i] << ' ';
//            cout << endl;
//            ++j;
//        } else break;
    }
    return 0;
}
