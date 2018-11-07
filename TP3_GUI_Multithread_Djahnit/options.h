#ifndef OPTIONS_H
#define OPTIONS_H


// Paramètres de l'application
// -------------------------------------------------------------------------------------
const int modeApplication=4;        // sélecteur pour fixer modalité : valeurs prédéfinies
                                    //  0 : thread unique (T_U)
                                    //  1 : un thread par ligne (T_pL)
                                    //  2 : 1 thread interface, 1 thread encodeur permanent (BT_1)
                                    //  3 : BT_1, votre version
                                    //  4 : idem avec buffer circulaire (BT_Buffer)

const int coefDuree = 200;        // paramètre fixe la durée de l'encodeur par caractères


const int masqueAffinite = 0x1;
                    //valeurs de masque =  1 ==> ordonnancement des threads sur un seul processeur
                    //					   5 ==> exécution des threads sur 2 processeurs,
                    //					   FF ==> exécution des threads sur tous les processeurs,
                    //   remarque à vérifier en fonction de votre machine

const int N=5;      // taille du buffer circulaire


#endif // OPTIONS_H
