/*
#include <iostream>
#include <fstream>

std::ofstream file("match.txt");

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
    std::cout << "Debut de l'ecriture du fichier de maniere sequentielle." << std::endl;

    writeA();
    writeB();

    file.close();
    std::cout << "Fin de l'ecriture du fichier de maniere sequentielle." << std::endl;
}
*/