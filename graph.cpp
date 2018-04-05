#include "graph.h"
#include <sstream>
#include <string>


/***************************************************
                    VERTEX
****************************************************/

/// Le constructeur met en place les éléments de l'interface
VertexInterface::VertexInterface(int idx, int x, int y, std::string pic_name, int pic_idx)
{
    // La boite englobante
    m_top_box.set_pos(x, y);
    m_top_box.set_dim(130, 100);
    m_top_box.set_moveable();

    // Le slider de réglage de valeur
    m_top_box.add_child( m_slider_value );
    m_slider_value.set_range(0.0 , 100.0); // Valeurs arbitraires, à adapter...
    m_slider_value.set_dim(20,80);
    m_slider_value.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);

    // Label de visualisation de valeur
    m_top_box.add_child( m_label_value );
    m_label_value.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Down);

    // Une illustration...
    if (pic_name!="")
    {
        m_top_box.add_child( m_img );
        m_img.set_pic_name(pic_name);
        m_img.set_pic_idx(pic_idx);
        m_img.set_gravity_x(grman::GravityX::Right);
    }

    // Label de visualisation d'index du sommet dans une boite
    m_top_box.add_child( m_box_label_idx );
    m_box_label_idx.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Down);
    m_box_label_idx.set_dim(20,12);
    m_box_label_idx.set_bg_color(BLANC);

    m_box_label_idx.add_child( m_label_idx );
    m_label_idx.set_message( std::to_string(idx) );
}


/// Gestion du Vertex avant l'appel à l'interface
void Vertex::pre_update()
{
    if (!m_interface)
        return;

    /// Copier la valeur locale de la donnée m_value vers le slider associé
    m_interface->m_slider_value.set_value(m_value);

    /// Copier la valeur locale de la donnée m_value vers le label sous le slider
    m_interface->m_label_value.set_message( std::to_string( (int)m_value) );
}


/// Gestion du Vertex après l'appel à l'interface
void Vertex::post_update()
{
    if (!m_interface)
        return;

    /// Reprendre la valeur du slider dans la donnée m_value locale
    m_value = m_interface->m_slider_value.get_value();
}



/***************************************************
                    EDGE
****************************************************/

/// Le constructeur met en place les éléments de l'interface
EdgeInterface::EdgeInterface(Vertex& from, Vertex& to)
{
    // Le WidgetEdge de l'interface de l'arc
    if ( !(from.m_interface && to.m_interface) )
    {
        std::cerr << "Error creating EdgeInterface between vertices having no interface" << std::endl;
        throw "Bad EdgeInterface instanciation";
    }
    m_top_edge.attach_from(from.m_interface->m_top_box);
    m_top_edge.attach_to(to.m_interface->m_top_box);
    m_top_edge.reset_arrow_with_bullet();

    // Une boite pour englober les widgets de réglage associés
    m_top_edge.add_child(m_box_edge);
    m_box_edge.set_dim(24,60);
    m_box_edge.set_bg_color(BLANCBLEU);

    // Le slider de réglage de valeur
    m_box_edge.add_child( m_slider_weight );
    m_slider_weight.set_range(0.0 , 100.0); // Valeurs arbitraires, à adapter...
    m_slider_weight.set_dim(16,40);
    m_slider_weight.set_gravity_y(grman::GravityY::Up);

    // Label de visualisation de valeur
    m_box_edge.add_child( m_label_weight );
    m_label_weight.set_gravity_y(grman::GravityY::Down);

}


/// Gestion du Edge avant l'appel à l'interface
void Edge::pre_update()
{
    if (!m_interface)
        return;

    /// Copier la valeur locale de la donnée m_weight vers le slider associé
    m_interface->m_slider_weight.set_value(m_weight);

    /// Copier la valeur locale de la donnée m_weight vers le label sous le slider
    m_interface->m_label_weight.set_message( std::to_string( (int)m_weight ) );
}

/// Gestion du Edge après l'appel à l'interface
void Edge::post_update()
{
    if (!m_interface)
        return;

    /// Reprendre la valeur du slider dans la donnée m_weight locale
    m_weight = m_interface->m_slider_weight.get_value();
}



/***************************************************
                    GRAPH
****************************************************/

/// Ici le constructeur se contente de préparer un cadre d'accueil des
/// éléments qui seront ensuite ajoutés lors de la mise ne place du Graphe
GraphInterface::GraphInterface(int x, int y, int w, int h)
{
    m_top_box.set_dim(1000,740);
    m_top_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);

    m_top_box.add_child(m_tool_box);
    m_tool_box.set_dim(80,720);
    m_tool_box.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);
    m_tool_box.set_bg_color(BLANCBLEU);

    m_top_box.add_child(m_main_box);
    m_main_box.set_dim(908,720);
    m_main_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);
    m_main_box.set_bg_color(BLANCJAUNE);


    int y_bt = 10;
    /// ajout des boutons de gestion du graph.
    /// pour les sommets :
    m_tool_box.add_child(m_text_bt_ajouter_vertex);
    m_text_bt_ajouter_vertex.set_dim(75,10);
    m_text_bt_ajouter_vertex.set_gravity_x(grman::GravityX::Left);
    m_text_bt_ajouter_vertex.set_posy(y_bt);
    m_text_bt_ajouter_vertex.set_bg_color(BLANC);
    m_text_bt_ajouter_vertex.set_message("Add vertex!");
    y_bt = y_bt+m_text_bt_ajouter_vertex.get_dimy();

    m_tool_box.add_child(m_bt_ajouter_vertex);
    m_bt_ajouter_vertex.set_dim(70,15);
    m_bt_ajouter_vertex.set_gravity_x(grman::GravityX::Left);
    m_bt_ajouter_vertex.set_posy(y_bt);
    m_bt_ajouter_vertex.set_bg_color(ROUGECLAIR);
    y_bt = y_bt + m_bt_ajouter_vertex.get_dimy();

    m_tool_box.add_child(m_text_bt_supprimer_vertex);
    m_text_bt_supprimer_vertex.set_dim(75,10);
    m_text_bt_supprimer_vertex.set_gravity_x(grman::GravityX::Left);
    m_text_bt_supprimer_vertex.set_posy(y_bt);
    m_text_bt_supprimer_vertex.set_bg_color(BLANC);
    m_text_bt_supprimer_vertex.set_message("sup vertex!");
    y_bt = y_bt+m_text_bt_supprimer_vertex.get_dimy();

    m_tool_box.add_child(m_bt_supprimer_vertex);
    m_bt_supprimer_vertex.set_dim(70,15);
    m_bt_supprimer_vertex.set_gravity_x(grman::GravityX::Left);
    m_bt_supprimer_vertex.set_posy(y_bt);
    m_bt_supprimer_vertex.set_bg_color(ROUGECLAIR);
    y_bt = y_bt + m_bt_supprimer_vertex.get_dimy();

    y += 20;
    /// pour les arcs :
    m_tool_box.add_child(m_text_bt_ajouter_edge);
    m_text_bt_ajouter_edge.set_dim(75,10);
    m_text_bt_ajouter_edge.set_gravity_x(grman::GravityX::Left);
    m_text_bt_ajouter_edge.set_posy(y_bt);
    m_text_bt_ajouter_edge.set_bg_color(BLANC);
    m_text_bt_ajouter_edge.set_message("add edge !");
    y_bt = y_bt+m_text_bt_ajouter_edge.get_dimy();

    m_tool_box.add_child(m_bt_ajouter_edge);
    m_bt_ajouter_edge.set_dim(70,15);
    m_bt_ajouter_edge.set_gravity_x(grman::GravityX::Left);
    m_bt_ajouter_edge.set_posy(y_bt);
    m_bt_ajouter_edge.set_bg_color(ROUGECLAIR);
    y_bt = y_bt + m_bt_ajouter_edge.get_dimy();

    m_tool_box.add_child(m_text_bt_supprimer_edge);
    m_text_bt_supprimer_edge.set_dim(75,10);
    m_text_bt_supprimer_edge.set_gravity_x(grman::GravityX::Left);
    m_text_bt_supprimer_edge.set_posy(y_bt);
    m_text_bt_supprimer_edge.set_bg_color(BLANC);
    m_text_bt_supprimer_edge.set_message("sup edge !");
    y_bt = y_bt+m_text_bt_supprimer_edge.get_dimy();

    m_tool_box.add_child(m_bt_supprimer_edge);
    m_bt_supprimer_edge.set_dim(70,15);
    m_bt_supprimer_edge.set_gravity_x(grman::GravityX::Left);
    m_bt_supprimer_edge.set_posy(y_bt);
    m_bt_supprimer_edge.set_bg_color(ROUGECLAIR);
    y_bt = y_bt + m_bt_supprimer_edge.get_dimy();

}


/// Méthode spéciale qui construit un graphe arbitraire (démo)
/// Cette méthode est à enlever et remplacer par un système
/// de chargement de fichiers par exemple.
/// Bien sûr on ne veut pas que vos graphes soient construits
/// "à la main" dans le code comme ça.
void Graph::make_example()
{
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);
    // La ligne précédente est en gros équivalente à :
    // m_interface = new GraphInterface(50, 0, 750, 600);

    /// Les sommets doivent être définis avant les arcs
    // Ajouter le sommet d'indice 0 de valeur 30 en x=200 et y=100 avec l'image clown1.jpg etc...
    add_interfaced_vertex(0, 30.0, 200, 100, "clown1.jpg");
    add_interfaced_vertex(1, 60.0, 400, 100, "clown2.jpg");
    add_interfaced_vertex(2,  50.0, 200, 300, "clown3.jpg");
    add_interfaced_vertex(3,  0.0, 400, 300, "clown4.jpg");
    add_interfaced_vertex(4,  100.0, 600, 300, "clown5.jpg");
    add_interfaced_vertex(5,  0.0, 100, 500, "bad_clowns_xx3xx.jpg", 0);
    add_interfaced_vertex(6,  0.0, 300, 500, "bad_clowns_xx3xx.jpg", 1);
    add_interfaced_vertex(7,  0.0, 500, 500, "bad_clowns_xx3xx.jpg", 2);

    /// Les arcs doivent être définis entre des sommets qui existent !
    // AJouter l'arc d'indice 0, allant du sommet 1 au sommet 2 de poids 50 etc...
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
}

/// La méthode sauvegarde le graphe dans un fichier numéroté
void Graph::graphe_sauvegarde()
{
    std::string fichier ="sauvegarde.txt";
    fichier = std::to_string(m_numero_graphe) + fichier;

    std::ofstream ofs(fichier.c_str(),std::ios::out);
    if(ofs)
    {
        ofs << "----- Fichier de sauvegarde du graphe " << m_numero_graphe<<" -----" << std::endl;
        ofs << "\"indice\",\"nom du sommet\",\"valeure du sommet\",\"posx\",\"posy\",\"image\""<< std::endl;
        ofs << m_ordre << std::endl;
        for(auto &elem : m_vertices)
        {
            ofs << elem.second.m_interface->m_label_idx.get_message() <<" "<< elem.second.m_value <<" "<< elem.second.m_interface->m_top_box.get_frame().pos.x <<" "<< elem.second.m_interface->m_top_box.get_frame().pos.y <<" "<< elem.second.m_interface->m_img.get_pic_name() << std::endl;
        }
        ofs << m_nb_arete<< std::endl;
        int i=0;
        for(auto &elem : m_edges)
        {
            i++;
            ofs <<i<<" " << elem.second.m_from <<" "<< elem.second.m_to <<" "<< elem.second.m_weight<<std::endl;
        }
    }
    else
    {
        std::cout << "probleme lors de l'ouverture du fichier de sauvegarde " << m_numero_graphe << std::endl;
    }

}

/// La méthode charge un fichier pour remplir les données d'un graphe
void Graph::graphe_chargement()
{
    std::cout << " ------ debut chargement graphe "<< m_numero_graphe << " ------- " << std::endl;
    std::string fichier = "_sauvegarde.txt";
    fichier = std::to_string(m_numero_graphe) + fichier;

    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    std::ifstream ifs(fichier.c_str());
    if(ifs)
    {
        m_ordre = m_nb_arete = 0;
        int ordre = 0, num_arete = 0;
        std::string line;
        for(int i=0;i<2;i++){
                std::getline(ifs,line);
                std::cout << line << " ignore " << std::endl;
        }
        ifs >> ordre;
        std::cout << ordre << std::endl;
        for(int i=0;i<ordre;i++)
        {
            std::cout << "numero " << i << std::endl;
            int indice,posx,posy,idx=99;
            double valeur;
            std::string image;
            ifs >> indice >> valeur >> posx >> posy >> image >> idx;
            if(idx==99)add_interfaced_vertex(indice,valeur,posx,posy,image.c_str());
            else add_interfaced_vertex(indice,valeur,posx,posy,image.c_str(),idx);
            std::cout << " sommet numero " << indice << " : ok!" << std::endl;
        }
        ifs >> num_arete;
        for(int i=0;i<num_arete;i++)
        {
            int indice,som1,som2;
            double poids;
            ifs >> indice >> som1 >> som2 >> poids;
            add_interfaced_edge(indice,som1,som2,poids);
            std::cout << " arete numero " << indice << " : ok!" << std::endl;
        }
    }
    else
    {
        std::cout << "probleme lors de l'ouverture du fichier de chargement " << m_numero_graphe << std::endl;
    }
}

/// La méthode pour compléter la matrice d'adjacence
void Graph::matrice_adjacent()
{
    m_matrice.resize(m_vertices.size(),std::vector<int>(m_vertices.size(),0));

    for(auto &elem : m_edges)
    {
        m_matrice[elem.second.m_from][elem.second.m_to] = 1;
    }

    for(unsigned int i=0;i<m_vertices.size();i++)
    {
        for(unsigned int j=0;j<m_vertices.size();j++)
        {
            std::cout << m_matrice[i][j] << " " ;
                //std::cout << j;
        }
        std::cout<<std::endl;
    }
}

/// La méthode update à appeler dans la boucle de jeu pour les graphes avec interface
void Graph::update()
{
    if (!m_interface)
        return;

    for (auto &elt : m_vertices)
        elt.second.pre_update();

    for (auto &elt : m_edges)
        elt.second.pre_update();

    m_interface->m_top_box.update();

    bouton_ajouter_vertex();
    bouton_supprimer_vertex();
    bouton_ajouter_edge();
    bouton_supprimer_edge();

    m_Timer.draw();

    for (auto &elt : m_vertices)
        elt.second.post_update();

    for (auto &elt : m_edges)
        elt.second.post_update();

}

/// Aide à l'ajout de sommets interfacés
void Graph::add_interfaced_vertex(int idx, double value, int x, int y, std::string pic_name, int pic_idx )
{
    m_ordre++;
    if ( m_vertices.find(idx)!=m_vertices.end() )
    {
        std::cerr << "Error adding vertex at idx=" << idx << " already used..." << std::endl;
        throw "Error adding vertex";
    }
    // Création d'une interface de sommet
    VertexInterface *vi = new VertexInterface(idx, x, y, pic_name, pic_idx);
    // Ajout de la top box de l'interface de sommet
    m_interface->m_main_box.add_child(vi->m_top_box);
    // On peut ajouter directement des vertices dans la map avec la notation crochet :
    m_vertices[idx] = Vertex(value, vi);
}

/// Aide à l'ajout d'arcs interfacés
void Graph::add_interfaced_edge(int idx, int id_vert1, int id_vert2, double weight)
{
    m_nb_arete++;
    if ( m_edges.find(idx)!=m_edges.end() )
    {
        std::cerr << "Error adding edge at idx=" << idx << " already used..." << std::endl;
        throw "Error adding edge";
    }

    if ( m_vertices.find(id_vert1)==m_vertices.end() || m_vertices.find(id_vert2)==m_vertices.end() )
    {
        std::cerr << "Error adding edge idx=" << idx << " between vertices " << id_vert1 << " and " << id_vert2 << " not in m_vertices" << std::endl;
        throw "Error adding edge";
    }

    EdgeInterface *ei = new EdgeInterface(m_vertices[id_vert1], m_vertices[id_vert2]);
    m_interface->m_main_box.add_child(ei->m_top_edge);
    m_edges[idx] = Edge(weight, ei);

    m_edges[idx].m_from = id_vert1;
    m_edges[idx].m_to = id_vert2;
    m_vertices[id_vert1].m_out.push_back(id_vert2);
    m_vertices[id_vert2].m_in.push_back(id_vert1);
}

/// Méthode pour effacer un sommet
void Graph::remove_vertex(int index)
{
    Vertex &remve=m_vertices.at(index);
    std::cout << "Removing vertex " << index << std::endl;

    ///suppression des arcs arrivant dans le sommet index
    for(auto &elem : remve.m_in)
    {
        std::cout << "arc arrivant dans "<< index << " " << elem << std::endl;
        for(auto &elemedge : m_edges)
        {
            ///test pour trouver l'index de l'arc qui va du sommet elem vers notre sommet index
            if(elemedge.second.m_from == elem && elemedge.second.m_to == index)
            {
                remove_edge(elemedge.first);
            }
        }
    }

    ///suppression des arcs partant du sommet index
    for(auto &elem : remve.m_out)
    {
        std::cout << "arc partant de "<< index << " " << elem << std::endl;
        for(auto &elemedge : m_edges)
        {
            ///test pour trouver l'index de l'arc qui va de notre sommet index vers le sommet elem
            if(elemedge.second.m_from == index && elemedge.second.m_to == elem)
            {
                remove_edge(elemedge.first);
            }
        }
    }

    ///maintenant qu'il n'y a plus d'arc connecté à notre sommet on peut le supprimer
    /// test : on a bien des éléments interfacés
    if(m_interface && remve.m_interface)
    {
        m_interface->m_main_box.remove_child( remve.m_interface->m_top_box );
    }

    /// aucune information à supprimer dans les autres sommets vu qu'ils n'ont pas l'indice de notre sommet index
    /// suppression du sommet du vecteur vertice
    m_vertices.erase(index);
    m_ordre--;
}

/// effacer un arc
/// eidx index of edge to remove
void Graph::remove_edge(int eidx)
{
    /// référence vers le Edge à enlever
    Edge &remed=m_edges.at(eidx);
    std::cout << "Removing edge " << eidx << " " << remed.m_from << "->" << remed.m_to << " " << remed.m_weight << std::endl;
    /// Tester la cohérence : nombre d'arc entrants et sortants des sommets 1 et 2
    //std::cout << m_vertices[remed.m_from].m_in.size() << " " << m_vertices[remed.m_from].m_out.size() << std::endl;
    //std::cout << m_vertices[remed.m_to].m_in.size() << " " << m_vertices[remed.m_to].m_out.size() << std::endl;
    //std::cout << m_edges.size() << std::endl;
    /// test : on a bien des éléments interfacés
    if (m_interface && remed.m_interface)
    {
        /// Ne pas oublier qu'on a fait ça à l'ajout de l'arc :
        /* EdgeInterface *ei = new EdgeInterface(m_vertices[id_vert1], m_vertices[id_vert2]); */
        /* m_interface->m_main_box.add_child(ei->m_top_edge); */
        /* m_edges[idx] = Edge(weight, ei); */
        /// Le new EdgeInterface ne nécessite pas de delete car on a un shared_ptr
        /// Le Edge ne nécessite pas non plus de delete car on n'a pas fait de new (sémantique par valeur)
        /// mais il faut bien enlever le conteneur d'interface m_top_edge de l'arc de la main_box du graphe
        m_interface->m_main_box.remove_child( remed.m_interface->m_top_edge );
    }
    /// Il reste encore à virer l'arc supprimé de la liste des entrants et sortants des 2 sommets to et from !
    /// References sur les listes de edges des sommets from et to
    std::vector<int> &vefrom = m_vertices[remed.m_from].m_out;
    std::vector<int> &veto = m_vertices[remed.m_to].m_in;
    vefrom.erase( std::remove( vefrom.begin(), vefrom.end(), eidx ), vefrom.end() );
    veto.erase( std::remove( veto.begin(), veto.end(), eidx ), veto.end() );
    /// Le Edge ne nécessite pas non plus de delete car on n'a pas fait de new (sémantique par valeur)
    /// Il suffit donc de supprimer l'entrée de la map pour supprimer à la fois l'Edge et le EdgeInterface
    /// mais malheureusement ceci n'enlevait pas automatiquement l'interface top_edge en tant que child de main_box !
    m_edges.erase( eidx );
    m_nb_arete--;
    /// Tester la cohérence : nombre d'arc entrants et sortants des sommets 1 et 2
    //std::cout << m_vertices[remed.m_from].m_in.size() << " " << m_vertices[remed.m_from].m_out.size() << std::endl;
    //std::cout << m_vertices[remed.m_to].m_in.size() << " " << m_vertices[remed.m_to].m_out.size() << std::endl;
    //std::cout << m_edges.size() << std::endl;
}


///Les méthodes pour la gestion du Graph
void Graph::bouton_ajouter_vertex()
{
    if(m_interface->m_bt_ajouter_vertex.clicked())
    {

    }
}

void Graph::bouton_supprimer_vertex()
{
    if(m_interface->m_bt_supprimer_vertex.clicked())
    {

    }
}

void Graph::bouton_ajouter_edge()
{
    if(m_interface->m_bt_ajouter_edge.clicked())
    {

    }
}

void Graph::bouton_supprimer_edge()
{
    if(m_interface->m_bt_supprimer_edge.clicked())
    {

    }
}
