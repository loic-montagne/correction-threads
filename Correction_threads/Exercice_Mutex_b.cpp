/*
#include <iostream>
#include <fstream>
#include <thread>

using namespace std;

ofstream file("match.txt");

void writeA()
{
    for (int i = 0; i < 200; i++) {
        file << 'A';
        file.flush();
    }
}

void writeB()
{
    for (int i = 0; i < 200; i++) {
        file << 'B';
        file.flush();
    }
}

int main()
{
    cout << "Debut de l'ecriture du fichier de maniere parallele." << endl;

    thread tA(writeA);
    thread tB(writeB);

    tA.join();
    tB.join();

    file.close();
    cout << "Fin de l'ecriture du fichier de maniere parallele." << endl;
}
*/