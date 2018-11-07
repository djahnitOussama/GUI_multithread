// TP3_Master AII - 2017_2018

#include <QtWidgets>
#include "interfaceg.h"
#include "temoinetat.h"



// Constructeur --> initialisation de l'interface graphique

InterfaceG::InterfaceG(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("TP3: test applications multitaches interactives");
    central=new QWidget;
    setCentralWidget(central);
    setGeometry(300,100,1400,700);

    // Création Menu
    menuF = menuBar()->addMenu("&Fichier");
    actionFinSaisie  =menuF->addAction(QIcon("img/Enregistre.png"),"Fin saisie",this,&InterfaceG::finSaisie,QKeySequence("Ctrl+F"));
    actionQuitter=menuF->addAction(QIcon("img/exit.png"),"Quitter",qApp,&QApplication::quit,QKeySequence("Ctrl+Q"));
    actionReloadFiles=menuF->addAction(QIcon("img/sync.ico"),"Recharger fichiers",this, &InterfaceG::afficherJournaux,QKeySequence("Ctrl+R"));

    // création toolbar
    toolbar=addToolBar("saisies");
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ligne=new QLineEdit(central);
    toolbar->addWidget(ligne);
    temoinSaisies=new TemoinEtat("saisie \ndisponible", "saisie\nbloquée","");
    temoinEncode=new TemoinEtat("encodeur\ndisponible","","encodeur \ntravaille");
    toolbar->addAction(actionFinSaisie);
    toolbar->addAction(actionReloadFiles);
    toolbar->addAction(actionQuitter);
    toolbar->addSeparator();
    toolbar->addAction(temoinSaisies);
    toolbar->addAction(temoinEncode);
    QFont font=ligne->font();
    font.setPointSize(font.pointSize()+2);
    ligne->setFont(font);
    ligne->setPlaceholderText("Saisir une ligne, 'entrée' pour valider");

    // création des zones d'affichage
    fenetreSaisies=new QTextEdit (central);
    fenetreSaisies->setReadOnly(true);
    fenetreEncodages=new QTextEdit ("<i style='color:darkblue'>Affichage des encodages ... après validation 'fin des saisies'</i><br>", central);
    fenetreEncodages->setAcceptRichText(true);
    fenetreEncodages->setReadOnly(true);
    fenetreEvenements=new QTextEdit("<i style='color:darkblue'>Affichage des événements ... après validation 'fin des saisies'</i><br>",central);
    fenetreEvenements->setReadOnly(true);

    // disposition des zones d'affichages
    layout1=new QHBoxLayout;
    central->setLayout(layout1);
    layout1->addWidget(fenetreSaisies);
    layout1->addWidget(fenetreEncodages);
    layout1->addWidget(fenetreEvenements);

    // paramètres d'affichage
     fenetreSaisies->setFontPointSize(11);
     fenetreEncodages->setFontPointSize(11);
     QFont font2("monospace",9);
     font2.setStyleHint(QFont::Monospace);
     fenetreEvenements->setFont(font2);
     fenetreEvenements->setMinimumWidth(550);

     // connexion des signaux
     connect(ligne,&QLineEdit::returnPressed, this,&InterfaceG::TraiteSaisie);
     connect(this, &InterfaceG::nouvelleLigne,this,&InterfaceG::TransmetLigne);

     // démarrage
    initSaisies();
}

 // initialisation du journal des saisies et fenetre d'affichage
 void InterfaceG::initSaisies()
 {
     startTime1.start();

     // réinitialisation des journaux (remise à 0 des fichiers)
     EcritJournal("./JournalSaisies.txt","** initialisation saisies **\n\n",false);
     EcritJournal("./JournalEvenements.txt","   0 s   : Debut essai\n\n",false);

     fenetreSaisies->setHtml ( "<i style='color:style='color:blue''>(Ctrl+F ou icône 'Fin Saisie' pour terminer)</i><br>** début de la saisie ** <br> " );
     actionQuitter->setDisabled(true);
 }


 // fonction appelée par l'interface graphique après chaque saisie de ligne
 void InterfaceG::TraiteSaisie()
 {
     static int indiceLigne=1;
     uint threadId =IdThread();

     // lecture champ de saisie et réinitialisation
     QString texteLigneSaisie = ligne->text();
     ligne->setText("");     // remise à zéro du champ de saisie
     ligne->setPlaceholderText("attendre");


     // mise à jour du journal des saisies et de l'affichage
     QString tx = QString("%1: %2").arg(indiceLigne).arg(texteLigneSaisie);      // préfixe avec indice de saisie
     fenetreSaisies->append(tx);
     EcritJournal("./JournalSaisies.txt",tx+"\n");
     QString tx2=QString ("Saisie -  validation %1, champ bloqué  (thread %2)").arg(indiceLigne).arg(threadId);
     temoinSaisies->ChangeEtat(1,tx2,startTime1.elapsed());

     // appel fonction interface avec encodeur (appel synchrone)
     MESSAGE message={indiceLigne, texteLigneSaisie};
     emit nouvelleLigne(message);

     // une fois transmis, on libere le champ de saisie
     ligne->setPlaceholderText("disponible");
     QString tx3=QString("Saisie -  champ disponible            (thread %1)").arg(threadId);
     temoinSaisies->ChangeEtat(0,tx3,startTime1.elapsed());

     indiceLigne++;
 }

 void InterfaceG::finSaisie()                // fonction appelée par le bouton 'fin de saisie'
 {

     // mise à jour du journal des saisies
     QString txt("%1 : utilisateur met fin aux saisies\n");
     EcritJournal("./JournalEvenements.txt",txt.arg(startTime1.elapsed()/1000.0,8,'f',3));

     // mise à jour de l'interface
     ligne->setEnabled(false);
     ligne->setText("saisie terminée");
     QPalette palette=ligne->palette();
     palette.setColor(QPalette::Text,"red");
     ligne->setPalette(palette);
     actionQuitter->setDisabled(false);
     actionFinSaisie->setDisabled(true);

     afficherJournaux();
 }

// affichage des journaux sur l'interface graphique
 void InterfaceG::afficherJournaux()
 {
     QFile journalEncodages("./JournalEncode.txt");
     if( !journalEncodages.open(QIODevice::ReadOnly) )
     {
         QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir le fichier: 'JournalEncode.txt'", QMessageBox::Ok );
         return;
     }
     QTextStream flux(&journalEncodages);
     fenetreEncodages->setText(flux.readAll());

     QFile journalEvts("./JournalEvenements.txt");
     if (journalEvts.open(QIODevice::ReadOnly))
     {
         QTextStream f2(&journalEvts);
         fenetreEvenements->setText(f2.readAll());
     }
 }

// écriture du texte dans le journal
 void EcritJournal(const char * nomJournal, const QString& texte, bool flagAppend)
 {
     // plusieurs essais si fichier bloqué en écriture
     for (int nbEssai=0;nbEssai<10; nbEssai++)
     {

         QFile * file = new QFile(nomJournal);
         if  (file->open(flagAppend?QIODevice::Append:QIODevice::WriteOnly) )
         {
             file->write(texte.toLocal8Bit());
             file->close();
             return;
         }
         // si échec on réessaie
         QThread::msleep(10);
     }
     qDebug()<< "EcritJournal : erreur accès "<<nomJournal;
 }

 int IdThread()
 {
#ifdef WIN32
    return (uint) QThread::currentThreadId();
#else
    return (uint)pthread_self()%USHRT_MAX;
#endif
 }
