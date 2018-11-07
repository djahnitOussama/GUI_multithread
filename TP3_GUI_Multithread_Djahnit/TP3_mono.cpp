// TP3_mono  Monothread    -  TP 2016

#define _CRT_SECURE_NO_WARNINGS
#define _WIN32_WINNT  0x0600 // Minimum platform is Windows Vista

#include <string>
#include <iostream>
#include <fstream>
#include <locale>
#include <queue>
using namespace std;
#include <windows.h>


//	Définition des paramètres du programme
// -------------------------------------------------------------------------------------

const char nomFichierA[] = "..\\TextesSaisis.txt";			// création du fichier dans le dossier Multitache_TP3 pour tous les projets
const char nomFichierB[] = "..\\TextesValides.txt";
const double coefDuree = 250;

const DWORD masqueAffinite = 0x1;
					//valeurs de masque =  1 ==> ordonnancement des threads sur un seul processeur 
					//					   5 ==> exécution des threads sur 2 processeurs,
					//					   FF ==> exécution des threads sur tous les processeurs,


// types
// ---------------------------------------------------------------------------------------
struct MESSAGE
{
	int index;
	string ligne;
};


// Variables globales
// -------------------------------------------------------------------------------------
MESSAGE gMessage;					// pour version mémoire partagée
queue <MESSAGE> fileMessages;		// pour version file de messages


// 
//	la fonction de traitement : vérifie, corrige et enregiste le texte dans un fichier
// -------------------------------------------------------------------------------------
void Validation(MESSAGE & message)
{

	// Initialisation pour première exécution
	static int compteurValide = 1;
	if (compteurValide == 1)								// première exécution de la fonction
	{
		ofstream fTextesValides(nomFichierB);				// ouverture du fichier, efface contenu précédent
		fTextesValides << "** début des sorties **" << endl;	// écriture de l'entête
	}

	// vérification simulée
	//		par un temps de calcul proportionnel à la taille de la chaine
	//		durée environ (nbre_caract x coefDuree) ms

	int nbOpBase = 40000;									// fixé pour que l'unité soit environ 1ms
	double nbOp = nbOpBase * message.ligne.size()*coefDuree;
	for (int i = 0; i < nbOp; i++)
	{
		double val = 0.34;									// valeur aléatoire, sans importance
		double a = sin((double)val*nbOp / nbOpBase);		// opération sans importance
	}

	// écriture dans le fichier de sortie
	ofstream fTextesValides(nomFichierB, ios::app);			// ouverture du fichier
	fTextesValides << compteurValide << "_" << message.index << ": " << message.ligne << endl;

	compteurValide++;
}


//	point d'entrée du programme
// -------------------------------------------------------------------------------------

void  main()
{
	// Initialisations 
	SetProcessAffinityMask(GetCurrentProcess(), masqueAffinite); // Mode monoprocesseur, biprocesseur ou multiprocesseur	
	locale::global(locale("fr-FR"));

	// Affichages sur la console
	SetConsoleTitle("TP3: test applications multitaches");
	cout << "saisissez les lignes de texte à encoder" << endl << endl;
	cout << "      -- retour chariot pour mettre fin au programme --" << endl << endl << endl;

	// Ouverture du fichier en écriture
	ofstream fTextesSaisis(nomFichierA);
	fTextesSaisis << "** début de la saisie ** " << endl;

	// variables locales
	string sLigne;
	char szTexte[512];
	int iteration = 1;	
	MESSAGE localMessage;

	// Boucle de travail : saisies clavier et encodage
	while (1)												// boucle permanente, sortie si ligne vide
	{
		getline(cin, sLigne);								// saisie clavier (avec attente fin de ligne)
		if (sLigne.length() == 0)							// ligne vide, considérée comme fin de programme
			break;

		OemToChar(sLigne.c_str(), szTexte);					// conversion pour gérer les lettres accentuées
		fTextesSaisis << iteration << ": " << szTexte << endl;	// copie dans le journal des saisies

		// on remplit les champs du message
		localMessage.index = iteration;
		localMessage.ligne = szTexte;

		Validation(localMessage);							// appel (synchrone) de la fonction d'encodage
		iteration++;
	}

}




