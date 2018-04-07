#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED


/**************************************************************
    Ici sont proposées 3 classes fondamentales
            Vertex (=Sommet)
            Edge (=Arête ou Arc)
            Graph (=Graphe)

    Les arêtes et les sommets et le graphe qu'ils constituent
    "travaillent" étroitement ensemble : pour cette raison les
    Vertex et Edge se déclarent amis (friend) de Graph pour que
    ce dernier puisse librement accéder aux membres (y compris
    protected ou private) de Vertex et Edge.

    Ces Classes peuvent êtres complétées. Il est également possible
    de les dériver mais il est malheureusement assez difficile
    de dériver le "triplet" des 3 classes en maintenant des relations
    cohérentes ( rechercher "c++ class covariance" et "c++ parallel inheritance"
    pour commencer .. ). Il est donc sans doute préférable, si possible,
    de "customiser" ces classes de base directement, sans héritage.

    Le modèle proposé permet de représenter un graphe orienté éventuellement
    pondéré, les arcs portent une ou des informations supplémentaire(s).
    Les relations/navigations Arcs -> Sommets et Sommets -> Arcs se font
    dans les 2 sens et utilisent des INDICES et NON PAS DES ADRESSES (pointeurs)
    de telle sorte que la topologie du graphe puisse être assez facilement
    lue et écrite en fichier, et bénéficie d'une bonne lisibilité en cas de
bugs...

    Chaque arc possède 2 attributs principaux (en plus de son éventuelle
pondération)
        -> m_from (indice du sommet de départ de l'arc )
        -> m_to (indice du sommet d'arrivée de l'arc )

    Chaque sommet possède 2 liste d'arcs (en plus de ses attributs "internes",
marquages...)
        -> m_in (liste des indices des arcs arrivant au sommet : accès aux
prédécesseurs)
        -> m_out (liste des indices des arcs partant du sommet : accès aux
successeurs)

    Cependant le problème des indices (par rapport aux pointeurs) et qu'en cas
    de destruction d'une entité (un arc et/ou un sommet sont enlevés du graphe)
alors :

    - Soit il faut reprendre toute la numérotation pour "boucher le trou"
      (par exemple on a supprimé le sommet n°4, le sommet n°5 devient le 4, 6
devient 5 etc...)
      ce qui pose des problèmes de stabilité et de cohérence, et une difficulté
à ré-introduire
      le(s) même(s) élément supprimé (au même indice)

    - Soit on admet que la numérotation des sommets et arcs n'est pas contigue,
càd qu'il
      peut y avoir des "trous" : sommets 0 1 5 7 8, pas de sommets 2 ni 3 ni 4
ni 6. La numérotation
      est stable mais on ne peut plus utiliser un simple vecteur pour ranger la
liste de tous
      les arcs et tous les sommets aux indices correspondants, on peut utiliser
une map
      qui associe un objet arc ou sommet à des indices arbitraires (pas
forcément contigus)

    C'est cette 2ème approche qui est proposée ici : dans la classe graphe vous
trouverez
        -> map<int, Edge>   m_edges
        -> map<int, Vertex> m_vertices    (le pluriel de vertex est vertices)

    Il faudra être attentif au fait que par rapport à un simple vecteur, le
parcours des éléments
    ne pourra PAS se faire avec un simple for (int i=0; i<m_edges.size(); ++i)
...m_edges[i]...
    et que les parcours à itérateur ne donneront pas directement des Edge ou des
Vertex
    mais des pairs, l'objet d'intérêt se trouvant dans "second" ("first"
contenant l'indice)
                for (auto &it = m_edges.begin(); it!=m_edges.end(); ++it)
...it->second...
    ou bien     for (auto &e : m_edges) ...e.second...

    Il n'est pas obligatoire d'utiliser ces classes pour le projet, vous pouvez
faire les votres

    Au niveau de l'interface, on dissocie une classe interface associée à chaque
classe fondamentale
    de telle sorte qu'il soit possible de travailler avec des graphes non
représentés à l'écran
    Imaginons par exemple qu'on doive générer 1000 permutations de graphes pour
tester des
    combinaisons, on ne souhaite pas représenter graphiquement ces 1000 graphes,
et les
    interfaces pèsent lourd en ressource, avec cette organisation on est libre
de réserver ou
    pas une interface de présentation associée aux datas (découplage
données/interface)

***********************************************************************************************/

#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "grman/grman.h"

/***************************************************
                    VERTEX
****************************************************/

class VertexInterface
{
        // Les (methodes des) classes amies pourront accéder
        // directement aux attributs (y compris privés)
        friend class Vertex;
        friend class EdgeInterface;
        friend class Graph;

    private:
        /// Les widgets de l'interface. N'oubliez pas qu'il ne suffit pas de déclarer
        /// ici un widget pour qu'il apparaeisse, il faut aussi le mettre en place et
        /// le paramétrer ( voir l'implémentation du constructeur dans le .cpp )

        // La boite qui contient toute l'interface d'un sommet
        grman::WidgetBox m_top_box;

        // Un slider de visualisation/modification de la valeur du sommet
        grman::WidgetVSlider m_slider_value;

        // Un label de visualisation de la valeur du sommet
        grman::WidgetText m_label_value;

        // Une image de "remplissage"
        grman::WidgetImage m_img;

        // Un label indiquant l'index du sommet
        grman::WidgetText m_label_idx;

        // Une boite pour le label précédent
        grman::WidgetText m_box_label_idx;

    public:
        // Le constructeur met en place les éléments de l'interface
        // voir l'implémentation dans le .cpp
        VertexInterface( int idx, int x, int y, std::string pic_name = "",
                         int pic_idx = 0);
};

class Vertex
{
        // Les (methodes des) classes amies pourront accéder
        // directement aux attributs (y compris privés)
        friend class Graph;
        friend class VertexInterface;
        friend class Edge;
        friend class EdgeInterface;

    private:
        /// liste des indices des arcs arrivant au sommet : accès aux prédécesseurs
        std::vector<int> m_in;

        /// liste des indices des arcs partant du sommet : accès aux successeurs
        std::vector<int> m_out;

        /// un exemple de donnée associée à l'arc, on peut en ajouter d'autres...
        double m_value;
        std::string m_name;

        /// le POINTEUR sur l'interface associée, nullptr -> pas d'interface
        std::shared_ptr<VertexInterface> m_interface = nullptr;


        /// variable pour algo dynamique de population :
        int m_N_t; // population � l'instant actuel
        int m_N_t_1; // population � l'instant actuel

        float m_r; // rythme de croissance (valeur fixe)
        int m_K; // capacit� de portage ( = population maximum )

        // Docu shared_ptr : https://msdn.microsoft.com/fr-fr/library/hh279669.aspx
        // La ligne précédente est en gros équivalent à la ligne suivante :
        // VertexInterface * m_interface = nullptr;

    public:
        /// Les constructeurs sont à compléter selon vos besoin...
        /// Ici on ne donne qu'un seul constructeur qui peut utiliser une interface

        Vertex (double value=0, float r=0.01, int Nt=1,int Nt1=1, VertexInterface *interface=nullptr, std::string name="") :
            m_value(value), m_interface(interface), m_r(r), m_N_t(Nt),m_N_t_1(Nt1), m_name(name) {  }


        /// Vertex étant géré par Graph ce sera la méthode update de graph qui
        /// appellera
        /// le pre_update et post_update de Vertex (pas directement la boucle de jeu)
        /// Voir l'implémentation Graph::update dans le .cpp
        void pre_update();
        void post_update();
};

/***************************************************
                    EDGE
****************************************************/

class EdgeInterface
{
        // Les (methodes des) classes amies pourront accéder
        // directement aux attributs (y compris privés)
        friend class Edge;
        friend class Graph;

    private:
        /// Les widgets de l'interface. N'oubliez pas qu'il ne suffit pas de déclarer
        /// ici un widget pour qu'il apparaisse, il faut aussi le mettre en place et
        /// le paramétrer ( voir l'implémentation du constructeur dans le .cpp )

        // Le WidgetEdge qui "contient" toute l'interface d'un arc
        grman::WidgetEdge m_top_edge;

        // Une boite pour englober les widgets de réglage associés
        grman::WidgetBox m_box_edge;

        // Un slider de visualisation/modification du poids valeur de l'arc
        grman::WidgetVSlider m_slider_weight;

        // Un label de visualisation du poids de l'arc
        grman::WidgetText m_label_weight;

    public:
        // Le constructeur met en place les éléments de l'interface
        // voir l'implémentation dans le .cpp
        EdgeInterface( Vertex &from, Vertex &to );
};

class Edge
{
        // Les (methodes des) classes amies pourront accéder
        // directement aux attributs (y compris privés)
        friend class Graph;
        friend class EdgeInterface;

    private:
        /// indice du sommet de départ de l'arc
        int m_from;

        /// indice du sommet d'arrivée de l'arc
        int m_to;

        /// un exemple de donnée associée à l'arc, on peut en ajouter d'autres...
        double m_weight;

        /// le POINTEUR sur l'interface associée, nullptr -> pas d'interface
        std::shared_ptr<EdgeInterface> m_interface = nullptr;


        /// variable pour algo dynamique de population
        int m_coef; // valeur fixe du coefficient de contribution de la population du sommet de d�part � la valeur K du sommet d'arriv�.

    public:
        /// Les constructeurs sont à compléter selon vos besoin...
        /// Ici on ne donne qu'un seul constructeur qui peut utiliser une interface
        Edge( double weight = 0, EdgeInterface *interface = nullptr )
            : m_weight( weight ), m_interface( interface ) {}

        /// Edge étant géré par Graph ce sera la méthode update de graph qui appellera
        /// le pre_update et post_update de Edge (pas directement la boucle de jeu)
        /// Voir l'implémentation Graph::update dans le .cpp
        void pre_update();
        void post_update();
};

/***************************************************
                    GRAPH
****************************************************/

class GraphInterface
{
        friend class Graph;

    private:
        /// Les widgets de l'interface. N'oubliez pas qu'il ne suffit pas de déclarer
        /// ici un widget pour qu'il apparaisse, il faut aussi le mettre en place et
        /// le paramétrer ( voir l'implémentation du constructeur dans le .cpp )

        /// La boite qui contient toute l'interface d'un graphe
        grman::WidgetBox m_top_box;

        /// Dans cette boite seront ajoutés les (interfaces des) sommets et des
        /// arcs...
        grman::WidgetBox m_main_box;

        /// Dans cette boite seront ajoutés des boutons de contrôle etc...
        grman::WidgetBox m_tool_box;


        /// Les boutons de manipulation du graphe
        /// pour les sommets
        grman::WidgetText m_text_bt_ajouter_vertex;
        grman::WidgetButton m_bt_ajouter_vertex;
        grman::WidgetText m_text_bt_supprimer_vertex;
        grman::WidgetButton m_bt_supprimer_vertex;

        grman::WidgetButton m_bt_dynamique;
        grman::WidgetText m_text_m_bt_dynamique;

        ///pour les arcs
        grman::WidgetText m_text_bt_ajouter_edge;
        grman::WidgetButton m_bt_ajouter_edge;
        grman::WidgetText m_text_bt_supprimer_edge;
        grman::WidgetButton m_bt_supprimer_edge;

        //std::vector<grman::WidgetButton*> m_vec_bt_ajouter_vertex;



    public:
        // Le constructeur met en place les éléments de l'interface
        // voir l'implémentation dans le .cpp
        GraphInterface( int x, int y, int w, int h );
};

class Graph
{
    private:
        /// La "liste" des arêtes
        std::map<int, Edge> m_edges;

        /// La liste des sommets
        std::map<int, Vertex> m_vertices;

        /// le POINTEUR sur l'interface associée, nullptr -> pas d'interface
        std::shared_ptr<GraphInterface> m_interface = nullptr;

        grman::WidgetTimer m_Timer;



        int m_ordre;
        int m_nb_arete;

        //std::vector<std::vector<int>> m_matrice;

        int m_numero_graphe;                     // Num graph (0,1,2)
        std::vector<std::vector<int>> adjacence; // matrice d'adjacence


    public:
        /// Les constructeurs sont à compléter selon vos besoin...
        /// Ici on ne donne qu'un seul constructeur qui peut utiliser une interface
        Graph( GraphInterface *interface = nullptr ) : m_interface( interface ) {}

        Graph( GraphInterface *interface, int num )
            : m_interface( interface ), m_numero_graphe( num ) {}


        void add_interfaced_vertex(int idx, double value, int x, int y, std::string pic_name="",float r=0,int Nt=0, std::string name="",int pic_idx=0);
        void add_interfaced_edge(int idx, int vert1, int vert2, double weight=0);


        /// Méthode spéciale qui construit un graphe arbitraire (démo)
        /// Voir implémentation dans le .cpp
        /// Cette méthode est à enlever et remplacer par un système
        /// de chargement de fichiers par exemple.
        void make_example();

        /// algo composantes connexes
        std::vector<std::vector<int>> toutesLesComposanteFortementConnexe(); // algo qui tire toutes les
        // composantes connexes
        std::vector<int> uneComposantesFortementConnexes( int s ); // algo qui tire les omposantes connexes de un seul sommet
        std::vector<std::vector<int>> transpose();//retourne la matrice transposée

        //algo du graphe reduit (lié au composantes fortement connexe)
        void graphe_reduit();
        void spring_model(std::vector<std::vector<int>> tabadjacence); //recup un tab d'adjacence
        std::vector<std::vector<int>> groupes_fortements_connexes_to_matrice(std::vector<std::vector<int>> groupes_fortements_connexes );
      void draw_graph_reduit_on_bmp(std::vector<std::vector<int>> pos,std::vector<std::vector<int>> tabadjacence);
        /// Méthode pour creer la matrice d'adjacence
        void matrice_adjacent();

        /// Méthode de sauvegarde de graph dans un fichier texte
        void graphe_sauvegarde();

        /// Méthode de chargement de graph d'un fichier texte
        void graphe_chargement();


        /// M�thode pour effacer un sommet
        void remove_vertex(int index);

        /// M�thode pour effacer un arc (index de l'arc � supprimer)
        void remove_edge(int eidx);

        ///les m�thodes des boutons de gestion du graph :
        void bouton_ajouter_vertex();
        void bouton_supprimer_vertex();
        void bouton_ajouter_edge();
        void bouton_supprimer_edge();
        void boutondynamiquedechainealimentaire();

        /// m�thode de la dynamique de population
        void dynamique_population();
        int calcul_K(int);
        int predation(int);
        void fctreproduction(int num_vertex_donne);
        void miseajoutarete();

        /// La m�thode update � appeler dans la boucle de jeu pour les graphes avec interface

        void update();
};

#endif // GRAPH_H_INCLUDED
