/*
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>

using namespace std;

ofstream file("match.txt");
bool a_turn = true;
mutex m;

void writeA()
{
    for (int i = 0; i < 200; i++) {
        while (!a_turn);

        m.lock();

        file << 'A';
        file.flush();
        a_turn = false;

        m.unlock();
    }
}

void writeB()
{
    for (int i = 0; i < 200; i++) {
        while (a_turn);

        m.lock();

        file << 'B';
        file.flush();
        a_turn = true;

        m.unlock();
    }
}

int main()
{
    cout << "Debut de l'ecriture du fichier de maniere parallele et synchronisee." << endl;

    thread tA(writeA);
    thread tB(writeB);

    tA.join();
    tB.join();

    file.close();
    cout << "Fin de l'ecriture du fichier de maniere parallele et synchronisee." << endl;
}
*/