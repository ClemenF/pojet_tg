#include "graph.h"
#include <sstream>
#include <string>


/***************************************************
                    VERTEX
****************************************************/

/// Le constructeur met en place les ï¿½lï¿½ments de l'interface
VertexInterface::VertexInterface(int idx, int x, int y, std::string pic_name, int pic_idx)
{
    // La boite englobante
    m_top_box.set_pos(x, y);
    m_top_box.set_dim(130, 100);
    m_top_box.set_moveable();

    // Le slider de rï¿½glage de valeur
    m_top_box.add_child( m_slider_value );
    m_slider_value.set_range(0.0 , 100.0); // Valeurs arbitraires, ï¿½ adapter...
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


/// Gestion du Vertex avant l'appel ï¿½ l'interface
void Vertex::pre_update()
{
    if (!m_interface)
        return;

    /// Copier la valeur locale de la donnï¿½e m_value vers le slider associï¿½
    m_interface->m_slider_value.set_value(m_value);

    /// Copier la valeur locale de la donnï¿½e m_value vers le label sous le slider
    m_interface->m_label_value.set_message( std::to_string( (int)m_value) );
}


/// Gestion du Vertex aprï¿½s l'appel ï¿½ l'interface
void Vertex::post_update()
{
    if (!m_interface)
        return;

    /// Reprendre la valeur du slider dans la donnï¿½e m_value locale
    m_value = m_interface->m_slider_value.get_value();
}



/***************************************************
                    EDGE
****************************************************/

/// Le constructeur met en place les ï¿½lï¿½ments de l'interface
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

    // Une boite pour englober les widgets de rï¿½glage associï¿½s
    m_top_edge.add_child(m_box_edge);
    m_box_edge.set_dim(24,60);
    m_box_edge.set_bg_color(BLANCBLEU);

    // Le slider de rï¿½glage de valeur
    m_box_edge.add_child( m_slider_weight );
    m_slider_weight.set_range(0.0 , 100.0); // Valeurs arbitraires, ï¿½ adapter...
    m_slider_weight.set_dim(16,40);
    m_slider_weight.set_gravity_y(grman::GravityY::Up);

    // Label de visualisation de valeur
    m_box_edge.add_child( m_label_weight );
    m_label_weight.set_gravity_y(grman::GravityY::Down);

}


/// Gestion du Edge avant l'appel ï¿½ l'interface
void Edge::pre_update()
{
    if (!m_interface)
        return;

    /// Copier la valeur locale de la donnï¿½e m_weight vers le slider associï¿½
    m_interface->m_slider_weight.set_value(m_weight);

    /// Copier la valeur locale de la donnï¿½e m_weight vers le label sous le slider
    m_interface->m_label_weight.set_message( std::to_string( (int)m_weight ) );
}

/// Gestion du Edge aprï¿½s l'appel ï¿½ l'interface
void Edge::post_update()
{
    if (!m_interface)
        return;

    /// Reprendre la valeur du slider dans la donnï¿½e m_weight locale
    m_weight = m_interface->m_slider_weight.get_value();
}



/***************************************************
                    GRAPH
****************************************************/

/// Ici le constructeur se contente de prï¿½parer un cadre d'accueil des
/// ï¿½lï¿½ments qui seront ensuite ajoutï¿½s lors de la mise ne place du Graphe
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
}


/// Mï¿½thode spï¿½ciale qui construit un graphe arbitraire (dï¿½mo)
/// Cette mï¿½thode est ï¿½ enlever et remplacer par un systï¿½me
/// de chargement de fichiers par exemple.
/// Bien sï¿½r on ne veut pas que vos graphes soient construits
/// "ï¿½ la main" dans le code comme ï¿½a.
void Graph::make_example()
{
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);
    // La ligne prï¿½cï¿½dente est en gros ï¿½quivalente ï¿½ :
    // m_interface = new GraphInterface(50, 0, 750, 600);

    /// Les sommets doivent ï¿½tre dï¿½finis avant les arcs
    // Ajouter le sommet d'indice 0 de valeur 30 en x=200 et y=100 avec l'image clown1.jpg etc...
    add_interfaced_vertex(0, 30.0, 200, 100, "clown1.jpg");
    add_interfaced_vertex(1, 60.0, 400, 100, "clown2.jpg");
    add_interfaced_vertex(2,  50.0, 200, 300, "clown3.jpg");
    add_interfaced_vertex(3,  0.0, 400, 300, "clown4.jpg");
    add_interfaced_vertex(4,  100.0, 600, 300, "clown5.jpg");
    add_interfaced_vertex(5,  0.0, 100, 500, "bad_clowns_xx3xx.jpg", 0);
    add_interfaced_vertex(6,  0.0, 300, 500, "bad_clowns_xx3xx.jpg", 1);
    add_interfaced_vertex(7,  0.0, 500, 500, "bad_clowns_xx3xx.jpg", 2);

    /// Les arcs doivent ï¿½tre dï¿½finis entre des sommets qui existent !
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


/// La mï¿½thode update ï¿½ appeler dans la boucle de jeu pour les graphes avec interface

/// La méthode sauvegarde le graphe dans un fichier numéroté
void Graph::graphe_sauvegarde()
{


}

/// La méthode charge un fichier pour remplir les données d'un graphe
void Graph::graphe_chargement()
{
    std::cout << " ------ debut chargement graphe "<< m_numero_graphe << " ------- " << std::endl;
    std::string fichier = "_sauvegarde.txt";
    fichier = std::to_string(m_numero_graphe) + fichier;
    std::cout << fichier << std::endl;

    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);

    std::ifstream ifs(fichier.c_str());
    if(ifs)
    {
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
    m_Timer.draw();
    if(keypressed())std::cout << readkey() <<std::endl;
    if(grman::key_press[KEY_RIGHT])std::cout << " touche appuye " << std::endl;
    if(grman::key_unpress[KEY_RIGHT])std::cout << " touche relache " << std::endl;

    for (auto &elt : m_vertices)
        elt.second.post_update();

    for (auto &elt : m_edges)
        elt.second.post_update();

}

/// Aide ï¿½ l'ajout de sommets interfacï¿½s
void Graph::add_interfaced_vertex(int idx, double value, int x, int y, std::string pic_name, int pic_idx )
{
    if ( m_vertices.find(idx)!=m_vertices.end() )
    {
        std::cerr << "Error adding vertex at idx=" << idx << " already used..." << std::endl;
        throw "Error adding vertex";
    }
    // Crï¿½ation d'une interface de sommet
    VertexInterface *vi = new VertexInterface(idx, x, y, pic_name, pic_idx);
    // Ajout de la top box de l'interface de sommet
    m_interface->m_main_box.add_child(vi->m_top_box);
    // On peut ajouter directement des vertices dans la map avec la notation crochet :
    m_vertices[idx] = Vertex(value, vi);
}

/// Aide ï¿½ l'ajout d'arcs interfacï¿½s
void Graph::add_interfaced_edge(int idx, int id_vert1, int id_vert2, double weight)
{
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
    m_vertices[id_vert1].m_out.push_back(idx);
    m_vertices[id_vert2].m_in.push_back(idx);
}


//algo composantes connexes
void toutesLesComposanteFortementConnexe(int s)
{
  std::vector<std::vector<int>> tabc; //tab des composantes fortements connexes
  std::vector<int> marques;//tab de marquages
  int x,y;//sommets intermediaires

  /**allocation memoire de tabc et marques**/
  tabc.resize(ordre);
  for (int i = 0; i < ordre; i++) {
    tabc.at(i).resize(ordre);
  }
  marques.resize(ordre);

//init a 0 des deux tabs
  for (int i = 0; i < ordre; i++) {
    marques.at(i)=0;
    for (int j = 0; j < ordre; j++) {
      tabc.at(i).at(j)=0;
    }
  }
  for ( x = 0; x < ordre; x++) {
    if (!marques[x]){
      tabc[x]=uneComposanteFortementConnexe(x);
      marques[x]=1;
      for (y = 0; y < ordre; y++) {
        if (tabc[x][y] && !marques[y])
          marques[y]=1;
      }
    }
  }
}
std::vector<int> uneComposantesFortementConnexes()
{
  std::vector<int> c1,c2,c,marques;
  int x,y;
  int ajoute = 1;


  //allocation memoire
  c1.resize(ordre);
  c2.resize(ordre);
  c.resize(ordre);
  marques.resize(ordre);

  //init valeur a 0
  for ( int i = 0 ; i < ordre ; i++ ) {
     c1.at( i ) = 0 ;
     c2.at( i ) = 0 ;
     c.at( i ) = 0 ;
     marques.at(i) = 0 ;
  }
  //on rend le sommet s connexe
   c1[s]=1;
   c2[s]=1;
   //recherche des composantes connexes partant de s a ajouter a c1
   while (ajoute) {
      ajoute = 0;
      for (x = 0; x < ordre; x++) {
         if (!marques[x] && c1[x]){
            marques[x] =1;
            for ( y = 0; y < ordre ; y++ ) {
               if adjacence[x][y] && !marques[y]){
                  c1[y] = 1 ;
                  ajoute = 1 ;
               }
            }
         }
      }
   }

   for ( int i = 0 ; i < ordre ; i++ ) {
      marques.at(i) = 0 ;
   }
   //recherche des composantes connexes arrivant a s a ajouter dans c2
   while (ajoute) {
      ajoute = 0;
      for (x = 0 ; x < ordre ; x++ ) {
         if ( !marques[x] && c2[x] ){
            marques[x] = 1 ;
            for ( y = 0 ; y < ordre ; y++ ) {
               if ( adjacence[y][x] && !marques[y]){
                  c2[y] = 1 ;
                  ajoute = 1 ;
               }
            }
         }
      }
   }
   //composante fortement connexe c =intersection de c1 et c2
   for (x=0 ; x < ordre ; x++){
      c[x] = c1[x] & c2[x];
   }
   return c;
}
