// TP3_Master AII - 2017_2018

#include "interfaceg.h"
#include <QApplication>
#include <QtWidgets>
#include <thread>
using namespace std;

#include "encodage.h"
#include "options.h"




int main(int argc, char *argv[])
{

    // gestion du masque d'affinité
#ifdef WIN32
    SetProcessAffinityMask(GetCurrentProcess(),masqueAffinite); // Mode monoprocesseur, biprocesseur ou multiprocesseur
#else
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    cpuset.__bits[0]=masqueAffinite;
    sched_setaffinity(0, sizeof(cpuset), (cpu_set_t *)&cpuset);
#endif

    // Initialisation interface graphique
    QApplication a(argc, argv);
    InterfaceG w;
    w.show();

    if (modeApplication >= 2)
    {
        // création d'un thread
        thread t(&InterfaceG::threadEncodagePermanent,&w);
        t.detach();
    }                  // il faut ici détacher le thread, join impossible dans ce contexte

    return a.exec();
}
