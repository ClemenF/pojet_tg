#include "graph.h"
#include <sstream>
#include <string>
#include <cmath>
#include <math.h>
/***************************************************
            DEFINE FOR SPRING MODEL
****************************************************/
#define MIN_X 400 // pour viser un carre au centre de..
#define MAX_X 630 // l'ecran donc des intervalles pour x et y
#define MIN_Y 240
#define MAX_Y 500

#define X 0 // utile pour spring model
#define Y 1 // ce sont les pos dans le vector

#define MAX_X_POPUP 1000 // max x et y du popup
#define MAX_Y_POPUP 700
#define AMORTISSEMENT 0.85 // de base 0.85
#define ATTRACTION 0.001    // de base 0.06
#define O_ATTRACTION 0.001      // de base 200
#define REPULSION 300      // de base 200
#define O_REPULSION 400      // de base 200
#define ITERATION 50
#define O_ITERATION 200
#define RADIUS_FLECHE 6

#define RADIUS_NODE 25 // rayon de sommets pour spring model

#define M_PI 3.14

/***************************************************
                    VERTEX
****************************************************/

/// Le constructeur met en place les �l�ments de l'interface
VertexInterface::VertexInterface( int idx, int x, int y, std::string pic_name,
                                  int pic_idx ) {
    // La boite englobante
    m_top_box.set_pos( x, y );
    m_top_box.set_dim( 130, 100 );
    m_top_box.set_moveable();
    m_top_box.set_bg_color( BLANCBLEU );
    // Le slider de r�glage de valeur
    m_top_box.add_child( m_slider_value );
    m_slider_value.set_range( 0.0, 400.0 ); // Valeurs arbitraires, � adapter...
    m_slider_value.set_dim( 20, 80 );
    m_slider_value.set_gravity_xy( grman::GravityX::Left, grman::GravityY::Up );
    // Label de visualisation de valeur
    m_top_box.add_child( m_label_value );
    m_label_value.set_gravity_xy( grman::GravityX::Left, grman::GravityY::Down );
    // Une illustration...
    if ( pic_name != "" ) {
        m_top_box.add_child( m_img );
        m_img.set_pic_name( pic_name );
        m_img.set_pic_idx( pic_idx );
        m_img.set_gravity_x( grman::GravityX::Right );
    }
    // Label de visualisation d'index du sommet dans une boite
    m_top_box.add_child( m_box_label_idx );
    m_box_label_idx.set_gravity_xy( grman::GravityX::Right, grman::GravityY::Down );
    m_box_label_idx.set_dim( 20, 12 );
    m_box_label_idx.set_bg_color( BLANC );
    m_box_label_idx.add_child( m_label_idx );
    m_label_idx.set_message( std::to_string( idx ) );
}

/// Gestion du Vertex avant l'appel � l'interface
void Vertex::pre_update() {
    if ( !m_interface )
        return;
    /// Copier la valeur locale de la donn�e m_value vers le slider associ�
    m_interface->m_slider_value.set_value( ( int )m_N_t_1 );
    /// Copier la valeur locale de la donn�e de la pop vers le label sous le slider
    m_interface->m_label_value.set_message( std::to_string( ( int )m_N_t_1 ) );
}

/// Gestion du Vertex apr�s l'appel � l'interface
void Vertex::post_update() {
    if ( !m_interface )
        return;
    /// Reprendre la valeur du slider dans la donn�e m_N_t locale
    //m_N_t = m_interface->m_slider_value.get_value();
}

/***************************************************
                    EDGE
****************************************************/

/// Le constructeur met en place les �l�ments de l'interface
EdgeInterface::EdgeInterface( Vertex &from, Vertex &to ) {
    // Le WidgetEdge de l'interface de l'arc
    if ( !( from.m_interface && to.m_interface ) ) {
        std::cerr
                << "Error creating EdgeInterface between vertices having no interface"
                << std::endl;
        throw "Bad EdgeInterface instanciation";
    }
    m_top_edge.attach_from( from.m_interface->m_top_box );
    m_top_edge.attach_to( to.m_interface->m_top_box );
    m_top_edge.reset_arrow_with_bullet();
    // Une boite pour englober les widgets de r�glage associ�s
    m_top_edge.add_child( m_box_edge );
    m_box_edge.set_dim( 24, 60 );
    m_box_edge.set_bg_color( BLANCBLEU );
    // Le slider de r�glage de valeur
    m_box_edge.add_child( m_slider_weight );
    m_slider_weight.set_range( 0.0, 20 ); // Valeurs arbitraires, � adapter...
    m_slider_weight.set_dim( 16, 40 );
    m_slider_weight.set_gravity_y( grman::GravityY::Up );
    // Label de visualisation de valeur
    m_box_edge.add_child( m_label_weight );
    m_label_weight.set_gravity_y( grman::GravityY::Down );
}

/// Gestion du Edge avant l'appel � l'interface
void Edge::pre_update() {
    if ( !m_interface )
        return;
    /// Copier la valeur locale de la donn�e m_weight vers le slider associ�
    m_interface->m_slider_weight.set_value( m_weight );
    /// Copier la valeur locale de la donn�e m_weight vers le label sous le
    /// slider
    m_interface->m_label_weight.set_message( std::to_string( ( int )m_weight ) );
}

/// Gestion du Edge apr�s l'appel � l'interface
void Edge::post_update() {
    if ( !m_interface )
        return;
    /// Reprendre la valeur du slider dans la donn�e m_weight locale
    //m_weight = m_interface->m_slider_weight.get_value();
    /*
      /// Copier la valeur locale de la donn�e de la pop vers le label sous le slider
      m_interface->m_label_value.set_message( std::to_string( (int)m_N_t) );
      /// Copier la valeur locale de la donn�e m_value vers le slider associ�
      m_interface->m_slider_value.set_value((int)m_N_t);
      */
}

/***************************************************
                    GRAPH
****************************************************/


/// Ici le constructeur se contente de pr�parer un cadre d'accueil des
/// �l�ments qui seront ensuite ajout�s lors de la mise ne place du Graphe
GraphInterface::GraphInterface( int x, int y, int w, int h ) {
    m_top_box.set_dim( 1000, 740 );
    m_top_box.set_gravity_xy( grman::GravityX::Right, grman::GravityY::Up );
    m_top_box.add_child( m_tool_box );
    m_tool_box.set_dim( 130, 720 );
    m_tool_box.set_gravity_xy( grman::GravityX::Left, grman::GravityY::Up );
    m_tool_box.set_bg_color( BLANCBLEU );
    m_top_box.add_child( m_main_box );
    m_main_box.set_dim( 870, 720 );
    m_main_box.set_gravity_xy( grman::GravityX::Right, grman::GravityY::Up );
    m_main_box.set_bg_color( BLANCJAUNE );
    int y_bt = 15;
    /// ajout des boutons de gestion du graph.
    /// pour les sommets :
    m_tool_box.add_child( m_text_bt_ajouter_vertex );
    m_text_bt_ajouter_vertex.set_dim( 110, 22 );
    m_text_bt_ajouter_vertex.set_gravity_x( grman::GravityX::Left );
    m_text_bt_ajouter_vertex.set_posy( y_bt );
    m_text_bt_ajouter_vertex.set_bg_color( BLANC );
    m_text_bt_ajouter_vertex.set_message( "Add vertex!" );
    y_bt = y_bt + m_text_bt_ajouter_vertex.get_dimy();
    m_tool_box.add_child( m_bt_ajouter_vertex );
    m_bt_ajouter_vertex.set_dim( 110, 22 );
    m_bt_ajouter_vertex.set_gravity_x( grman::GravityX::Left );
    m_bt_ajouter_vertex.set_posy( y_bt );
    m_bt_ajouter_vertex.set_bg_color( CYANSOMBRE );
    y_bt = y_bt + m_bt_ajouter_vertex.get_dimy() + 10;
    m_tool_box.add_child( m_text_bt_supprimer_vertex );
    m_text_bt_supprimer_vertex.set_dim( 110, 22 );
    m_text_bt_supprimer_vertex.set_gravity_x( grman::GravityX::Left );
    m_text_bt_supprimer_vertex.set_posy( y_bt );
    m_text_bt_supprimer_vertex.set_bg_color( BLANC );
    m_text_bt_supprimer_vertex.set_message( "sup vertex!" );
    y_bt = y_bt + m_text_bt_supprimer_vertex.get_dimy();
    m_tool_box.add_child( m_bt_supprimer_vertex );
    m_bt_supprimer_vertex.set_dim( 110, 22 );
    m_bt_supprimer_vertex.set_gravity_x( grman::GravityX::Left );
    m_bt_supprimer_vertex.set_posy( y_bt );
    m_bt_supprimer_vertex.set_bg_color( CYANSOMBRE );
    y_bt = y_bt + m_bt_supprimer_vertex.get_dimy() + 10;
    y += 30;
    /// pour les arcs :
    m_tool_box.add_child( m_text_bt_ajouter_edge );
    m_text_bt_ajouter_edge.set_dim( 110, 22 );
    m_text_bt_ajouter_edge.set_gravity_x( grman::GravityX::Left );
    m_text_bt_ajouter_edge.set_posy( y_bt );
    m_text_bt_ajouter_edge.set_bg_color( BLANC );
    m_text_bt_ajouter_edge.set_message( "add edge !" );
    y_bt = y_bt + m_text_bt_ajouter_edge.get_dimy();
    m_tool_box.add_child( m_bt_ajouter_edge );
    m_bt_ajouter_edge.set_dim( 110, 22 );
    m_bt_ajouter_edge.set_gravity_x( grman::GravityX::Left );
    m_bt_ajouter_edge.set_posy( y_bt );
    m_bt_ajouter_edge.set_bg_color( CYANSOMBRE );
    y_bt = y_bt + m_bt_ajouter_edge.get_dimy() + 10;
    m_tool_box.add_child( m_text_bt_supprimer_edge );
    m_text_bt_supprimer_edge.set_dim( 110, 22 );
    m_text_bt_supprimer_edge.set_gravity_x( grman::GravityX::Left );
    m_text_bt_supprimer_edge.set_posy( y_bt );
    m_text_bt_supprimer_edge.set_bg_color( BLANC );
    m_text_bt_supprimer_edge.set_message( "sup edge !" );
    y_bt = y_bt + m_text_bt_supprimer_edge.get_dimy();
    m_tool_box.add_child( m_bt_supprimer_edge );
    m_bt_supprimer_edge.set_dim( 110, 22 );
    m_bt_supprimer_edge.set_gravity_x( grman::GravityX::Left );
    m_bt_supprimer_edge.set_posy( y_bt );
    m_bt_supprimer_edge.set_bg_color( CYANSOMBRE );
    y_bt = y_bt + m_bt_supprimer_edge.get_dimy() + 10;
    m_tool_box.add_child( m_text_m_bt_dynamique );
    m_text_m_bt_dynamique.set_dim( 110, 22 );
    m_text_m_bt_dynamique.set_gravity_x( grman::GravityX::Left );
    m_text_m_bt_dynamique.set_posy( y_bt );
    m_text_m_bt_dynamique.set_bg_color( BLANC );
    m_text_m_bt_dynamique.set_message( "Dynamique!" );
    y_bt = y_bt + m_text_m_bt_dynamique.get_dimy();
    m_tool_box.add_child( m_bt_dynamique );
    m_bt_dynamique.set_dim( 110, 22 );
    m_bt_dynamique.set_gravity_x( grman::GravityX::Left );
    m_bt_dynamique.set_posy( y_bt );
    m_bt_dynamique.set_bg_color( CYANSOMBRE );
    y_bt = y_bt + m_bt_dynamique.get_dimy() + 10;
    ///BOUTON POUR REORGANISER LES SOMMETS
    //txt
    m_tool_box.add_child( m_text_bt_reorganisation );
    m_text_bt_reorganisation.set_dim( 110, 22 );
    m_text_bt_reorganisation.set_gravity_x( grman::GravityX::Left );
    m_text_bt_reorganisation.set_posy( y_bt );
    m_text_bt_reorganisation.set_bg_color( BLANC );
    m_text_bt_reorganisation.set_message( "Reorganiser" );
    y_bt = y_bt + m_text_bt_reorganisation.get_dimy();
    //bouton
    m_tool_box.add_child( m_bt_reorganisation );
    m_bt_reorganisation.set_dim( 110, 22 );
    m_bt_reorganisation.set_gravity_x( grman::GravityX::Left );
    m_bt_reorganisation.set_posy( y_bt );
    m_bt_reorganisation.set_bg_color( CYANSOMBRE );
    y_bt = y_bt + m_bt_reorganisation.get_dimy() + 10;
    ///BOUTON POUR Graphe de connexité
    //txt
    m_tool_box.add_child( m_text_bt_connexe );
    m_text_bt_connexe.set_dim( 110, 22 );
    m_text_bt_connexe.set_gravity_x( grman::GravityX::Left );
    m_text_bt_connexe.set_posy( y_bt );
    m_text_bt_connexe.set_bg_color( BLANC );
    m_text_bt_connexe.set_message( "Forte Connexite" );
    y_bt = y_bt + m_text_bt_connexe.get_dimy();
    //bouton
    m_tool_box.add_child( m_bt_connexe );
    m_bt_connexe.set_dim( 110, 22 );
    m_bt_connexe.set_gravity_x( grman::GravityX::Left );
    m_bt_connexe.set_posy( y_bt );
    m_bt_connexe.set_bg_color( CYANSOMBRE );
    y_bt = y_bt + m_bt_connexe.get_dimy() + 10;
    ///BOUTON POUR K-connexite
    //txt
    m_tool_box.add_child( m_text_k_connexe );
    m_text_k_connexe.set_dim( 110, 22 );
    m_text_k_connexe.set_gravity_x( grman::GravityX::Left );
    m_text_k_connexe.set_posy( y_bt );
    m_text_k_connexe.set_bg_color( BLANC );
    m_text_k_connexe.set_message( "K Connexite" );
    y_bt = y_bt + m_text_k_connexe.get_dimy();
    //bouton
    m_tool_box.add_child( m_k_connexe );
    m_k_connexe.set_dim( 110, 22 );
    m_k_connexe.set_gravity_x( grman::GravityX::Left );
    m_k_connexe.set_posy( y_bt );
    m_k_connexe.set_bg_color( CYANSOMBRE );
    y_bt = y_bt + m_k_connexe.get_dimy() + 10;
}

/// M�thode sp�ciale qui construit un graphe arbitraire (d�mo)
/// Cette m�thode est � enlever et remplacer par un syst�me
/// de chargement de fichiers par exemple.

/// Bien s�r on ne veut pas que vos graphes soient construits
/// "� la main" dans le code comme �a.
/*void Graph::make_example()
{
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);
    // La ligne pr�c�dente est en gros �quivalente � :
=======
/// Bien s�r on ne veut pas que vos graphes soient construits
/// "� la main" dans le code comme �a.
void Graph::make_example() {
    m_interface = std::make_shared<GraphInterface>( 50, 0, 750, 600 );
    // La ligne pr�c�dente est en gros �quivalente � :
>>>>>>> develop
    // m_interface = new GraphInterface(50, 0, 750, 600);
    /// Les sommets doivent �tre d�finis avant les arcs
    // Ajouter le sommet d'indice 0 de valeur 30 en x=200 et y=100 avec l'image
    // clown1.jpg etc...
    add_interfaced_vertex( 0, 30.0, 200, 100, "clown1.jpg" );
    add_interfaced_vertex( 1, 60.0, 400, 100, "clown2.jpg" );
    add_interfaced_vertex( 2, 50.0, 200, 300, "clown3.jpg" );
    add_interfaced_vertex( 3, 0.0, 400, 300, "clown4.jpg" );
    add_interfaced_vertex( 4, 100.0, 600, 300, "clown5.jpg" );
    add_interfaced_vertex( 5, 0.0, 100, 500, "bad_clowns_xx3xx.jpg", 0 );
    add_interfaced_vertex( 6, 0.0, 300, 500, "bad_clowns_xx3xx.jpg", 1 );
    add_interfaced_vertex( 7, 0.0, 500, 500, "bad_clowns_xx3xx.jpg", 2 );
    /// Les arcs doivent �tre d�finis entre des sommets qui existent !
    // AJouter l'arc d'indice 0, allant du sommet 1 au sommet 2 de poids 50 etc...
<<<<<<< HEAD
    add_interfaced_edge(0, 1, 2, 50.0);
    add_interfaced_edge(1, 0, 1, 50.0);
    add_interfaced_edge(2, 1, 3, 75.0);
    add_interfaced_edge(3, 4, 1, 25.0);
    add_interfaced_edge(4, 6, 3, 25.0);
    add_interfaced_edge(5, 7, 3, 25.0);
    add_interfaced_edge(6, 3, 4, 0.0);
    add_interfaced_edge(7, 2, 0, 100.0);
    add_interfaced_edge(8, 5, 2, 20.0);
    add_interfaced_edge(9, 3, 7, 80.0);
}*/

/// La m�thode sauvegarde le graphe dans un fichier num�rot�
void Graph::graphe_sauvegarde() {
    std::string fichier = "sauvegarde.txt";
    fichier = std::to_string( m_numero_graphe ) + fichier;
    std::ofstream ofs( fichier.c_str(), std::ios::out );
    if( ofs ) {
        ofs << "----- Fichier de sauvegarde du graphe " << m_numero_graphe << " -----" << std::endl;
        ofs << "\"indice\",\"nom du sommet\",\"valeure du sommet\",\"posx\",\"posy\",\"image\",\"rythme de croissance\",\"population\"" << std::endl;
        ofs << m_ordre << std::endl;
        for( auto &elem : m_vertices ) {
            //ofs << elem.second.m_interface->m_label_idx.get_message() <<" "<< elem.second.m_value <<" "<< elem.second.m_interface->m_top_box.get_frame().pos.x <<" "<< elem.second.m_interface->m_top_box.get_frame().pos.y <<" "<< elem.second.m_interface->m_img.get_pic_name();
            ofs << elem.second.m_interface->m_label_idx.get_message() << " ";
            ofs << elem.second.m_name << " " ;
            ofs << elem.second.m_value << " " ;
            ofs << elem.second.m_interface->m_top_box.get_frame().pos.x << " ";
            ofs << elem.second.m_interface->m_top_box.get_frame().pos.y << " ";
            ofs << elem.second.m_interface->m_img.get_pic_name() << " " ;
            ofs << elem.second.m_r << " " ;
            ofs << elem.second.m_N_t << " " ;
            ofs << std::endl ;
        }
        ofs << m_nb_arete << std::endl;
        int i = 0;
        for( auto &elem : m_edges ) {
            i++;
            ofs << i << " " << elem.second.m_from << " " << elem.second.m_to << " " << elem.second.m_weight << std::endl;
        }
    } else {
        std::cout << "probleme lors de l'ouverture du fichier de sauvegarde " << m_numero_graphe << std::endl;
    }
}

/// La m�thode update � appeler dans la boucle de jeu pour les graphes avec
/// interface


/// La m�thode sauvegarde le graphe dans un fichier num�rot�
//void Graph::graphe_sauvegarde() {}

/// La m�thode charge un fichier pour remplir les donn�es d'un graphe
void Graph::graphe_chargement() {
    //std::cout << " ------ debut chargement graphe " << m_numero_graphe
    //        << " ------- " << std::endl;
    std::string fichier = "_sauvegarde.txt";
    fichier = std::to_string( m_numero_graphe ) + fichier;
    //std::cout << "fichier = "<< fichier << std::endl;
    m_interface = std::make_shared<GraphInterface>( 50, 0, 750, 600 );
    std::ifstream ifs( fichier.c_str() );
    if( ifs ) {
        m_ordre = m_nb_arete = 0;
        int ordre = 0, num_arete = 0;
        std::string line;
        for ( int i = 0; i < 2; i++ ) {
            std::getline( ifs, line );
            //std::cout << line << " ignore " << std::endl;
        }
        ifs >> ordre;
        //std::cout << ordre << std::endl;
        for ( int i = 0; i < ordre; i++ ) {
            std::cout << "numero " << i << std::endl;
            std::string name;
            int indice, posx, posy, Nt;
            float r;
            double valeur;
            std::string image;
            ifs >> indice >> name >> valeur >> posx >> posy >> image >> r >> Nt;
            add_interfaced_vertex( indice, valeur, posx, posy, image.c_str(), r, Nt, name.c_str() );
            //std::cout << " sommet numero " << indice << " : ok!" << std::endl;
        }
        ifs >> num_arete;
        for ( int i = 0; i < num_arete; i++ ) {
            int indice, som1, som2;
            double poids;
            ifs >> indice >> som1 >> som2 >> poids;
            add_interfaced_edge( indice, som1, som2, poids );
            // std::cout << " arete numero " << indice << " : ok!" << std::endl;
        }
    } else {
        std::cout << "probleme lors de l'ouverture du fichier de chargement " << m_numero_graphe << std::endl;
    }
    matrice_adjacent();
}

/// La m�thode pour compl�ter la matrice d'adjacence
void Graph::matrice_adjacent() {
    adjacence.resize( m_vertices.size(), std::vector<int>( m_vertices.size(), 0 ) );
    for ( auto &elem : m_edges ) {
        adjacence[elem.second.m_from][elem.second.m_to] = 1;
    }
    for ( unsigned int i = 0; i < m_vertices.size(); i++ ) {
        for ( unsigned int j = 0; j < m_vertices.size(); j++ ) {
            //std::cout << adjacence[i][j] << " ";
            // std::cout << j;
        }
        //std::cout << std::endl;
    }
}

/// La m�thode update � appeler dans la boucle de jeu pour les graphes avec
/// interface

void Graph::update() {
    if ( !m_interface )
        return;
    for ( auto &elt : m_vertices )
        elt.second.pre_update();
    for ( auto &elt : m_edges )
        elt.second.pre_update();
    m_interface->m_top_box.update();
    bouton_ajouter_vertex();
    bouton_supprimer_vertex();
    bouton_ajouter_edge();
    bouton_supprimer_edge();
    boutondynamiquedechainealimentaire();
    bouton_reorganisation();
    bouton_forte_connexite();
    bouton_k_connexite_k_plet();
    m_Timer.draw();
    if ( keypressed() ) {
        std::cout << readkey() << std::endl;
        graphe_reduit();
    }
    if ( grman::key_press[KEY_RIGHT] )
        std::cout << " touche appuye " << std::endl;
    if ( grman::key_unpress[KEY_RIGHT] )
        std::cout << " touche relache " << std::endl;
    for ( auto &elt : m_vertices )
        elt.second.post_update();
    for ( auto &elt : m_edges )
        elt.second.post_update();
}


/// Aide � l'ajout de sommets interfac�s
void Graph::add_interfaced_vertex( int idx, double value, int x, int y, std::string pic_name, float r, int Nt, std::string name, int pic_idx ) {
    m_ordre++;
    if ( m_vertices.find( idx ) != m_vertices.end() ) {
        std::cerr << "Error adding vertex at idx=" << idx << " already used..." << std::endl;
        throw "Error adding vertex";
    }
    // Cr�ation d'une interface de sommet
    VertexInterface *vi = new VertexInterface( idx, x, y, pic_name, pic_idx );
    // Ajout de la top box de l'interface de sommet
    m_interface->m_main_box.add_child( vi->m_top_box );
    // On peut ajouter directement des vertices dans la map avec la notation crochet :
    m_vertices[idx] = Vertex( value, r, Nt, Nt, vi, name );
}

/// Aide � l'ajout d'arcs interfac�s
void Graph::add_interfaced_edge( int idx, int id_vert1, int id_vert2, double weight ) {
    m_nb_arete++;
    if ( m_edges.find( idx ) != m_edges.end() ) {
        std::cerr << "Error adding edge at idx=" << idx << " already used..." << std::endl;
        throw "Error adding edge";
    }
    if ( m_vertices.find( id_vert1 ) == m_vertices.end() ||
            m_vertices.find( id_vert2 ) == m_vertices.end() ) {
        std::cerr << "Error adding edge idx=" << idx << " between vertices "
                  << id_vert1 << " and " << id_vert2 << " not in m_vertices"
                  << std::endl;
        throw "Error adding edge";
    }
    EdgeInterface *ei =
        new EdgeInterface( m_vertices[id_vert1], m_vertices[id_vert2] );
    m_interface->m_main_box.add_child( ei->m_top_edge );
    m_edges[idx] = Edge( weight, ei );
    m_edges[idx].m_from = id_vert1;
    m_edges[idx].m_to = id_vert2;
    m_vertices[id_vert1].m_out.push_back( id_vert2 );
    m_vertices[id_vert2].m_in.push_back( id_vert1 );
}

/// M�thode pour effacer un sommet
void Graph::remove_vertex( int index ) {
    Vertex &remve = m_vertices.at( index );
    std::cout << "Removing vertex " << index << std::endl;
    ///suppression des arcs arrivant dans le sommet index
    for( auto &elem : remve.m_in ) {
        std::cout << "arc arrivant dans " << index << " " << elem << std::endl;
        for( auto &elemedge : m_edges ) {
            ///test pour trouver l'index de l'arc qui va du sommet elem vers notre sommet index
            if( elemedge.second.m_from == elem && elemedge.second.m_to == index ) {
                remove_edge( elemedge.first );
            }
        }
    }
    ///suppression des arcs partant du sommet index
    for( auto &elem : remve.m_out ) {
        std::cout << "arc partant de " << index << " " << elem << std::endl;
        for( auto &elemedge : m_edges ) {
            ///test pour trouver l'index de l'arc qui va de notre sommet index vers le sommet elem
            if( elemedge.second.m_from == index && elemedge.second.m_to == elem ) {
                remove_edge( elemedge.first );
            }
        }
    }
    ///maintenant qu'il n'y a plus d'arc connect� � notre sommet on peut le supprimer
    /// test : on a bien des �l�ments interfac�s
    if( m_interface && remve.m_interface ) {
        m_interface->m_main_box.remove_child( remve.m_interface->m_top_box );
    }
    /// aucune information � supprimer dans les autres sommets vu qu'ils n'ont pas l'indice de notre sommet index
    /// suppression du sommet du vecteur vertice
    m_vertices.erase( index );
    m_ordre--;
}

/// effacer un arc
/// eidx index of edge to remove
void Graph::remove_edge( int eidx ) {
    /// r�f�rence vers le Edge � enlever
    Edge &remed = m_edges.at( eidx );
    std::cout << "Removing edge " << eidx << " " << remed.m_from << "->" << remed.m_to << " " << remed.m_weight << std::endl;
    /// Tester la coh�rence : nombre d'arc entrants et sortants des sommets 1 et 2
    //std::cout << m_vertices[remed.m_from].m_in.size() << " " << m_vertices[remed.m_from].m_out.size() << std::endl;
    //std::cout << m_vertices[remed.m_to].m_in.size() << " " << m_vertices[remed.m_to].m_out.size() << std::endl;
    //std::cout << m_edges.size() << std::endl;
    /// test : on a bien des �l�ments interfac�s
    if ( m_interface && remed.m_interface ) {
        /// Ne pas oublier qu'on a fait �a � l'ajout de l'arc :
        /* EdgeInterface *ei = new EdgeInterface(m_vertices[id_vert1], m_vertices[id_vert2]); */
        /* m_interface->m_main_box.add_child(ei->m_top_edge); */
        /* m_edges[idx] = Edge(weight, ei); */
        /// Le new EdgeInterface ne n�cessite pas de delete car on a un shared_ptr
        /// Le Edge ne n�cessite pas non plus de delete car on n'a pas fait de new (s�mantique par valeur)
        /// mais il faut bien enlever le conteneur d'interface m_top_edge de l'arc de la main_box du graphe
        m_interface->m_main_box.remove_child( remed.m_interface->m_top_edge );
    }
    /// Il reste encore � virer l'arc supprim� de la liste des entrants et sortants des 2 sommets to et from !
    /// References sur les listes de edges des sommets from et to
    std::vector<int> &vefrom = m_vertices[remed.m_from].m_out;
    std::vector<int> &veto = m_vertices[remed.m_to].m_in;
    vefrom.erase( std::remove( vefrom.begin(), vefrom.end(), eidx ), vefrom.end() );
    veto.erase( std::remove( veto.begin(), veto.end(), eidx ), veto.end() );
    /// Le Edge ne n�cessite pas non plus de delete car on n'a pas fait de new (s�mantique par valeur)
    /// Il suffit donc de supprimer l'entr�e de la map pour supprimer � la fois l'Edge et le EdgeInterface
    /// mais malheureusement ceci n'enlevait pas automatiquement l'interface top_edge en tant que child de main_box !
    m_edges.erase( eidx );
    m_nb_arete--;
    /// Tester la coh�rence : nombre d'arc entrants et sortants des sommets 1 et 2
    //std::cout << m_vertices[remed.m_from].m_in.size() << " " << m_vertices[remed.m_from].m_out.size() << std::endl;
    //std::cout << m_vertices[remed.m_to].m_in.size() << " " << m_vertices[remed.m_to].m_out.size() << std::endl;
    //std::cout << m_edges.size() << std::endl;
}

void Graph::bouton_reorganisation() {
    if( m_interface->m_bt_reorganisation.clicked() ) {
      matrice_adjacent();
        spring_model( adjacence, false ); //on envoie la matrice du Graphe et false pour dire de ne pas dessiner
    }
}
void Graph::bouton_k_connexite_k_plet() {
    int i = -1;
    bool toto = true;
    int compte = 0;
    if( m_interface->m_k_connexe.clicked() ) {
        std::cout << "K_Connexité " << std::endl;
        std::vector<std::vector<int>> a = kplet( compte );
        while ( toto == true ) {
            std::cout << "Quel couple de solution voulez vous afficher : " << std::endl;
            std::cin >> i;
            if( i < compte - 1 ) {
                for( auto &elem : m_vertices ) {
                    elem.second.m_interface->m_top_box.set_bg_color( BLANCBLEU );
                }
                toto = false;
                for( auto &elem : a[i] ) {
                    std::map<int, Vertex>::iterator it;
                    it = m_vertices.find( elem );
                    it->second.m_interface->m_top_box.set_bg_color( ROSE );
                }
            } else {
                std::cout << "Veuillez selectionner une clés valide" << std::endl;
            }
        }
    }
}

void Graph::bouton_forte_connexite() {
    if( m_interface->m_bt_connexe.clicked() ) {
matrice_adjacent();
        graphe_reduit();//on lance l'algo du graphe réduit
    }
}

///Les m�thodes pour la gestion du Graph
void Graph::bouton_ajouter_vertex() {
    if( m_interface->m_bt_ajouter_vertex.clicked() ) {
        char fichier[100];
        char nomdusommet[100];
        bool onpasse = false;
        int k = -1;
        BITMAP *iceberg;
        BITMAP *image;
        int indice = -1;
        int a = 0;
        iceberg = grman::get_picture( "chutes.jpg" ); //http://wallpaperswide.com/seagulls_on_an_iceberg-wallpapers.html
        if ( !iceberg ) {
            allegro_message( "pas pu trouver/charger chutes.jpg" );
            allegro_exit();
            //exit( EXIT_FAILURE );
        }
        for( auto &elem : m_vertices ) {
            //std::cout << "Cles : " << elem.first << "||Nom du sommet : " << elem.second.m_name << std::endl;
            std::cout << "Nom du sommet : " << elem.second.m_name << std::endl;
        }
        std::map<int, Vertex>::iterator it;
        do {
            indice++;
            it = m_vertices.find( indice );
        } while( it != m_vertices.end() );
        if ( it == m_vertices.end() ) {
            std::cout << "Veuillez entrer un nom de sommet au choix : " << std::endl;
            std::cin >> nomdusommet;
            strcpy ( fichier, nomdusommet ); //on donne le nom du sommet
            strcat( fichier, ".jpg" ); //on ajoute l'extension jpg
            image = grman::get_picture( fichier );
            if ( !image ) {
                allegro_message( "pas pu trouver/charger l'image" );
            }
            BITMAP *buffer;
            buffer = create_bitmap( SCREEN_W, SCREEN_H );
            while ( k < 0 ) {
                clear_bitmap( buffer );
                blit( iceberg, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
                blit( image, buffer, 0, 0, 325, 117, image->w, image->h );
                if( mouse_b & mouse_x > 250 & mouse_x<770 & mouse_y>290 & mouse_y < 420 ) {
                    k = 2;
                }
                if( mouse_b & mouse_x > 790 & mouse_x<1000 & mouse_y>143 & mouse_y < 260 ) {
                    k = 4;
                }
                textout_ex( buffer, font, nomdusommet, 330, 33, makecol( 0, 0, 0 ), -1 );
                textprintf_ex( buffer, font, 335, 75, makecol( 0, 0, 0 ), -1, " %d", indice );
                blit( buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
            }
            // appel de la fonction add_interface_vertex pour ajouter un sommet au graph
            if( k == 2 ) {
                add_interfaced_vertex( indice, 10, 30, 30, fichier, 0.01, 20, nomdusommet );
            }
        }
        matrice_adjacent();
        set_ordre(m_vertices.size());
    }
}


void Graph::bouton_supprimer_vertex() {
    if( m_interface->m_bt_supprimer_vertex.clicked() ) {
        BITMAP *iceberg;
        iceberg = grman::get_picture( "chutes2.jpg" );
        BITMAP *buffer;
        std::map<int, Vertex>::iterator it;
        buffer = create_bitmap( SCREEN_W, SCREEN_H );
        int num_sommet_a_enlever = 0;
        int k = -2;
        bool toto = true;
        for( auto &elem : m_vertices ) {
            std::cout << "Cles : " << elem.first << " Nom du sommet : " << elem.second.m_name << std::endl;
        }
        while ( toto == true ) {
            std::cout << " Quelle sommet voulez-vous enlever " << std::endl;
            std::cin >> num_sommet_a_enlever;
            it = m_vertices.find( num_sommet_a_enlever );
            if( it != m_vertices.end() ) {
                toto = false;
            } else {
                std::cout << "  Veuillez selectionner une clés valide" << std::endl;
            }
        }
        while ( k < 0 ) {
            clear_bitmap( buffer );
            blit( iceberg, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
            if( mouse_b & mouse_x > 250 & mouse_x<770 & mouse_y>290 & mouse_y < 420 ) {
                k = 2;
            }
            if( mouse_b & mouse_x > 790 & mouse_x<1000 & mouse_y>25 & mouse_y < 130 ) {
                k = 4;
            }
            /*if(mouse_b)
            {
                std::cout << "mouse_x" << mouse_x << std::endl;
                std::cout << "mouse_y" << mouse_y << std::endl;
            }       */
            textprintf_ex( buffer, font, 465, 47, makecol( 0, 0, 0 ), -1, " %d", num_sommet_a_enlever );
            blit( buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
        }
        // appel de la fonction remove avec en paramètre l'indice de l'arc à supprimer
        if( k == 2 ) {
            remove_vertex( num_sommet_a_enlever );
        }
        matrice_adjacent();
        set_ordre(m_vertices.size());
    }
}
void Graph::bouton_ajouter_edge() {
    if( m_interface->m_bt_ajouter_edge.clicked() ) {
        int depart = 0;
        int arriver = 0;
        int poid = 0;
        int indice = -1;
        for( auto &elem : m_vertices ) {

            std::cout << "Nom du sommet : " << elem.second.m_name << std::endl;
        }
        std::map<int, Edge>::iterator it;
        do {
            indice++;
            it = m_edges.find( indice );
        } while( it != m_edges.end() && indice < m_edges.size()+1);
        it = m_edges.find( indice );
        std::cout << " Arete partant de : " << std::endl;
        std::cin >> depart;
        std::cout << " et arrivant en sommet d'indice : " << std::endl;
        std::cin >> arriver;
        std::cout << " Entrez le poid de l'arete en question " << std::endl;
        std::cin >> poid;
        // appel de la fonction add_interfaced_edge pour ajouter un arc au graph
            add_interfaced_edge( indice, depart, arriver, poid );
            matrice_adjacent();
    }

}

void Graph::boutondynamiquedechainealimentaire() {
    if( m_interface->m_bt_dynamique.clicked() ) {
        dynamique_population();
        // appel de la fonction remove avec en paramètre l'indice de l'arc à supprimer
    }
}
void Graph::bouton_supprimer_edge() {

    if( m_interface->m_bt_supprimer_edge.clicked() ) {
      int num_arete_a_enlever = 0;
     bool toto = true;
        for( auto &elem : m_edges ) {
            std::cout << "n° : " << elem.first << " || arete de " << elem.second.m_from << " vers "  <<  elem.second.m_to << std::endl;
        }
        while ( toto == true ) {
            std::cout << " Quelle arete voulez-vous enlever " << std::endl;
            std::cin >> num_arete_a_enlever;
            std::map<int, Edge>::iterator it;
            it = m_edges.find( num_arete_a_enlever );
            if( it != m_edges.end() ) {
                toto = false;
            } else {
                std::cout << "  Veuillez selectionner une clés valide" << std::endl;
            }
        }
        // appel de la fonction remove avec en paramètre l'indice de l'arc à supprimer
        remove_edge( num_arete_a_enlever );
        matrice_adjacent();
    }
}

/// m�thode de la dynamique de population
void Graph::dynamique_population() {
    // ajouter un bouton pour passer au tour de jeu suivant
    int k = 0;
    int preda = 0;
    for( auto &elem : m_vertices ) {
        fctreproduction( elem.first );
        k = calcul_K( elem.first );
        elem.second.m_K = k;
        elem.second.m_N_t = elem.second.m_N_t_1;
        if( k == 0 ) {
            k = 1;
        }
        preda = predation( elem.first );
        //std::cout << " population avant : " << elem.second.m_N_t << std::endl;
        int w = ( 1 - ( elem.second.m_N_t / k ) );
        if ( w == 0 ) {
            w = 1;
        }
        //if(preda>500)
        // {
        //  preda= preda/2;
        //}
        int at = elem.second.m_N_t + elem.second.m_r * elem.second.m_N_t * w + preda;
        //if(at - elem.second.m_N_t <elem.second.m_N_t/1.5)
        //{
        //  at = elem.second.m_N_t/1.3;
        std::cout << "at = " << at << std::endl;
        //}
        int somme = 0;
        for ( int i = 0; i < elem.second.m_in.size(); i++ ) {
            std::map<int, Edge>::iterator it;
            it = m_edges.find( elem.second.m_in[i] );
            somme = somme + it->second.m_weight;
            std::cout << "sommet : " << elem.first << " somme = " << somme << std::endl;
        }
        elem.second.m_N_t_1 = at;
        if( somme == 0 ) {
            elem.second.m_N_t_1 = 0;
        }
        if( elem.second.m_N_t_1 < 0 ) {
            elem.second.m_N_t_1 = 0;
        }
        if ( elem.second.m_N_t_1 == 21 ) {
            elem.second.m_N_t = 0;
        }
        //std::cout << " Num de sommet : " << elem.first << std::endl;
        //std::cout << " population apres : " << elem.second.m_N_t << std::endl;
    }
    miseajoutarete();
    //std::cout << "----- FIN ----" << std::endl;
}

int Graph::calcul_K( int num_vertex_donne ) {
    int a;
    int b;
    std::vector<int> somme;
    for ( auto &elem : m_edges ) {
        a = 0;
        b = 0;
        if ( elem.second.m_to == num_vertex_donne ) {
            std::map<int, Vertex>::iterator it;
            it = m_vertices.find( elem.second.m_from );
            a = it->second.m_N_t; // On prend la value du sommet en question
            b = elem.second.m_weight;
            //std::cout << " a = " << a << "b = " << b << std::endl;
            somme.push_back( a * b );
        }
    }
    int results = 0;
    //std::cout << "On print la K : " << std::endl;
    for ( unsigned int i = 0; i < somme.size(); i++ ) {
        //std::cout << somme[i] <<" + ";
        results = somme[i] + results;
    }
    //std::cout << std::endl;
    return results;
}

int Graph::predation( int num_vertex_donne ) {
    int a;
    int b;
    std::vector<int> somme;
    for ( auto &elem : m_edges ) {
        a = 0;
        b = 0;
        if ( elem.second.m_from == num_vertex_donne ) {
            std::map<int, Vertex>::iterator it;
            it = m_vertices.find( elem.second.m_to );
            a = it->second.m_N_t; // On prend la value du sommet en question
            b = elem.second.m_weight;
            // std::cout << " a = " << a << "b = " << b << std::endl;
            somme.push_back( a * b );
        }
    }
    int results = 0;
    // std::cout << "On print la sommme : " << std::endl;
    for ( unsigned int i = 0; i < somme.size(); i++ ) {
        //std::cout << somme[i] <<" + ";
        results = somme[i] + results;
    }
    // std::cout << std::endl;
    results = -results;
    return results;
}

void Graph::fctreproduction( int num_vertex_donne ) {
    std::map<int, Vertex>::iterator it;
    it = m_vertices.find( num_vertex_donne );
    //int pop_avant = it->second.m_value;
    it->second.m_N_t = ceil( it->second.m_N_t_1 + ( it->second.m_N_t_1 ) * 0.55 / 2 ) ;
    //std::cout << "on reprodui le sommet " << num_vertex_donne <<std::endl;
    //std::cout << "Pop_avant : " << pop_avant << " Pop_apres " <<it->second.m_value <<std::endl;
}

void Graph::miseajoutarete() {
    for ( auto &elem : m_edges ) {
        std::map<int, Vertex>::iterator it;
        it = m_vertices.find( elem.second.m_from );
        int a = it->second.m_N_t_1; // On prend la value du sommet en question
        std::cout << "arete : " << elem.first << " + poid : " << elem.second.m_weight << std::endl;
        elem.second.m_weight = a / 4;
        std::cout << "arete : " << elem.first << " + poid : " << elem.second.m_weight << std::endl << std::endl;
        if ( elem.second.m_weight < 0 ) {
            elem.second.m_weight = 1;
        }
    }
}




// algo composantes connexes
std::vector<std::vector<int>> Graph::toutesLesComposanteFortementConnexe() {
    matrice_adjacent();//reactualise la matrice d'adjacence
    std::vector<std::vector<int>> tabc; // tab des composantes fortements connexes
    std::vector<int> marques;           // tab de marquages
    int x, y;                           // sommets intermediaires
    /**allocation memoire de tabc et marques**/
    tabc.resize( m_vertices.size() );
    for ( int i = 0; i < m_vertices.size(); i++ ) {
        tabc.at( i ).resize( m_vertices.size() );
    }
    marques.resize( m_vertices.size() );
    // init a 0 des deux tabs
    for ( int i = 0; i < m_vertices.size(); i++ ) {
        marques.at( i ) = 0;
        for ( int j = 0; j < m_vertices.size(); j++ ) {
            tabc.at( i ).at( j ) = 0;
        }
    }
    int cpt = 0;
    for ( x = 0; x < m_vertices.size(); x++ ) {
        if ( !marques[x] ) {
            cpt += 1;
            tabc[x] = uneComposantesFortementConnexes( x );
            marques[x] = 1;
            /*for ( y = 0; y < m_vertices.size(); y++ )
            {
               // if ( tabc[x][y] && !marques[y] )
                    //marques[y] = 1;
            }*/
        }
    }
    std::cout << "CPT :" << cpt;
    std::ofstream fichier(
        "test.txt", std::ios::out | std::ios::trunc ); // ouverture en �criture
    // avec effacement du
    // fichier ouvert
    if ( fichier ) {
        for ( int i = 0; i < m_vertices.size(); i++ ) {
            for ( int j = 0; j < m_vertices.size(); j++ ) {
                fichier << tabc.at( i ).at( j ) << " ";
            }
            fichier << std::endl;
        }
        fichier.close();
    } else
        std::cerr << "Impossible d'ouvrir le fichier !" << std::endl;
    return tabc;
}

std::vector<int> Graph::uneComposantesFortementConnexes( int s ) {
    std::vector<int> c1, c, c2, marques;
    int x, y;
    int ajoute = 1;
    // allocation memoire
    c1.resize( m_vertices.size() );
    c2.resize( m_vertices.size() );
    c.resize( m_vertices.size() );
    marques.resize( m_vertices.size() );
    // init valeur a 0
    for ( int i = 0; i < m_vertices.size(); i++ ) {
        c1.at( i ) = 0;
        c2.at( i ) = 0;
        c.at( i ) = 0;
        marques.at( i ) = 0;
    }
    // on rend le sommet s connexe
    c1[s] = 1;
    c2[s] = 1;
    // recherche des composantes connexes partant de s a ajouter a c1
    while ( ajoute ) {
        ajoute = 0;
        for ( x = 0; x < m_vertices.size(); x++ ) {
            if ( !marques[x] && c1[x] ) {
                marques[x] = 1;
                for ( y = 0; y < m_vertices.size(); y++ ) {
                    // std::cout <<" val :"<<adjacence.at(x).at(y);
                    if ( adjacence[x][y] && !marques[y] ) { // ERROR HERE
                        c1[y] = 1;
                        ajoute = 1;
                    }
                }
            }
        }
    }
    for ( int i = 0; i < m_vertices.size(); i++ ) {
        marques.at( i ) = 0;
    }
    ajoute = 1;
    std::vector<std::vector<int>> Tadjacence = transpose();
    // recherche des composantes connexes arrivant a s a ajouter dans c2
    while ( ajoute ) {
        ajoute = 0;
        for ( x = 0; x < m_vertices.size(); x++ ) {
            if ( !marques[x] && c2[x] ) {
                marques[x] = 1;
                for ( y = 0; y < m_vertices.size(); y++ ) {
                    // std::cout <<" val :"<<adjacence.at(x).at(y);
                    if ( adjacence[y][x] && !marques[y] ) { // ERROR HERE
                        c2[y] = 1;
                        ajoute = 1;
                    }
                }
            }
        }
    }
    // composante fortement connexe c =intersection de c1 et c2
    for ( x = 0; x < m_vertices.size(); x++ ) {
        c[x] = c1[x] & c2[x];
        //        std::cout << c1[x] << " " ;
    }
    //    std::cout << std::endl;
    return c;
}
std::vector<std::vector<int>>
Graph::transpose() { // Get transpose of matrice and return it
    std::vector<std::vector<int>> T;
    T.resize( m_vertices.size(),
              std::vector<int>( m_vertices.size(), 0 ) ); // resize double vector
    for ( int i = 0; i < m_vertices.size(); i++ ) {
        for ( int j = 0; j < m_vertices.size(); j++ ) {
            T[i][j] = adjacence[j][i]; // on inverse
            // std::cout << T[i][j] << " ";
        }
        // std::cout << std::endl;
    }
    return T;
}

void Graph::graphe_reduit() {
    std::vector<std::vector<int>> tabc(
                                   toutesLesComposanteFortementConnexe() ); // tab des composantes fortements
    // connexes
    // grman::init_popup();
    int ordre = m_vertices.size();
    std::vector<bool> marques( ordre, false ); // tab de marquages
    groupes_fortements_connexes.clear(); //reinit le vect
    int lig = 0;
    for ( int i = 0; i < ordre; i++ ) {
        if ( !marques.at( i ) ) {
            marques.at( i ) = true;
            lig++;
            groupes_fortements_connexes.resize( lig );
            for ( int j = 0; j < ordre; j++ ) {
                if ( tabc[i][j] ) {
                    groupes_fortements_connexes[lig - 1].push_back( j );
                    marques.at( j ) = true;
                }
            }
        }
    }
    // tst dans un fichier tst2
    std::ofstream fichier(
        "test2.txt", std::ios::out | std::ios::trunc ); // ouverture en �criture
    // avec effacement du
    // fichier ouvert
    if ( fichier ) {
        for ( int i = 0; i < groupes_fortements_connexes.size(); i++ ) {
            for ( int j = 0; j < groupes_fortements_connexes[i].size(); j++ ) {
                fichier << groupes_fortements_connexes[i][j] << " ";
            }
            fichier << std::endl;
        }
        fichier.close();
    } else
        std::cerr << "Impossible d'ouvrir le fichier !" << std::endl;
    std::vector<std::vector<int>> adjacence_graphe_reduit = groupes_fortements_connexes_to_matrice( groupes_fortements_connexes ) ;
    spring_model( adjacence_graphe_reduit );
}

std::vector<std::vector<int>> Graph::groupes_fortements_connexes_to_matrice(
std::vector<std::vector<int>> groupes_fortements_connexes ) {
    std::vector<std::vector<int>> IM;//matrice d'adjacence intermediaire
    std::vector<std::vector<int>> M;//matrice d'adjacence
    IM.resize( groupes_fortements_connexes.size(), std::vector<int>( adjacence.size(), 0 ) ); //lig: sommet graphe reduit, col:matrice d'adjacence de base
    M.resize( groupes_fortements_connexes.size(), std::vector<int>( groupes_fortements_connexes.size(), 0 ) );
    for ( int i = 0; i < groupes_fortements_connexes.size(); i++ ) {
        for ( int j = 0; j < groupes_fortements_connexes[i].size(); j++ ) {
            for ( int k = 0; k < adjacence.size(); k++ ) {
                IM[i][k] = IM[i][k] | adjacence[groupes_fortements_connexes[i][j]][k];
            }
        }
    }
    for ( int i = 0; i < groupes_fortements_connexes.size(); i++ ) { //on cast la matrice intermediare de ses colonnes
        for ( int j = 0; j < groupes_fortements_connexes[i].size(); j++ ) {
            for ( int k = 0; k < IM.size(); k++ ) {
                M[k][i] = M[k][i] | IM[k][groupes_fortements_connexes[i][j]];
            }
        }
    }
    for ( int i = 0; i < M.size(); i++ ) { //on enleve les aretes qui pointes sur le même sommet
        for ( int j = 0; j < M.size(); j++ ) {
            if ( i == j )
                M[i][j] = 0;
        }
    }
    // tst dans un fichier tst3
    std::ofstream fichier(
        "test3.txt", std::ios::out | std::ios::trunc ); // ouverture en �criture
    // avec effacement du
    // fichier ouvert
    if ( fichier ) {
        for ( int i = 0; i < IM.size(); i++ ) {
            for ( int j = 0; j < IM[i].size(); j++ ) {
                fichier << IM[i][j] << " ";
            }
            fichier << std::endl;
        }
        fichier << std::endl;
        fichier << std::endl;
        fichier << std::endl;
        for ( int i = 0; i < M.size(); i++ ) {
            for ( int j = 0; j < M[i].size(); j++ ) {
                fichier << M[i][j] << " ";
            }
            fichier << std::endl;
        }
        fichier.close();
    } else
        std::cerr << "Impossible d'ouvrir le fichier !" << std::endl;
    return M;
}

void Graph::spring_model(
    std::vector<std::vector<int>>
    tabadjacence, bool draw ) {  // matrice d'adjacence pour les aretes
    int ordre = tabadjacence.size();
    /// FORCE D'ATTRACTION ET REPULSION///
    std::vector<std::vector<int>> netforce; // vector de force pour chaque sommet
    netforce.resize( ordre,
                     std::vector<int>( 2, 0 ) ); // resize avec 2 cases pour x et y
    /// POS X ET Y DE CHAQUE SOMMETS///
    std::vector<std::vector<int>> pos; // vector de positions pour chaque sommet
    pos.resize( ordre, std::vector<int>( 2, 0 ) ); // resize avec 2 cases pour x et y
    /// VELOCITE///
    std::vector<std::vector<int>>
                               velocity; // vector de positions pour chaque sommet
    velocity.resize( ordre,
                     std::vector<int>( 2, 0 ) ); // resize avec 2 cases pour x et y
    /// tableau de position en rand() pour chaque sommet (vertices)
    /// A FAIRE!!!!
    for ( int i = 0; i < pos.size(); i++ ) {
        for ( int j = 0; j < pos[i].size(); j++ ) {
            if ( j == X )
                // pos[i][j] = rand() % (MAX_X - MIN_X) + MIN_X;
                pos[i][j] = rand() % ( MAX_X - MIN_X ) + MIN_X;
            else
                // pos[i][j] = rand() % (MAX_Y - MIN_Y) + MIN_Y;
                pos[i][j] = rand() % ( MAX_Y - MIN_Y ) + MIN_Y;
        }
    }
    int count;
    if( draw )
        count = ITERATION;
    else
        count = O_ITERATION;
    for ( int iteration = 0; iteration < count; iteration++ ) {
        int v;
        for ( int i = 0; i < ordre; i++ ) { // loop dans les sommets
            v = i;                            //= au sommet en cours
            int u;                            // un autre sommet
            netforce[v][0] = 0;               // mise a 0 de x
            netforce[v][1] = 0;               // mise a 0 de y
            for ( int j = 0; j < ordre; j++ ) { // loop dans les autres sommets
                if ( i != j ) {
                    u = j;
                    // squared distance entre "u" & "v"
                    // ((v.x-u.x)*(v.x-u.x)+(v.y-u.y)*(v.y-u.y))
                    int rsq = ( ( pos[v][X] - pos[u][X] ) * ( pos[v][X] - pos[u][X] ) +
                                ( pos[v][Y] - pos[u][Y] ) * ( pos[v][Y] - pos[u][Y] ) );
                    // counting the repulsion between two vertices
                    if ( draw ) {
                        netforce[v][X] += REPULSION * ( pos[v][X] - pos[u][X] ) /
                                          rsq; // 200 est une constante a changer si pas OK
                        netforce[v][Y] += REPULSION * ( pos[v][Y] - pos[u][Y] ) / rsq;
                    } else {
                        netforce[v][X] += O_REPULSION * ( pos[v][X] - pos[u][X] ) /
                                          rsq; // 200 est une constante a changer si pas OK
                        netforce[v][Y] += O_REPULSION * ( pos[v][Y] - pos[u][Y] ) / rsq;
                    }
                }
            }
            for ( int j = 0; j < ordre; j++ ) { // loop dans les arcs
                if ( tabadjacence[i][j] ) {
                    u = j;
                    /// calcul de l'attraction
                    if ( draw ) {
                        netforce[v][X] +=
                            ATTRACTION *
                            ( pos[u][X] -
                              pos[v][X] ); // 0.06 est une constante a changer si pas OK
                        netforce[v][Y] += ATTRACTION * ( pos[u][Y] - pos[v][Y] );
                    } else {
                        netforce[v][X] +=
                            O_ATTRACTION *
                            ( pos[u][X] -
                              pos[v][X] ); // 0.06 est une constante a changer si pas OK
                        netforce[v][Y] += O_ATTRACTION * ( pos[u][Y] - pos[v][Y] );
                    }
                }
            }
            // calcul de velocite (avec amortissement de 0.85)
            velocity[v][X] = ( velocity[v][X] + netforce[v][X] ) * AMORTISSEMENT;
            velocity[v][Y] = ( velocity[v][Y] + netforce[v][Y] ) * AMORTISSEMENT;
        }
        for ( int i = 0; i < ordre;
                i++ ) { // actualisation des pos avec les nouvelles
            v = i;
            // if(v.isDragged){ v.x = mouseX; v.y = mouseY; }
            // else { v.x += v.velocity.x; v.y += v.velocity.y; }
            if( draw ) { //cas de figure du graphe reduit
                if ( pos[v][X] < MAX_X_POPUP - ( RADIUS_NODE + 50 ) && pos[v][X] > 0 + RADIUS_NODE ) //50 est arbitraire
                    pos[v][X] += velocity[v][X];
                if ( pos[v][Y] < MAX_Y_POPUP - ( RADIUS_NODE + 50 ) && pos[v][Y] > 0 + RADIUS_NODE )
                    pos[v][Y] += velocity[v][Y];
            } else { //cas de figure du graphe normal
                if ( pos[v][X] < MAX_X_POPUP - 250 && pos[v][X] > 0 + RADIUS_NODE )//200 est arbitraire
                    pos[v][X] += velocity[v][X];
                if ( pos[v][Y] < MAX_Y_POPUP - 100 && pos[v][Y] > 0 + RADIUS_NODE )
                    pos[v][Y] += velocity[v][Y];
            }
        }
    }
    if ( draw )
        draw_graph_reduit_on_bmp( pos, tabadjacence ); //ouvre popup et affiche le graph en fixe
    else
        actualisation_pos_sommet( pos ); //reactualise les pos des sommets, n'ouvre pas de popup
}

void Graph::actualisation_pos_sommet( std::vector<std::vector<int>> pos ) {
    for ( const auto& elem : m_vertices ) {
        elem.second.m_interface->m_top_box.set_pos( pos[elem.first][X], pos[elem.first][Y] );
    }
}
void Graph::draw_graph_reduit_on_bmp( std::vector<std::vector<int>> pos, std::vector<std::vector<int>> tabadjacence ) {
    int ordre = pos.size();
    /// DRAWING GRAPH ON BITMAP
    BITMAP *bmp_graphe = create_bitmap( 1024, 700 );
    clear_to_color( bmp_graphe, makecol( 100, 100, 100 ) );
    // drawing edges
    for ( int i = 0; i < ordre; i++ ) {
        for ( int j = 0; j < ordre; j++ ) {
            if ( tabadjacence[i][j] ) { // si il y a une arete alors..
                // line( bmp_graphe, pos[i][X], pos[i][Y], pos[j][X], pos[j][Y],
                //       makecol( 0, 200, 0 ) ); // on trace l'arete
                grman::thick_line( bmp_graphe, pos[i][X], pos[i][Y], pos[j][X], pos[j][Y], 2,  makecol( 0, 200, 0 ) );
                float H = sqrt( ( pos[j][X] - pos[i][X] ) * ( pos[j][X] - pos[i][X] ) + ( pos[j][Y] - pos[i][Y]  ) * ( pos[j][Y] - pos[i][Y] ) );
                float O = pos[j][Y] - pos[i][Y]    ;
                float teta = asin( O / H );
                if ( pos[i][X] < pos[j][X] ) {
                    teta += M_PI;
                } else {
                    teta = -teta;
                }
//               std::cout << " teta: "<<teta;
                circlefill( bmp_graphe, pos[j][X] + ( RADIUS_NODE )*cos( teta ), pos[j][Y] + ( RADIUS_NODE )*sin( teta ), RADIUS_FLECHE, makecol( 0, 200, 0 ) );
                //float PHI=(teta+M_PI/4)*RADIUS_FLECHE;
                //int x,y;
                //x=pos[j][X] + (RADIUS_NODE)*cos( teta );
                //y=pos[j][Y] + (RADIUS_NODE)*sin( teta );
                //grman::thick_line( bmp_graphe, x , y ,pos[j][X] + (RADIUS_NODE+10 )*cos( teta ),pos[j][X] + (RADIUS_NODE )*cos( teta ), 2,  makecol( 0, 200, 0 ) );
                //grman::thick_line( bmp_graphe, x, y, x+RADIUS_FLECHE*cos(teta-M_PI/2), y+RADIUS_FLECHE*sin(teta-M_PI/2), 2,  makecol( 0, 200, 0 ) );
            }
        }
    }
    // drawing nodes
    std::string str;//nom du sommet
    str.clear();
    for ( int i = 0; i < ordre; i++ ) {
        str.clear();
        for ( int j = 0; j < groupes_fortements_connexes[i].size(); j++ ) {
            str += std::to_string( groupes_fortements_connexes[i][j] );
            if( j < groupes_fortements_connexes[i].size() - 1 )
                str += ",";
        }
        std::cout << " str: " << str;
        circlefill( bmp_graphe, pos[i][X], pos[i][Y], RADIUS_NODE + 2 * str.size() - 4, makecol( 0, 0, 0 ) );
        textprintf_centre_ex( bmp_graphe, font, pos[i][X], pos[i][Y],
                              makecol( 255, 255, 255 ), 0, "%s", str.c_str() );
    }
    grman::init_popup( bmp_graphe );
}
int Graph::fact ( int valeur ) {
    int resultatFact = 1;
    for( int i = 1; i <= valeur; i++ ) {
        resultatFact *= i;
    }
    return resultatFact;
}

void Graph::go( int offset, int k, std::vector<int> mes_sommets, std::vector<int> combination, std::vector<std::vector<int>> &vectordepossibiliteksommet, int &nombre ) {
    if ( k == 0 ) {
        //std::cout << "combination no " << nombre << ": [ ";
        for ( int i = 0; i < combination.size(); ++i ) {
            //std::cout << combination[i] << " ";
            vectordepossibiliteksommet[nombre].push_back( combination[i] );
        }
        //std::cout << "] " << std::endl;
        nombre = nombre + 1;
        return;
    }
    for ( int i = offset; i <= mes_sommets.size() - k; ++i ) {
        combination.push_back( mes_sommets[i] );
        go( i + 1, k - 1, mes_sommets, combination, vectordepossibiliteksommet, nombre );
        combination.pop_back();
    }
}
std::vector<std::vector<int>> Graph::kplet( int &compte ) {
    int kmin = -1;
    std::map<int, bool> marquage;
    std::vector<std::vector<int>> resultat_numsommet;
    std::vector<std::vector<int>> vectordepossibiliteksommet;
    std::vector <int> cles_int_sommet;
    std::vector<int> combination;
    resultat_numsommet.resize( m_vertices.size() );
    for ( auto &elem : m_vertices ) { // Remplissage cles_int_sommet
        cles_int_sommet.push_back( elem.first );
    }
    for ( int k = 2; k < m_vertices.size(); k++ ) {
        vectordepossibiliteksommet.clear(); //On vide les possibilité
        combination.clear();
        // Creation des possibilité avec k sommet
        int bin = 0;
        bin = fact( m_vertices.size() ) / ( fact( k ) * fact( m_vertices.size() - k ) ); // On calcule le nombre de possibilité
        std::cout << "Nombre de possibilité de sommet : " << bin << std::endl;
        vectordepossibiliteksommet.resize( bin );
        int a = 0;
        go( 0, k, cles_int_sommet, combination, vectordepossibiliteksommet, a ); // Remplissage vectorpossibilite sommet de taille bin * k
        for ( int x = 0; x < bin; x++ ) { // Affichage de vecteur de possibilité
            for ( auto &elem : vectordepossibiliteksommet[x] ) {
                std::cout << elem << " " ;
            }
            std::cout << std::endl;
        }
        for ( int x = 0; x < bin; x++ ) { // on check si le fait de neutraliser ces k sommet déconnecte le graphe
            //std::cout << "Debut" << std::endl;
            for ( auto &elem : m_vertices ) { // Mettre a false tous les sommets
                marquage[elem.first] = false;
            }
            for ( auto &elem : vectordepossibiliteksommet[x] ) {
                std::map<int, bool>::iterator it;
                it = marquage.find( elem );
                if ( it != marquage.end() ) { // On met a "true" chaque couple
                    it->second = true;
                    //std::cout << it->first << ". on passe ici" << std::endl;
                }
            }
            //std::cout << "FIN" << std::endl;
            if( k > 0 ) {
                //std::cout << "K = " << k << std::endl;
                //std::cout << "X = " << x+1 << std::endl;
                if( connexite( marquage, compte ) == false ) {
                    kmin = k;
                    for ( auto &elem : marquage ) {
                        //std::cout << "compte ?" << compte << " " ;
                        if ( elem.second == true ) {
                            resultat_numsommet[compte].push_back( elem.first );
                        }
                    }
                    compte = compte + 1;
                }
            }
            std::cout << std::endl;
        }
        if ( kmin != -1 ) { // Si on a trouver un kmin alors on sort de la boucle
            k = m_vertices.size() + 1;
            //std::cout << "Solution : " << std::endl;
            std::cout << std::endl;
        }
    }
    std::cout << "------ SOLUTION ------" << std::endl << std::endl;
    for ( int i = 0; i < compte; i++ ) {
        std::cout << "Couple " << i << " de sommet a enlever afin de deconnecter le graphe : " << std::endl;
        for ( int j = 0; j < resultat_numsommet[i].size(); j++ ) {
            std::cout << resultat_numsommet[i][j] << " " ;
        }
        std::cout << std::endl;
    }
    return resultat_numsommet;
}


bool Graph::connexite( std::map<int, bool> marquage, int compte ) {
    bool connexe = true;
    std::vector<int> numero_newgraphe;
    std::vector<std::vector<int>> quelsommetestrelier;
    std::vector<int> sommetrestant;
    std::vector<int> couple;
    for ( auto &elem : marquage ) { // On met la cles de tous les sommets ne faisant pas parti du couple dans un tableau
        if ( elem.second == false ) {
            numero_newgraphe.push_back( elem.first );
        } else if ( elem.second == true ) {
            couple.push_back( elem.first );
            std::cout << "on enleve : " << elem.first << std::endl;
        }
    }   // On a maintenant deux tableaux remplis "numero_newgraphe" -> Avec les sommets toujours present
    // et "couple" -> le couple qu'on enleve
    quelsommetestrelier.clear();
    quelsommetestrelier.resize( couple.size() );
    for( auto &elem : m_edges ) {
        for( int i = 0; i < couple.size(); i++ ) {
            if ( elem.second.m_to == couple[i] || elem.second.m_from == couple[i] ) {
            } else {
                quelsommetestrelier[i].push_back( elem.second.m_from );
                quelsommetestrelier[i].push_back( elem.second.m_to );
            }
        }
    }
    // std::cout << "quelsommetestrelier" << std::endl;
    for ( int i = 0; i < couple.size(); i++ ) {
        for ( auto &elem : quelsommetestrelier[i] ) {
            //   std::cout << " "<< elem << " ";
        }
        std::cout << std::endl;
    }
    //Tableau a fusionner et prendre que les nombres presents dans tous les tabeaux
//std::cout << " Nombre de tableau : "<< quelsommetestrelier.size() << std::endl;
    for ( auto &elem : m_vertices ) {
        int compteur = 0;
        for ( int a = 0; a < quelsommetestrelier.size(); a++ ) {
            for( int b = 0; b < quelsommetestrelier[a].size(); b++ ) {
                if( elem.first == quelsommetestrelier[a][b] ) {
                    compteur = compteur + 1;
                    b = quelsommetestrelier[a].size() + 1;
                }
            }
        }
        if( compteur == couple.size() ) {
            sommetrestant.push_back( elem.first );
        }
    }
    std::cout << std::endl;
    if ( couple.size() != sommetrestant.size() ) {
        if( m_vertices.size() - couple.size() > sommetrestant.size() ) {
            connexe = false;
            std::cout << "Ceci sont les composantes connexe quaud on prend le couple de sommet en question" << std::endl;
            for ( auto &elem : sommetrestant ) {
                std::cout << " " << elem << " ";
            }
            std::cout << std::endl << std::endl;
        }
    }
    return connexe;
}
