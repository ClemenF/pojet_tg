#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
#include <iostream>
#include <regex>


#include "grman.h"
#include <allegro.h>
#include <png.h>
#include <loadpng.h>
#include <jpgalleg.h>

/* maximum number of bytes a single (Unicode) character can have */
#define MAX_BYTES_PER_CHAR 4
/* for the d_edit_proc object */
#define LEN 32
char the_string[(LEN + 1) * MAX_BYTES_PER_CHAR] = "Edit Text";

namespace grman
{


// Permet de compiler en mode plein �cran (d�commenter)
//#define MODE_GRAPHIQUE GFX_AUTODETECT_WINDOWED
#define MODE_GRAPHIQUE GFX_AUTODETECT_FULLSCREEN


// Nombre de pixels horizontalement et verticalement
#define LARGEURECRAN 1024
#define HAUTEURECRAN 768

// Couleur initiale :
#define PAGE_COULEUR_INIT BLANC

// Temporisation de la boucle de jeu en ms ( rest dans mettre_a_jour )
#define TEMPO_MAJ 20

int page_color = PAGE_COULEUR_INIT;

BITMAP *page = NULL;
BITMAP *popup = NULL;

char key_last;
int mouse_click;
int mouse_unclick;
int key_press[KEY_MAX];
int key_unpress[KEY_MAX];

int mouse_click_x;
int mouse_click_y;

int mouse_move_x;
int mouse_move_y;

Coords mouse_pos;

Frame page_frame;
Widget *gui_over = nullptr;
Widget *gui_last_over = nullptr;
Widget *gui_focus = nullptr;
Widget *gui_leave = nullptr;


/// Gestion des contenus graphiques (fichiers images)
std::unordered_map<std::string, BITMAP *> g_pic_names;
std::unordered_set<std::string> g_failed_pic_names;
std::string g_pic_path;


unsigned get_picture_nb( std::string name )
{
    std::string delim = "xx";
    std::smatch m;
    if ( std::regex_search( name, m, std::regex( delim + "([0-9]+)" + delim ) ) )
    {
        unsigned nb = strtoul( m[1].str().c_str(), NULL, 10 );
        return nb ? nb : 1;
    }
    return 1;
}


BITMAP *get_picture( std::string pic_name )
{
    pic_name = g_pic_path + pic_name;
    BITMAP *pic = nullptr;
    auto pr = g_pic_names.find( pic_name );
    if ( pr != g_pic_names.end() )
        pic = pr->second;
    else
    {
        pic = load_bitmap( pic_name.c_str(), NULL );
        if ( pic )
        {
            g_pic_names.insert( std::make_pair( pic_name, pic ) );
            std::cout << "Loaded " << pic_name << std::endl;
        }
        else
        {
            auto it = g_failed_pic_names.find( pic_name );
            if ( it == g_failed_pic_names.end() )
            {
                g_failed_pic_names.insert( pic_name );
                std::cout << "COULD NOT LOAD " << pic_name << std::endl;
            }
            return nullptr;
        }
    }
    return pic;
}

void show_picture( BITMAP *dest, std::string pic_name, int x, int y, unsigned pic_idx )
{
    BITMAP *pic = get_picture( pic_name );
    if ( !pic )
        return;
    unsigned nb = get_picture_nb( pic_name );
    if ( nb == 1 )
        draw_sprite( dest, pic, x, y );
    else
        masked_blit( pic, dest, ( pic_idx % nb )*pic->w / nb, 0, x, y, pic->w / nb, pic->h );
}

void set_pictures_path( std::string path_name )
{
    if ( path_name[path_name.length() - 1] != '/' )
        path_name += "/";
    g_pic_path = path_name;
}

void mettre_a_jour()
{
    buf_afficher_page();
    rest( TEMPO_MAJ );
    rafraichir_clavier_souris();
    buf_effacer_page();
    /// mouse_unclick && !gui_grabbed  ||  new click select || capture left (key ENTER or KEY ESC) ?
    if ( mouse_unclick )
    {
        gui_leave = gui_focus;
        gui_focus = nullptr;
    }
    else
        gui_leave = nullptr;
    gui_last_over = gui_over;
    gui_over = gui_focus;
}

void init()
{
    //if (page)return;
    srand( time( NULL ) );
    /// Gestion des caract�res accentu�s
    set_uformat( U_ASCII );
    allegro_init();
    install_timer();
    install_keyboard();
    set_keyboard_rate( 0, 0 );
    install_mouse();
    register_png_file_type();
    jpgalleg_init();
    set_color_depth( desktop_color_depth() );
    if ( set_gfx_mode( MODE_GRAPHIQUE, LARGEURECRAN, HAUTEURECRAN, 0, 0 ) != 0 )
    {
        if ( set_gfx_mode( GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0 ) != 0 )
        {
            allegro_message( "prb gfx mode" );
            allegro_exit();
            exit( EXIT_FAILURE );
        }
    }
    set_display_switch_mode( SWITCH_BACKGROUND );
    show_mouse( screen );
    page = create_bitmap( SCREEN_W, SCREEN_H );
    rafraichir_clavier_souris();
    buf_effacer_page();
    buf_afficher_page();
    page_frame.pos.x = 0;
    page_frame.pos.y = 0;
    page_frame.dim.x = SCREEN_W;
    page_frame.dim.y = SCREEN_H;
}

void init_popup(BITMAP* bmp_popup)//popup qui reçoit en param une bitmap a afficher
{
    DIALOG the_dialog[] =
    {
        /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1) (d2)  (dp)           (dp2) (dp3) */
        { d_clear_proc,      0,    0,    0,    0,    255,  0,    0,    0,       0,   0,    NULL,          NULL, NULL  },

        // { d_check_proc,      12,   12,   161,  49,   255,  0,    't',  0,       0,   0,   (void*)  "&Toggle Me",  NULL, NULL  },
        { d_bitmap_proc,     0,  0,   1024,   700,   0,  0,    0,      0,       0,   0,    NULL,                   NULL, NULL  },


        { d_button_proc,     462,  700,  100,  40,   255,  0,    0,    D_EXIT,  0,   0,  ( void* )  "Exit",        NULL, NULL  },
        { NULL,              0,    0,    0,    0,    0,    0,    0,    0,       0,   0,    NULL,          NULL, NULL  }
    };
//dialog avec bouton exit et image
   // the_dialog[1].dp = load_bitmap( "clown1.jpg", NULL );
   the_dialog[1].dp = bmp_popup;
    do_dialog( the_dialog, -1 );
}

void init_EditText(BITMAP* bmp_popup,char the_text[])//dialog edit text
{
    DIALOG the_dialog[] =
    {
        /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1) (d2)  (dp)           (dp2) (dp3) */
        { d_clear_proc,      0,    0,    0,    0,    255,  0,    0,    0,       0,   0,    NULL,          NULL, NULL  },
        { d_bitmap_proc,     0,    0,  1024,  720,      0,  0,    0,    0,       0,   0,    NULL,                   NULL, NULL  },
        // { d_check_proc,      12,   12,   161,  49,   255,  0,    't',  0,       0,   0,   (void*)  "&Toggle Me",  NULL, NULL  },
        { d_textbox_proc,   30,   550,  100,   50,     255,    0,    0,      0,       0,   0,    (void *)the_text,       NULL, NULL  },
       { d_edit_proc,       30,  600,  100,   30,   0,  255,    0,      0,     LEN,   0,    the_string,            NULL, NULL  },


        { d_button_proc,     30,  640,  100,  40,   255,  0,    0,    D_EXIT,  0,   0,  ( void* )  "OK",        NULL, NULL  },
        { NULL,              0,    0,    0,    0,    0,    0,    0,    0,       0,   0,    NULL,          NULL, NULL  }
    };
//dialog avec bouton exit et edit txt
the_dialog[1].dp = bmp_popup;
    do_dialog( the_dialog, -1 );
}

//{ d_edit_proc,       160, 130,  160,    8,   0,  0,    0,      0,     LEN,   0,    the_string,             NULL, NULL  },

void buf_afficher_popup()
{
    if ( !popup ) return;
    acquire_screen();
    blit( popup, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
    release_screen();
}

void fermer_popup()
{
    if ( !popup ) return;
    destroy_bitmap( popup );
    popup = NULL;
    allegro_exit();
}

void fermer_allegro()
{
    if ( !page ) return;
    destroy_bitmap( page );
    page = NULL;
    allegro_exit();
}

void buf_effacer_page()
{
    if ( !page ) return;
    clear_to_color( page, page_color );
}

void buf_afficher_page()
{
    if ( !page ) return;
    acquire_screen();
    blit( page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
    release_screen();
}



void rafraichir_clavier_souris()
{
    static int mouse_prev, mouse_now;
    static int key_prev[KEY_MAX], key_now[KEY_MAX];
    int k;
    mouse_prev = mouse_now;
    mouse_now = mouse_b;
    mouse_click = mouse_now & ~mouse_prev;
    mouse_unclick = ~mouse_now & mouse_prev;
    mouse_pos = Coords( mouse_x, mouse_y );
    for ( k = 0; k < KEY_MAX; k++ )
    {
        key_prev[k] = key_now[k];
        key_now[k] = key[k];
        key_press[k] = key_now[k] && !key_prev[k];
        key_unpress[k] = !key_now[k] && key_prev[k];
    }
    key_last = '\0';
    while ( keypressed() )
        key_last = ( char )readkey();
    get_mouse_mickeys( &mouse_move_x, &mouse_move_y );
    if ( mouse_click )
    {
        mouse_click_x = mouse_x;
        mouse_click_y = mouse_y;
    }
}


/// Auxiliaires : compl�ments aux fonctions graphiques allegro

void thick_line( BITMAP *bmp, int x1, int y1, int x2, int y2, int thickness, int color )
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    thickness = std::max( thickness, 1 );
    if ( abs( dx ) > abs( dy ) )
        for ( int i = 1 - thickness; i < thickness; ++i )
            line( bmp, x1, y1 + i, x2, y2 + i, color );
    else
        for ( int i = 1 - thickness; i < thickness; ++i )
            line( bmp, x1 + i, y1, x2 + i, y2, color );
}


} /// FIN DE NAMESPACE GRMAN
