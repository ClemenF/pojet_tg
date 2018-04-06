#include "grman/grman.h"
#include <iostream>

#include "graph.h"

int main()
{
    /// A appeler en 1er avant d'instancier des objets graphiques etc...
    grman::init();
    //grman::init_popup();

    /// Le nom du r�pertoire o� se trouvent les images � charger
    grman::set_pictures_path("pics");

    /// Un exemple de graphe
    Graph g(nullptr,1);
    g.graphe_chargement();
    //g.dynamique_population();
    //g.make_example();
    //g.matrice_adjacent();
    //g.test_remove_edge(2);
    //g.remove_vertex(2);
    //g.make_example();


    /// Vous gardez la main sur la "boucle de jeu"
    /// ( contrairement � des frameworks plus avanc�s )
    while ( !key[KEY_ESC] )
    {
        /// Il faut appeler les m�thodes d'update des objets qui comportent des widgets
        g.update();

        /// Mise � jour g�n�rale (clavier/souris/buffer etc...)
        grman::mettre_a_jour();
    }


    g.graphe_sauvegarde();

    grman::fermer_allegro();

    return 0;
}
END_OF_MAIN();
