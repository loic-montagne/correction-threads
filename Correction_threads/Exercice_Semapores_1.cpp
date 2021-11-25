
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include "Semaphore.h"

using namespace std;

ofstream file("match.txt");
CSemaphore semA(1);
CSemaphore semB(0);

void writeA()
{
    for (int i = 0; i < 200; i++) {
        semA.wait();

        file << "Ping-";
        file.flush();

        semB.notify();
    }
}

void writeB()
{
    for (int i = 0; i < 200; i++) {
        semB.wait();

        file << "Pong-";
        file.flush();

        semA.notify();
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
