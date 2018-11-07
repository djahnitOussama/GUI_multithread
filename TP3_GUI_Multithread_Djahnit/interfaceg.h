#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "temoinetat.h"


struct MESSAGE          // pour transmission vers encodeur
{
    int index;
    QString ligne;
};


class InterfaceG : public QMainWindow
{
    Q_OBJECT

    // les widgets de la fenetre
    QWidget * central;
    QTextEdit* fenetreSaisies;
    QTextEdit* fenetreEncodages;
    QTextEdit* fenetreEvenements;
    QLineEdit * ligne;
    QHBoxLayout * layout1;
    QToolBar * toolbar;

    // menus et toolbar
    QMenu* menuF;
    QAction * actionQuitter;
    QAction * actionFinSaisie;
    QAction * actionReloadFiles;
    TemoinEtat *temoinSaisies;
    TemoinEtat *temoinEncode;

    // chronomètre
    QTime startTime1;


public:
    InterfaceG(QWidget *parent = 0);

    void initSaisies();
    void TraiteSaisie();
    void finSaisie();
    void reStart();

signals:
    void nouvelleLigne(MESSAGE message);

public slots:
    void TransmetLigne(MESSAGE message);
    void afficherJournaux();

    // les fonctions de gestion de l'encodage
public:
    void EncodeLigne(MESSAGE message);
    void initEncodages();
    void threadEncodagePermanent();

};

 void EcritJournal(const char * nomJournal, const QString &texte, bool flagAppend=true);

 int IdThread();


#endif // MAINWINDOW_H
