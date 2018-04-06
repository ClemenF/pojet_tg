#include "graph.h"
#include <sstream>
#include <string>

/***************************************************
            DEFINE FOR SPRING MODEL
****************************************************/
#define MIN_X 400 // pour viser un carre au centre de..
#define MAX_X 630 // l'ecran donc des intervalles pour x et y
#define MIN_Y 240
#define MAX_Y 500

#define X 0 // utile pour spring model
#define Y 1 // ce sont les pos dans le vector

#define MAX_X_POPUP 1000 //max x et y du popup
#define MAX_Y_POPUP 700
#define AMORTISSEMENT 0.75 //de base 0.85
#define ATTRACTION 0.02 //de base 0.06
#define REPULSION 300 //de base 200
#define ITERATION 50

#define RADIUS_NODE 25 //rayon de sommets pour spring model

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
    // Le slider de r�glage de valeur
    m_top_box.add_child( m_slider_value );
    m_slider_value.set_range( 0.0, 100.0 ); // Valeurs arbitraires, � adapter...
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
    m_interface->m_slider_value.set_value( m_value );
    /// Copier la valeur locale de la donn�e m_value vers le label sous le
    /// slider
    m_interface->m_label_value.set_message( std::to_string( ( int )m_value ) );
}

/// Gestion du Vertex apr�s l'appel � l'interface
void Vertex::post_update() {
    if ( !m_interface )
        return;
    /// Reprendre la valeur du slider dans la donn�e m_value locale
    m_value = m_interface->m_slider_value.get_value();
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
    m_slider_weight.set_range( 0.0, 100.0 ); // Valeurs arbitraires, � adapter...
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
    m_weight = m_interface->m_slider_weight.get_value();
}

/***************************************************
                    GRAPH
****************************************************/

/// Ici le constructeur se contente de pr�parer un cadre d'accueil des
/// �l�ments qui seront ensuite ajout�s lors de la mise ne place du Graphe
GraphInterface::GraphInterface( int x, int y, int w, int h ) {
    m_top_box.set_dim( 1000, 740 ); //grand cadre 1024:720
    m_top_box.set_gravity_xy( grman::GravityX::Right, grman::GravityY::Up ); //set gravity box
    m_top_box.add_child( m_tool_box ); //ajout d'une view
    m_tool_box.set_dim( 80, 720 ); //dim
    m_tool_box.set_gravity_xy( grman::GravityX::Left, grman::GravityY::Up );
    m_tool_box.set_bg_color( BLANCBLEU );
    m_top_box.add_child( m_main_box ); //ajout graph
    m_main_box.set_dim( 908, 720 );
    m_main_box.set_gravity_xy( grman::GravityX::Right, grman::GravityY::Up );
    m_main_box.set_bg_color( BLANCJAUNE );
}

/// M�thode sp�ciale qui construit un graphe arbitraire (d�mo)
/// Cette m�thode est � enlever et remplacer par un syst�me
/// de chargement de fichiers par exemple.
/// Bien s�r on ne veut pas que vos graphes soient construits
/// "� la main" dans le code comme �a.
void Graph::make_example() {
    m_interface = std::make_shared<GraphInterface>( 50, 0, 750, 600 );
    // La ligne pr�c�dente est en gros �quivalente � :
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
    add_interfaced_edge( 0, 1, 2, 50.0 );
    add_interfaced_edge( 1, 0, 1, 50.0 );
    add_interfaced_edge( 2, 1, 3, 75.0 );
    add_interfaced_edge( 3, 4, 1, 25.0 );
    add_interfaced_edge( 4, 6, 3, 25.0 );
    add_interfaced_edge( 5, 7, 3, 25.0 );
    add_interfaced_edge( 6, 3, 4, 0.0 );
    add_interfaced_edge( 7, 2, 0, 100.0 );
    add_interfaced_edge( 8, 5, 2, 20.0 );
    add_interfaced_edge( 9, 3, 7, 80.0 );
}

/// La m�thode update � appeler dans la boucle de jeu pour les graphes avec
/// interface

/// La m�thode sauvegarde le graphe dans un fichier num�rot�
void Graph::graphe_sauvegarde() {}

/// La m�thode charge un fichier pour remplir les donn�es d'un graphe
void Graph::graphe_chargement() {
    std::cout << " ------ debut chargement graphe " << m_numero_graphe
              << " ------- " << std::endl;
    std::string fichier = "_sauvegarde.txt";
    fichier = std::to_string( m_numero_graphe ) + fichier;
    std::cout << fichier << std::endl;
    m_interface = std::make_shared<GraphInterface>( 50, 0, 750, 600 );
    std::ifstream ifs( fichier.c_str() );
    if ( ifs ) {
        int ordre = 0, num_arete = 0;
        std::string line;
        for ( int i = 0; i < 2; i++ ) {
            std::getline( ifs, line );
            std::cout << line << " ignore " << std::endl;
        }
        ifs >> ordre;
        std::cout << ordre << std::endl;
        for ( int i = 0; i < ordre; i++ ) {
            std::cout << "numero " << i << std::endl;
            int indice, posx, posy, idx = 99;
            double valeur;
            std::string image;
            ifs >> indice >> valeur >> posx >> posy >> image >> idx;
            if ( idx == 99 )
                add_interfaced_vertex( indice, valeur, posx, posy, image.c_str() );
            else
                add_interfaced_vertex( indice, valeur, posx, posy, image.c_str(), idx );
            std::cout << " sommet numero " << indice << " : ok!" << std::endl;
        }
        ifs >> num_arete;
        for ( int i = 0; i < num_arete; i++ ) {
            int indice, som1, som2;
            double poids;
            ifs >> indice >> som1 >> som2 >> poids;
            add_interfaced_edge( indice, som1, som2, poids );
            std::cout << " arete numero " << indice << " : ok!" << std::endl;
        }
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
            std::cout << adjacence[i][j] << " ";
            // std::cout << j;
        }
        std::cout << std::endl;
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
void Graph::add_interfaced_vertex( int idx, double value, int x, int y,
                                   std::string pic_name, int pic_idx ) {
    if ( m_vertices.find( idx ) != m_vertices.end() ) {
        std::cerr << "Error adding vertex at idx=" << idx << " already used..."
                  << std::endl;
        throw "Error adding vertex";
    }
    // Cr�ation d'une interface de sommet
    VertexInterface *vi = new VertexInterface( idx, x, y, pic_name, pic_idx );
    // Ajout de la top box de l'interface de sommet
    m_interface->m_main_box.add_child( vi->m_top_box );
    // On peut ajouter directement des vertices dans la map avec la notation
    // crochet :
    m_vertices[idx] = Vertex( value, vi );
}

/// Aide � l'ajout d'arcs interfac�s
void Graph::add_interfaced_edge( int idx, int id_vert1, int id_vert2,
                                 double weight ) {
    if ( m_edges.find( idx ) != m_edges.end() ) {
        std::cerr << "Error adding edge at idx=" << idx << " already used..."
                  << std::endl;
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
    m_vertices[id_vert1].m_out.push_back( idx );
    m_vertices[id_vert2].m_in.push_back( idx );
}

// algo composantes connexes
std::vector<std::vector<int>> Graph::toutesLesComposanteFortementConnexe() {
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
    std::vector<std::vector<int>> groupes_fortements_connexes;
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
   spring_model( adjacence );
}

void Graph::spring_model( std::vector<std::vector<int>>
                   tabadjacence ) { // matrice d'adjacence pour les aretes
    int ordre = tabadjacence.size();
    /// FORCE D4ATTRACTION ET REPULSION///
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
                //pos[i][j] = rand() % (MAX_X - MIN_X) + MIN_X;
                pos[i][j] = rand() % ( MAX_X - MIN_X ) + MIN_X;
            else
                //pos[i][j] = rand() % (MAX_Y - MIN_Y) + MIN_Y;
                pos[i][j] = rand() % ( MAX_Y - MIN_Y ) + MIN_Y;
        }
    }
    for ( int iteration = 0; iteration < ITERATION; iteration++ ) {
        int v;
        for ( int i = 0; i < ordre; i++ ) { // loop dans les sommets
            v = i;                     //= au sommet en cours
            int u;                          // un autre sommet
            netforce[v][0] = 0;             // mise a 0 de x
            netforce[v][1] = 0;             // mise a 0 de y
            for ( int j = 0; j < ordre; j++ ) { // loop dans les autres sommets
                if ( i != j ) {
                    u = j;
                    // squared distance entre "u" & "v"
                    // ((v.x-u.x)*(v.x-u.x)+(v.y-u.y)*(v.y-u.y))
                    int rsq = ( ( pos[v][X] - pos[u][X] ) * ( pos[v][X] - pos[u][X] ) +
                                ( pos[v][Y] - pos[u][Y] ) * ( pos[v][Y] - pos[u][Y] ) );
                    // counting the repulsion between two vertices
                    netforce[v][X] += REPULSION * ( pos[v][X] - pos[u][X] ) /
                                      rsq; // 200 est une constante a changer si pas OK
                    netforce[v][Y] += REPULSION * ( pos[v][Y] - pos[u][Y] ) / rsq;
                }
            }
            for ( int j = 0; j < ordre; j++ ) { // loop dans les arcs
                if ( tabadjacence[i][j] ) {
                    u = j;
                    /// calcul de l'attraction
                    netforce[v][X] +=
                        ATTRACTION * ( pos[u][X] -
                                 pos[v][X] ); // 0.06 est une constante a changer si pas OK
                    netforce[v][Y] += ATTRACTION * ( pos[u][Y] - pos[v][Y] );
                }
            }
            // calcul de velocite (avec amortissement de 0.85)
            velocity[v][X] = ( velocity[v][X] + netforce[v][X] ) * AMORTISSEMENT;
            velocity[v][Y] = ( velocity[v][Y] + netforce[v][Y] ) * AMORTISSEMENT;
        }
        for ( int i = 0; i < ordre; i++ ) { // actualisation des pos avec les nouvelles
            v = i;
            // if(v.isDragged){ v.x = mouseX; v.y = mouseY; }
            // else { v.x += v.velocity.x; v.y += v.velocity.y; }
            if(pos[v][X] < MAX_X_POPUP-RADIUS_NODE && pos[v][X] > 0+RADIUS_NODE)
            pos[v][X] += velocity[v][X];
            if(pos[v][Y] < MAX_Y_POPUP-RADIUS_NODE && pos[v][Y] > 0+RADIUS_NODE)
            pos[v][Y] += velocity[v][Y];
        }
    }
    ///DRAWING GRAPH ON BITMAP
    BITMAP* bmp_graphe = create_bitmap(1024, 700);
    clear_to_color( bmp_graphe, makecol( 100, 100, 100 ) );
    // drawing edges
    for ( int i = 0; i < ordre; i++ ) {
        for ( int j = 0; j < ordre; j++ ) {
            if( tabadjacence[i][j] ) { //si il y a une arete alors..
                line( bmp_graphe, pos[i][X], pos[i][Y], pos[j][X], pos[j][Y], makecol( 0, 200, 0 ) ); //on trace l'arete
            }
        }
    }
    //drawing nodes
    for ( int i = 0; i < ordre; i++ ) {
        circlefill( bmp_graphe, pos[i][X], pos[i][Y], RADIUS_NODE, makecol( 0, 0, 0 ) );
        textprintf_centre_ex( bmp_graphe, font, pos[i][X], pos[i][Y], makecol( 255, 255, 255 ), 0, "%d", i );
    }
    grman::init_popup(bmp_graphe);
}
