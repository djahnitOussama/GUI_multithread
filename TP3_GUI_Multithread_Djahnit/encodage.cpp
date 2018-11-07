// TP3_Master AII - 2017_2018

#include <QtWidgets>
#include "interfaceg.h"
#include<iostream>
#include <thread>
#include "options.h"
#include<qsemaphore.h>

// Variables globales
// -------------------------------------------------------------------------------------
MESSAGE caseMemoire;					// pour version mémoire partagée
MESSAGE bufferCirculaire[N];
QSemaphore semtrans(1);//mode application=3
QSemaphore semmode4(N);//mode application=4
QSemaphore semencode(0);//semaphore pour l'encodage valable pour les deux modes : 3 et 4

static int Nextindex = 0;

//  ----------------------------------------------------------------------------------
//   TransmetLigne  fait l'intermédiaire entre l'interface graphique et l'encodeur
//   - elle est appelée par l'interface graphique (donc toujours dans le thread principal)
//     pour chaque ligne saisie (1 à la fois)
//   - transmet à l'encodeur avec les modalités déterminées par les options du programme,
//  ----------------------------------------------------------------------------------
void InterfaceG::TransmetLigne(MESSAGE message)
{
    // transmission du message

    if (modeApplication == 0)
        EncodeLigne(message);							// appel (synchrone) de la fonction d'encodage


    if (modeApplication == 1)
    {
        std::thread t(&InterfaceG::EncodeLigne,this,message );
        t.detach();                     // il faut ici détacher le thread, join impossible dans ce contexte

       }


    if (modeApplication == 2)
    {
        caseMemoire=message;                      // transmission par variable partagée
    }

    if (modeApplication==3)
    {
        semtrans.acquire(); //lorsque on lance la premiere fois l'application on passe directement,on ne peut acceder a nouveau jusqu'a ou la copie soit faite
        caseMemoire=message;
        semencode.release();//on libere pour l'encodage

    }

    if (modeApplication==4)
    {


         semmode4.acquire();//a chaque access on decremente le compteur de la semaphore donc on a N access a cette partie critque(sans encodage) ,apres encodage on incremente ce compteur
         bufferCirculaire[Nextindex]=message;//copier le message dans la case memoire correspondante
         semencode.release();//on libere l'access pour l'encodage
         Nextindex++;
         if(Nextindex==N)
             Nextindex=0;


    }

    return;
}


//
// fonction exécutée comme thread secondaire permanent (avec boucle while sans fin)
//      attend un nouveau message
//--------------------------------------------------------------------------------------

void InterfaceG::threadEncodagePermanent()
{
    MESSAGE copieMessage;


    if (modeApplication==2)
    {
        static int lastIndice=0;
        while(1)                        // toujours pret à traiter un nouveau message
        {
            while (lastIndice == caseMemoire.index)
            ;

             lastIndice = caseMemoire.index;
            copieMessage = caseMemoire;
            EncodeLigne(copieMessage);

        }
    }
    if (modeApplication==3)
    {

        while(1)                        // toujours pret à traiter un nouveau message
        {
            semencode.acquire(); //encodage ne peut pas demarrer tant que la saisie n'est pas terminer

            copieMessage = caseMemoire;
            semtrans.release();// apres avoir copier le message on libere la partie de la saisie
            EncodeLigne(copieMessage);

        }
    }
    if (modeApplication==4)
    {
         int nextdata=0;
            while(1)
            {
            semencode.acquire(); //encodage ne peut pas demarrer tant que la saisie n'est pas validé

            copieMessage = bufferCirculaire[nextdata];//on copie le contenu de la case du buffer dans une variable
            semmode4.release();// apres avoir copier le message on incremente le compteur de la semaphore pour augmenter le nombre d' access a la premiere partie critique du code
            EncodeLigne(copieMessage);//appel à la fonction d'encodage
            nextdata++;
            if(nextdata==N)
                nextdata=0;
            }


    }
}



//
//	la fonction de traitement : vérifie, encode et enregiste le texte dans un fichier
//  selon mode choisi, travaille dans le thread principal ou un thread dédié à l'encodage
// -------------------------------------------------------------------------------------

void InterfaceG:: EncodeLigne(MESSAGE message)
{
    static int indiceEncodage = 0;
    indiceEncodage++;
    uint threadId =IdThread();
    // Initialisation pour première exécution
    if (indiceEncodage == 1)								// première exécution de la fonction
        initEncodages();

    // copie des infos
    QString texteCode = message.ligne;
    int indexMessage = message.index;


    // infos début du traitement
    int heure=startTime1.elapsed();
    int valOccupation = -1;
    if (modeApplication==4)
        valOccupation =1;  // il serait encore mieux de mettre le nombre de ligne en attente - voir la méthode available du sémaphore

    QString tx1=QString("Encode -  commence ligne %1_%2          (thread %3)").arg(indiceEncodage).arg(indexMessage).arg(threadId);
     QMetaObject::invokeMethod(temoinEncode,"ChangeEtat",Qt::AutoConnection,Q_ARG(int,valOccupation),Q_ARG(QString,tx1),Q_ARG(int,heure));


    // encodage simulée
    //		par un temps de calcul proportionnel à la taille de la chaine
    //		durée environ (nbre_caract x coefDuree) ms
    int nbOpBase = 40000;									// fixé pour que l'unité soit environ 1ms
    double nbOp = nbOpBase * message.ligne.size()*coefDuree;
    for (int i = 0; i < nbOp; i++)
    {
        double val = 0.34;									// valeur aléatoire, sans importance
        double a = sin((double)val*nbOp / nbOpBase);		// opération sans importance
        val=a;
    }

    // infos fin du traitement
    heure=startTime1.elapsed();
    QString tx2=QString("Encode -  fin ligne %1_%2               (thread %3)").arg(indiceEncodage).arg(indexMessage).arg(threadId);
    QMetaObject::invokeMethod(temoinEncode,"ChangeEtat",Qt::AutoConnection,Q_ARG(int,0),Q_ARG(QString,tx2),Q_ARG(int,heure));

    // mise à jour du journal des encodages et affichage
    QString tx = QString(+"%1_%2 : %3\n").arg(indiceEncodage).arg(indexMessage).arg(texteCode);      // préfixe avec indice de saisie

    EcritJournal("./JournalEncode.txt",tx );
}

void InterfaceG::initEncodages()
{
    QString txt=QString("** encodages   - version test:%1    - coefDuree : %2 **\n\n").arg(modeApplication).arg(coefDuree) ;
    EcritJournal("./JournalEncode.txt",txt,false);
}





