
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "Semaphore.h"

using namespace std;

// Valeurs constantes utilisées dans le programme
// ----------------------------------------------------------------------------------------------------------------

// Nombre de chaînes de caractères accessible à un rédacteur
#define STRINGS_COUNT 6

// Taille du tableau partagé
#define BUFFER_SIZE 10

// Nombre de secondes que met un rédacteur pour écrire dans le tableau partagé
#define WRITING_TIME 1


// Prototypes des fonctions disponibles dans le programme
// (l'implémentation est faite dans ce fichier)
// ----------------------------------------------------------------------------------------------------------------

void reader(int);
void writer(int);
bool strtoi(char*, int*);
#ifdef _WIN32
void sleep(DWORD);
void usleep(DWORD);
#endif


// Variables globales
// ----------------------------------------------------------------------------------------------------------------

// Tableau des chaînes de caractères que peut écrire un rédacteur dans le tableau partagé
char writer_strings[STRINGS_COUNT][BUFFER_SIZE] = { "Hello", "Hola", "Quetal", "Salut", "Bonjour", "Bonsoir" };

// Tableau partagé : 
//    - contient une chaîne écrite par un rédacteur
//    - est lu par tous les lecteurs
char buffer[BUFFER_SIZE];

// Nombre de rédacteurs en cours d'exécution
int writers_count = 0;

// Nombre de lecteurs en cours d'exécution
int readers_count = 0;


// Fonctions
// ----------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    cout << "[TP] Demarrage du programme, lecture des arguments recus en cous..." << endl;
    cout << "[TP] Identification des threads :" << endl;
    cout << "     [TP] => Thread principal" << endl;
    cout << "     [RX] => Thread lecteur X" << endl;
    cout << "     [WX] => Thread redacteur X" << endl;
    cout << endl;
    cout << "[TP] Lecture des arguments reçus en cous..." << endl;
    int readers_max, writers_max;
    if (argc != 3 || 
        !strtoi(argv[1], &readers_max) ||
        !strtoi(argv[2], &writers_max)) {
        cout << "[TP] Arguments non valides, le programme s'arrete !" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "[TP] Arguments corrects, " << readers_max << " lecteur(s) et " << writers_max << " redacteur(s) vont etre crees." << endl;
    cout << "[TP] Initialisation du programme en cours..." << endl;

    // Initialisation du tableau partagé : tout à 0
    memset(buffer, BUFFER_SIZE * sizeof(char), 0);

    // Initialisation du générateur aleatoire
    srand(time(NULL));

    // Initialisation du tableau de threads
    const int threads_count = readers_max + writers_max;
    thread* t_threads = new thread[threads_count];
    int indice = 0;

    cout << "[TP] Initialisation du programme terminee." << endl;
    cout << "[TP] Creation des lecteurs et des redacteurs de maniere aleatoire..." << endl;

    while (readers_count < readers_max || 
           writers_count < writers_max)
    {
        // Génération d'un nombre aléatoire compris entre 0 et 1
        bool even = rand() % 2;

        // 1 lecteur si le nombre obtenu aléatoirement est pair
        if (readers_count < readers_max && even)
        {
            readers_count++;
            t_threads[indice] = thread(reader, readers_count);
            indice++;
        } 
        // 1 rédacteur si le nombre obtenu aléatoirement est impair
        else if (writers_count < writers_max && !even)
        {
            writers_count++;
            t_threads[indice] = thread(writer, writers_count);
            indice++;
        }

        // Attente aléatoire entre 100 et 2000 millisecondes avant de créer prochain threadle 
        usleep((rand() % 1901) + 100);
    }

    cout << "[TP] Creation des lecteurs et des redacteurs terminee." << endl;
    cout << "[TP] Attente de la fin d'execution de tous les lecteurs / redacteurs..." << endl;

    for (int i = 0; i < threads_count; i++)
        t_threads[i].join();

    cout << "[TP] Tous les lecteurs / redacteurs ont fini leur traitement." << endl;
    cout << "[TP] Liberation de la memoire..." << endl;

    delete[] t_threads;

    cout << "[TP] Memoire liberee, le programme va s'arreter." << endl;

    return(EXIT_SUCCESS);
}

void reader(int id)
{
    cout << "  [R" << id << "] Je viens de demarrer." << endl;
    cout << "  [R" << id << "] Je commence a lire..." << endl;
    cout << "  [R" << id << "] La chaine est : " << buffer << endl;
    cout << "  [R" << id << "] J'ai fini de lire." << endl;
    cout << "  [R" << id << "] Je me termine." << endl;
}

void writer(int id)
{
    cout << "  [W" << id << "] Je viens de demarrer." << endl;

    int indice = rand() % STRINGS_COUNT;
    cout << "  [W" << id << "] Je vais ecrire la chaine \"" << writer_strings[indice] << "\" se trouvant a l'indice " << indice << "." << endl;

    cout << "  [W" << id << "] Je commence a ecrire..." << endl;
    for (int i = 0; i < strlen(writer_strings[indice]); i++)
    {
        buffer[i] = writer_strings[indice][i];
        sleep(WRITING_TIME);
    }

    cout << "  [W" << id << "] J'ai fini d'ecrire." << buffer << endl;
    cout << "  [W" << id << "] Je me termine." << endl;
}

bool strtoi(char* value, int* result)
{
    char* p;
    errno = 0;
    long arg = strtol(value, &p, 10);
    if (*p != '\0' || errno != 0) {
        return false;
    }

    if (arg < INT_MIN || arg > INT_MAX) {
        return false;
    }

    *result = arg;
    return true;
}

#ifdef _WIN32
void sleep(DWORD dwMilliseconds)
{
    Sleep(dwMilliseconds * 1000);
}
void usleep(DWORD dwMilliseconds)
{
    Sleep(dwMilliseconds);
}
#endif