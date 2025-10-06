#include "fontconfig_viewer.h"
#include <iostream>

FontConfigViewer::FontConfigViewer() {
    // Initialize GTK
    gtk_init();
    
    // Create main window
    window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "FontConfig Viewer");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    
    // Connect close signal
    g_signal_connect(window, "close-request", G_CALLBACK(+[](GtkWindow* window, gpointer user_data) {
        gtk_window_destroy(window);
        return TRUE;
    }), NULL);
    
    // Create main container
    GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    // Setup UI components
    setupMenuBar(main_box);
    setupMainContent(main_box);
    connectSignals();
    
    // Load fonts
    loadFonts();
    populateList();
}

FontConfigViewer::~FontConfigViewer() {
    // GTK widgets are automatically destroyed when the window is destroyed
}

void FontConfigViewer::setupMenuBar(GtkWidget* main_box) {
    // Build a simple menubar (File -> Quit)
    GMenu* file_menu = g_menu_new();
    g_menu_append(file_menu, "Quit", "win.quit");
    GMenu* menubar_model = g_menu_new();
    g_menu_append_submenu(menubar_model, "File", G_MENU_MODEL(file_menu));
    GtkWidget* menubar = gtk_popover_menu_bar_new_from_model(G_MENU_MODEL(menubar_model));
    gtk_box_append(GTK_BOX(main_box), menubar);
    g_object_unref(file_menu);
    g_object_unref(menubar_model);

    // Add action group "win" for Quit action
    static const GActionEntry win_actions[] = {
        { "quit", +[](GSimpleAction* action, GVariant* parameter, gpointer user_data) {
            GtkWindow* win = GTK_WINDOW(user_data);
            gtk_window_destroy(win);
        }, NULL, NULL, NULL }
    };
    GSimpleActionGroup* win_group = g_simple_action_group_new();
    g_action_map_add_action_entries(G_ACTION_MAP(win_group), win_actions, G_N_ELEMENTS(win_actions), window);
    gtk_widget_insert_action_group(GTK_WIDGET(window), "win", G_ACTION_GROUP(win_group));
    g_object_unref(win_group);
}

void FontConfigViewer::setupMainContent(GtkWidget* main_box) {
    setupFontList();
    setupDetailsPane();

    // Paned main area: left list, right details
    GtkWidget* paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_hexpand(paned, TRUE);
    gtk_widget_set_vexpand(paned, TRUE);
    gtk_paned_set_start_child(GTK_PANED(paned), scrolled_window);
    gtk_paned_set_end_child(GTK_PANED(paned), detail_box);
    gtk_paned_set_shrink_start_child(GTK_PANED(paned), FALSE);
    gtk_paned_set_shrink_end_child(GTK_PANED(paned), FALSE);
    // Set initial position to ~35% of the default window width (800px)
    gtk_paned_set_position(GTK_PANED(paned), 280);
    gtk_box_append(GTK_BOX(main_box), paned);
}

void FontConfigViewer::setupFontList() {
    // Create scrolled window for the list (left pane)
    scrolled_window = gtk_scrolled_window_new();
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), 
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    // Create list box
    list_box = gtk_list_box_new();
    gtk_widget_set_hexpand(list_box, TRUE);
    gtk_widget_set_vexpand(list_box, TRUE);
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_box), GTK_SELECTION_SINGLE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), list_box);
}

void FontConfigViewer::setupDetailsPane() {
    // Create details pane (right pane)
    detail_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(detail_box, 12);
    gtk_widget_set_margin_end(detail_box, 12);
    gtk_widget_set_margin_top(detail_box, 12);
    gtk_widget_set_margin_bottom(detail_box, 12);
    gtk_widget_set_hexpand(detail_box, TRUE);
    gtk_widget_set_vexpand(detail_box, TRUE);

    GtkWidget* details_title = gtk_label_new("Font Details");
    gtk_widget_set_halign(details_title, GTK_ALIGN_START);
    gtk_label_set_markup(GTK_LABEL(details_title), "<b>Font Details</b>");

    detail_family_label = gtk_label_new("Family: -");
    gtk_widget_set_halign(detail_family_label, GTK_ALIGN_START);

    detail_style_label = gtk_label_new("Style: -");
    gtk_widget_set_halign(detail_style_label, GTK_ALIGN_START);

    detail_file_label = gtk_label_new("File: -");
    gtk_widget_set_halign(detail_file_label, GTK_ALIGN_START);

    gtk_box_append(GTK_BOX(detail_box), details_title);
    gtk_box_append(GTK_BOX(detail_box), detail_family_label);
    gtk_box_append(GTK_BOX(detail_box), detail_style_label);
    gtk_box_append(GTK_BOX(detail_box), detail_file_label);
}

void FontConfigViewer::connectSignals() {
    // Update detail pane on selection
    g_signal_connect(list_box, "row-selected", G_CALLBACK(+[](GtkListBox* box, GtkListBoxRow* row, gpointer user_data) {
        if (!row) return;
        FontConfigViewer* self = static_cast<FontConfigViewer*>(user_data);
        int idx = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(row), "font-index"));
        if (idx < 0 || idx >= static_cast<int>(self->fonts.size())) return;
        const FontInfo& f = self->fonts[idx];
        std::string fam = "Family: " + f.family;
        std::string sty = "Style: " + f.style;
        std::string fil = "File: " + f.file;
        gtk_label_set_text(GTK_LABEL(self->detail_family_label), fam.c_str());
        gtk_label_set_text(GTK_LABEL(self->detail_style_label), sty.c_str());
        gtk_label_set_text(GTK_LABEL(self->detail_file_label), fil.c_str());
    }), this);
}

void FontConfigViewer::loadFonts() {
    // Initialize fontconfig
    FcInit();
    
    // Create a pattern to match all fonts
    FcPattern* pattern = FcPatternCreate();
    FcObjectSet* object_set = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FILE, NULL);
    
    // Get font list
    FcFontSet* font_set = FcFontList(NULL, pattern, object_set);
    
    if (font_set) {
        for (int i = 0; i < font_set->nfont; ++i) {
            FcPattern* font = font_set->fonts[i];
            FcChar8* family = nullptr;
            FcChar8* style = nullptr;
            FcChar8* file = nullptr;
            
            if (FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
                FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch &&
                FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
                
                FontInfo font_info;
                font_info.family = reinterpret_cast<const char*>(family);
                font_info.style = reinterpret_cast<const char*>(style);
                font_info.file = reinterpret_cast<const char*>(file);
                
                fonts.push_back(font_info);
            }
        }
        FcFontSetDestroy(font_set);
    }
    
    FcObjectSetDestroy(object_set);
    FcPatternDestroy(pattern);
    FcFini();
    
    std::cout << "Loaded " << fonts.size() << " fonts" << std::endl;
}

void FontConfigViewer::populateList() {
    int index = 0;
    for (const auto& font : fonts) {
        std::string font_name = font.family + " - " + font.style;

        GtkWidget* row_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_widget_set_margin_start(row_box, 8);
        gtk_widget_set_margin_end(row_box, 8);
        gtk_widget_set_margin_top(row_box, 6);
        gtk_widget_set_margin_bottom(row_box, 6);

        GtkWidget* family_style = gtk_label_new(font_name.c_str());
        gtk_widget_set_halign(family_style, GTK_ALIGN_START);
        gtk_label_set_markup(GTK_LABEL(family_style), ("<b>" + font_name + "</b>").c_str());
        gtk_label_set_ellipsize(GTK_LABEL(family_style), PANGO_ELLIPSIZE_END);
        gtk_label_set_wrap(GTK_LABEL(family_style), FALSE);

        GtkWidget* file_path = gtk_label_new(font.file.c_str());
        gtk_widget_set_halign(file_path, GTK_ALIGN_START);
        gtk_label_set_markup(GTK_LABEL(file_path), ("<small>" + font.file + "</small>").c_str());
        gtk_label_set_ellipsize(GTK_LABEL(file_path), PANGO_ELLIPSIZE_MIDDLE);
        gtk_label_set_wrap(GTK_LABEL(file_path), FALSE);

        gtk_box_append(GTK_BOX(row_box), family_style);
        gtk_box_append(GTK_BOX(row_box), file_path);

        GtkWidget* row = gtk_list_box_row_new();
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), row_box);
        g_object_set_data(G_OBJECT(row), "font-index", GINT_TO_POINTER(index));
        gtk_list_box_append(GTK_LIST_BOX(list_box), row);

        ++index;
    }

    // Select first item by default if available
    if (index > 0) {
        GtkListBoxRow* first = gtk_list_box_get_row_at_index(GTK_LIST_BOX(list_box), 0);
        gtk_list_box_select_row(GTK_LIST_BOX(list_box), first);
    }
}

void FontConfigViewer::run() {
    gtk_widget_set_visible(window, TRUE);
    
    // Create and run main loop
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    g_signal_connect(window, "destroy", G_CALLBACK(+[](GtkWidget* widget, gpointer user_data) {
        GMainLoop* loop = static_cast<GMainLoop*>(user_data);
        g_main_loop_quit(loop);
    }), loop);
    
    g_main_loop_run(loop);
    g_main_loop_unref(loop);
}
