#include "grman/grman.h"
#include <iostream>
#include "graph.h"


int menu ()
{
    std::string quitter;
    int a=0;
    BITMAP *image;
    image=grman::get_picture("ocean.jpg");
    BITMAP *iceberg;
    iceberg=grman::get_picture("iceberg.jpg"); //http://wallpaperswide.com/seagulls_on_an_iceberg-wallpapers.html
    if (!image)
    {
        allegro_message("pas pu trouver/charger mon_image.bmp");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    while (quitter!="suite" && quitter!="quitter")
    {
        blit(image,screen,0,0, 0, 0, SCREEN_W, SCREEN_H);
        if(mouse_b & mouse_x>320 & mouse_x<715 & mouse_y>240 & mouse_y<360)
        {
            std::cout << "mouse_x " << mouse_x<< std::endl;
            std::cout << "mouse_y " << mouse_y<< std::endl;
            quitter="suite";

        }
        if(mouse_b & mouse_x>320 & mouse_x<715 & mouse_y>440 & mouse_y<550)
        {
            std::cout << "mouse_x " << mouse_x<< std::endl;
            std::cout << "mouse_y " << mouse_y<< std::endl;
            quitter="quitter";
            a=5;
        }
    }
    if(quitter == "suite")
    {
        while (quitter!="chaine1" && quitter!="chaine2" && quitter!="chaine3")
        {
            blit(iceberg,screen,0,0, 0, 0, SCREEN_W, SCREEN_H);
            if(mouse_b & mouse_x>330 & mouse_x<705 & mouse_y>180 & mouse_y<285)
            {
                quitter="chaine1";
                a=1;
            }
            if(mouse_b & mouse_x>330 & mouse_x<705 & mouse_y>344 & mouse_y<444)
            {
                quitter="chaine2";
                a=0;

            }
            if(mouse_b & mouse_x>330 & mouse_x<705 & mouse_y>502 & mouse_y<615)
            {
                quitter="chaine3";
                a=2;
            }
        }
    }

    return a;
}


int main()
{
    /// A appeler en 1er avant d'instancier des objets graphiques etc...
    grman::init();
    //grman::init_popup();

    /// Le nom du r�pertoire o� se trouvent les images � charger
    grman::set_pictures_path("pics");


    int num_graphe = menu();

    if(num_graphe != 5) // Si a = 5 alors on a choisi "quitter"
    {
    /// Un exemple de graphe
    Graph g(nullptr,num_graphe);
    g.graphe_chargement();
    //g.dynamique_population();
    //g.make_example();
    //g.matrice_adjacent();
    //g.test_remove_edge(2);
    //g.remove_vertex(2);
    //g.make_example();    while ( menu== true )


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
    }





    grman::fermer_allegro();

    return 0;
}
END_OF_MAIN();
