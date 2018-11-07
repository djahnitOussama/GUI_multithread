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


//	D�finition des param�tres du programme
// -------------------------------------------------------------------------------------

const char nomFichierA[] = "..\\TextesSaisis.txt";			// cr�ation du fichier dans le dossier Multitache_TP3 pour tous les projets
const char nomFichierB[] = "..\\TextesValides.txt";
const double coefDuree = 250;

const DWORD masqueAffinite = 0x1;
					//valeurs de masque =  1 ==> ordonnancement des threads sur un seul processeur 
					//					   5 ==> ex�cution des threads sur 2 processeurs,
					//					   FF ==> ex�cution des threads sur tous les processeurs,


// types
// ---------------------------------------------------------------------------------------
struct MESSAGE
{
	int index;
	string ligne;
};


// Variables globales
// -------------------------------------------------------------------------------------
MESSAGE gMessage;					// pour version m�moire partag�e
queue <MESSAGE> fileMessages;		// pour version file de messages


// 
//	la fonction de traitement : v�rifie, corrige et enregiste le texte dans un fichier
// -------------------------------------------------------------------------------------
void Validation(MESSAGE & message)
{

	// Initialisation pour premi�re ex�cution
	static int compteurValide = 1;
	if (compteurValide == 1)								// premi�re ex�cution de la fonction
	{
		ofstream fTextesValides(nomFichierB);				// ouverture du fichier, efface contenu pr�c�dent
		fTextesValides << "** d�but des sorties **" << endl;	// �criture de l'ent�te
	}

	// v�rification simul�e
	//		par un temps de calcul proportionnel � la taille de la chaine
	//		dur�e environ (nbre_caract x coefDuree) ms

	int nbOpBase = 40000;									// fix� pour que l'unit� soit environ 1ms
	double nbOp = nbOpBase * message.ligne.size()*coefDuree;
	for (int i = 0; i < nbOp; i++)
	{
		double val = 0.34;									// valeur al�atoire, sans importance
		double a = sin((double)val*nbOp / nbOpBase);		// op�ration sans importance
	}

	// �criture dans le fichier de sortie
	ofstream fTextesValides(nomFichierB, ios::app);			// ouverture du fichier
	fTextesValides << compteurValide << "_" << message.index << ": " << message.ligne << endl;

	compteurValide++;
}


//	point d'entr�e du programme
// -------------------------------------------------------------------------------------

void  main()
{
	// Initialisations 
	SetProcessAffinityMask(GetCurrentProcess(), masqueAffinite); // Mode monoprocesseur, biprocesseur ou multiprocesseur	
	locale::global(locale("fr-FR"));

	// Affichages sur la console
	SetConsoleTitle("TP3: test applications multitaches");
	cout << "saisissez les lignes de texte � encoder" << endl << endl;
	cout << "      -- retour chariot pour mettre fin au programme --" << endl << endl << endl;

	// Ouverture du fichier en �criture
	ofstream fTextesSaisis(nomFichierA);
	fTextesSaisis << "** d�but de la saisie ** " << endl;

	// variables locales
	string sLigne;
	char szTexte[512];
	int iteration = 1;	
	MESSAGE localMessage;

	// Boucle de travail : saisies clavier et encodage
	while (1)												// boucle permanente, sortie si ligne vide
	{
		getline(cin, sLigne);								// saisie clavier (avec attente fin de ligne)
		if (sLigne.length() == 0)							// ligne vide, consid�r�e comme fin de programme
			break;

		OemToChar(sLigne.c_str(), szTexte);					// conversion pour g�rer les lettres accentu�es
		fTextesSaisis << iteration << ": " << szTexte << endl;	// copie dans le journal des saisies

		// on remplit les champs du message
		localMessage.index = iteration;
		localMessage.ligne = szTexte;

		Validation(localMessage);							// appel (synchrone) de la fonction d'encodage
		iteration++;
	}

}




