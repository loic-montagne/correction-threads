
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
template <typename... T> void print(T...);
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

// Sémaphore pour réserver l'accès au tableau partagé
CSemaphore semBuffer(1);

// Sémaphore pour réserver l'accès à la sortie standard
// Géré dans print()
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

    // Initialisation du tableau partagé : tout à 0
    memset(buffer, 0, BUFFER_SIZE * sizeof(char));

    // Initialisation du générateur aleatoire
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

    // rand n'est pas prévu pour être utilisé avec des threads, 
    // pour obtenir un comportement "correct", 
    // il faut réinitialiser la graine dans chaque thread...
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