
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <cstdarg>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "Semaphore.h"

using namespace std;

// Valeurs constantes utilis�es dans le programme
// ----------------------------------------------------------------------------------------------------------------

// Nombre de cha�nes de caract�res accessible � un r�dacteur
#define STRINGS_COUNT 6

// Taille du tableau partag�
#define BUFFER_SIZE 10

// Nombre de secondes que met un r�dacteur pour �crire dans le tableau partag�
#define WRITING_TIME 1


// Prototypes des fonctions disponibles dans le programme
// (l'impl�mentation est faite dans ce fichier)
// ----------------------------------------------------------------------------------------------------------------

void reader(int);
void writer(int);
bool strtoi(char*, int*);
template <typename... T> void print(T...);
#ifdef _WIN32
void sleep(DWORD);
void usleep(DWORD);
#endif


// Variables globales
// ----------------------------------------------------------------------------------------------------------------

// Tableau des cha�nes de caract�res que peut �crire un r�dacteur dans le tableau partag�
char writer_strings[STRINGS_COUNT][BUFFER_SIZE] = { "Hello", "Hola", "Quetal", "Salut", "Bonjour", "Bonsoir" };

// Tableau partag� : 
//    - contient une cha�ne �crite par un r�dacteur
//    - est lu par tous les lecteurs
char buffer[BUFFER_SIZE];

// Nombre de r�dacteurs en cours d'ex�cution
int writers_count = 0;

// Nombre de lecteurs en cours d'ex�cution
int readers_count = 0;

// S�maphore pour r�server l'acc�s au tableau partag�
CSemaphore semBuffer(1);

// S�maphore pour r�server l'acc�s � la sortie standard
// G�r� dans print()
CSemaphore semStdOut(1);


// Fonctions
// ----------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    print("[TP] Demarrage du programme, lecture des arguments recus en cous...\n",
          "[TP] Identification des threads :\n",
          "     [TP] => Thread principal\n",
          "     [RX] => Thread lecteur X\n",
          "     [WX] => Thread redacteur X\n",
          "\n",
          "[TP] Lecture des arguments recus en cous...\n");
    int readers_max, writers_max;
    if (argc != 3 ||
        !strtoi(argv[1], &readers_max) ||
        !strtoi(argv[2], &writers_max)) {
        print("[TP] Arguments non valides, le programme s'arrete !\n");
        exit(EXIT_FAILURE);
    }

    print("[TP] Arguments corrects, ", readers_max, " lecteur(s) et ", writers_max, " redacteur(s) vont etre crees.\n",
          "[TP] Initialisation du programme en cours...\n");

    // Initialisation du tableau partag� : tout � 0
    memset(buffer, 0, BUFFER_SIZE * sizeof(char));

    // Initialisation du g�n�rateur aleatoire
    srand(time(NULL));

    // Initialisation du tableau de threads
    const int threads_count = readers_max + writers_max;
    thread* t_threads = new thread[threads_count];
    int indice = 0;

    print("[TP] Initialisation du programme terminee.\n",
          "[TP] Creation des lecteurs et des redacteurs de maniere aleatoire...\n");

    while (readers_count < readers_max ||
           writers_count < writers_max)
    {
        // G�n�ration d'un nombre al�atoire compris entre 0 et 1
        bool even = rand() % 2;

        // 1 lecteur si le nombre obtenu al�atoirement est pair
        if (readers_count < readers_max && even)
        {
            readers_count++;
            t_threads[indice] = thread(reader, readers_count);
            indice++;
        }
        // 1 r�dacteur si le nombre obtenu al�atoirement est impair
        else if (writers_count < writers_max && !even)
        {
            writers_count++;
            t_threads[indice] = thread(writer, writers_count);
            indice++;
        }

        // Attente al�atoire entre 100 et 2000 millisecondes avant de cr�er prochain threadle 
        usleep((rand() % 1901) + 100);
    }

    print("[TP] Creation des lecteurs et des redacteurs terminee.\n",
          "[TP] Attente de la fin d'execution de tous les lecteurs / redacteurs...\n");

    for (int i = 0; i < threads_count; i++)
        t_threads[i].join();

    print("[TP] Tous les lecteurs / redacteurs ont fini leur traitement.\n",
          "[TP] Liberation de la memoire...\n");

    delete[] t_threads;

    print("[TP] Memoire liberee, le programme va s'arreter.\n");

    return(EXIT_SUCCESS);
}

void reader(int id)
{
    print("  [R", id, "] Je viens de demarrer.\n",
          "  [R", id, "] Je demande l'acces au buffer...\n");
    semBuffer.wait();
    print("  [R", id, "] Je commence a lire...\n",
          "  [R", id, "] La chaine est : ", buffer, "\n",
          "  [R", id, "] J'ai fini de lire, je libere l'acces au buffer.\n");
    semBuffer.notify();
    print("  [R", id, "] Je me termine.\n");
}

void writer(int id)
{
    print("  [W", id, "] Je viens de demarrer.\n");

    // rand n'est pas pr�vu pour �tre utilis� avec des threads, 
    // pour obtenir un comportement "correct", 
    // il faut r�initialiser la graine dans chaque thread...
    srand(time(NULL));

    int indice = rand() % STRINGS_COUNT;
    print("  [W", id, "] Je vais ecrire la chaine \"", writer_strings[indice], "\" se trouvant a l'indice ", indice, ".\n",
          "  [W", id, "] Je demande l'acces au buffer...\n");
    semBuffer.wait();

    print("  [W", id, "] Je commence a ecrire...\n");
    for (int i = 0; i < strlen(writer_strings[indice]); i++)
    {
        buffer[i] = writer_strings[indice][i];
        sleep(WRITING_TIME);
    }
    buffer[strlen(writer_strings[indice])] = '\0';

    print("  [W", id, "] J'ai fini d'ecrire, je libere l'acces au buffer.", "\n");
    semBuffer.notify();

    print("  [W", id, "] Je me termine.\n");
}

template <typename... T> void print(T... args)
{
    semStdOut.wait();
    ((cout << args), ...);
    cout.flush();
    semStdOut.notify();
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