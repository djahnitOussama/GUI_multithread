// TP3_Master AII - 2017_2018

#include <QLocale>
#include "temoinetat.h"
#include "interfaceg.h"
#include <qdebug.h>

TemoinEtat::TemoinEtat(const char *txt0, const char *txt1, const char *txt2, QObject * parent)
    : QAction(txt0, parent), texte{txt0,txt1,txt2}
{
    setIcon(icone[0]);
}

// pour la gestion des icones témoins, plusieurs modes permettent si besoin
// de tenir de compte de file d'attente

void TemoinEtat::ChangeEtat(int signal, QString texteInfo, int heure)
{
    static int compteur=0;
    int etatPrecedent=(etat!=0);

    if (signal <0)
        mode=2;

    if (mode==2)           // mode 2 avec compteur local
    {
        if (signal==-1)
        {
            compteur++;
            etat=2;
        }
        if (signal==0)
        {
            compteur--;
            if (compteur==0)
            {    etat=0;
                emit working(false);
            }
        }
    }
    if (mode ==1)             // mode 1 compteur transmis par signal
    {
        compteur=signal;
        etat = (compteur > 0);
    }
    if (mode == 0)          // par défaut signal binaire sans compteur
    {
        etat = (signal != 0);
    }

    if (etatPrecedent!=(etat!=0))  // si changement d'état
        emit working(etat !=0);

    QLocale france;
    QString txt("%1 : %2\n");
    EcritJournal("./JournalEvenements.txt",txt.arg(heure/1000.0,8,'f',3).arg(texteInfo));

    // mise à jour des icones témoins
    setIcon(icone[etat]);
    setText(texte[etat]);
    QList<QWidget *> list= associatedWidgets();
    list.at(1)->repaint();
}

