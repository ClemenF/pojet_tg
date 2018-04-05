#ifndef GRMAN_H_INCLUDED
#define GRMAN_H_INCLUDED

/// Graphic Manager is a collection of functions and globals
/// that acts as a glue between your C++ app and Allegro
/// Call grman::init(); once at startup

#include <allegro.h>

#include <string>


#include "grman_couleurs.h"
#include "coords.h"

namespace grman
{



/// Constantes

/// Variables globales : visibles partout o� grman.h est inclus

// La couleur de fond effective utilis�e
extern int page_color;

// Le buffer sur lequel tout est dessin�
extern BITMAP *page;
//le buffer de popup
extern BITMAP *popup;

// La frame associ�e pour les widgets
extern Frame page_frame;

// Gestion des widget : avec quel widget la souris interagit ?
class Widget;
extern Widget *gui_over;
extern Widget *gui_last_over;
extern Widget *gui_focus;
extern Widget *gui_leave;


/// Les globales suivantes n�cessitent un appel � rafraichir_clavier_souris
/// en d�but de boucle d'interaction pour �tre mises � jour � chaque tour de boucle

// La key_last appuy�e depuis le dernier tour de boucle
// '\0' si aucun bouton appuy� au dernier tour de boucle
extern char key_last;

extern Coords mouse_pos;


// Le clic souris depuis le dernier tour de boucle
// s'utilise comme mouse_b mais contrairement � mouse_b
// n'est valable que pendant un tour de boucle (pas de r�p�tition)
// mouse_click&1 pour clic gauche, mouse_click&2 pour clic droit
extern int mouse_click;

// Le rel�chement du clic souris depuis le dernier tour de boucle
// d�tecte le passage de l'�tat enfonc� � l'�tat rel�ch�
// n'est valable que pendant un tour de boucle (pas de r�p�tition)
// mouse_unclick&1 pour clic gauche, mouse_unclick&2 pour clic droit
extern int mouse_unclick;

// M�me principe que key mais d�tecte les transitions (press�<->non press�)
// valable pendant un seul tour de boucle (pas de r�p�tition)
// exemple : if (key_unpress[KEY_RIGHT]) printf("touche droite relachee !\n");
extern int key_press[KEY_MAX];
extern int key_unpress[KEY_MAX];

// D�placement relatif de la souris depuis le dernier tour de boucle
extern int mouse_click_x;
extern int mouse_click_y;

extern int mouse_move_x;
extern int mouse_move_y;

extern int mouse_click;

/// Gestion des ressources image (fichiers images et BITMAP charg�es)
unsigned get_picture_nb(std::string name);
BITMAP *get_picture(std::string pic_name);
void show_picture(BITMAP *dest, std::string file_name, int x, int y, unsigned idx=0);
void set_pictures_path(std::string path_name);

/// A appeler une fois en fin de boucle de jeu
void mettre_a_jour();

/// Lancement et fermeture services Allegro

// A appeler une fois et une seule en d�but de main (apr�s les d�clarations)
void init();

//lance un popup avec buffer fixe
void  init_popup();
void buf_afficher_popup();
void fermer_popup();
// Pour lib�rer le buffer et fermer tous les services
// pas indispensable : la fin du programme ferme et lib�re tout automatiquement
void fermer_allegro();


/// Buffer et sorties graphiques

// Efface le buffer (utilise page_color)
// A appeler une fois dans la boucle d'interaction
// avant de redessiner le dessin (et �ventuellement
// autre chose apr�s)
void buf_effacer_page();

// Affiche la page effectivement � l'�cran
// A appeler une fois dans la boucle d'interaction
// A LA FIN JUSTE AVANT rest(10);
void buf_afficher_page();



/// Entr�es clavier/souris

// un appel � rafraichir_clavier_souris doit �tre fait
// en d�but de boucle d'interaction pour mettre � jour les globales
void rafraichir_clavier_souris();


/// Auxiliaires : compl�ments aux fonctions graphiques allegro

void thick_line(BITMAP *bmp, int x1, int y1, int x2, int y2, int thickness, int color);

}

/************************************************
            A CORRIGER
*************************************************/

/// Cette inclusion en fin de header parce que widget d�pend de grman
/// Ceci est �vitable (et � �viter) en re-factorisant le projet et les d�pendance...
#include "widget.h"


#endif // GRMAN_H_INCLUDED
