#ifndef TEMOINETAT_H
#define TEMOINETAT_H

#include <QIcon>
#include <QAction>

// pour gestion des icones d'état du travail, personnalisées pour l'application

class TemoinEtat : public QAction
{
    Q_OBJECT
    int mode=0;
    int etat=0;
    QIcon icone[3]={QIcon("img/vert.png"),QIcon("img/rouge.png"),QIcon("img/jaune.png")};
    QString texte[3] ;

public:
    TemoinEtat(const char* txt0, const char* txt1, const char *txt2=NULL, QObject *parent=NULL);
    void setMode(int a)     {mode=a;}
    int estOccupe()           {return etat!=0;}

public slots:
    void ChangeEtat(int nouvelEtat, QString texteInfo, int heure);

signals:
    void working(bool);
};

#endif // TEMOINETAT_H
