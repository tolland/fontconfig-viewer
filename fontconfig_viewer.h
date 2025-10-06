#ifndef FONTCONFIG_VIEWER_H
#define FONTCONFIG_VIEWER_H

#include <gtk/gtk.h>
#include <fontconfig/fontconfig.h>
#include <vector>
#include <string>

struct FontInfo {
    std::string family;
    std::string style;
    std::string file;
};

class FontConfigViewer {
private:
    GtkWidget* window;
    GtkWidget* scrolled_window;
    GtkWidget* list_box;
    GtkWidget* detail_box;
    GtkWidget* detail_family_label;
    GtkWidget* detail_style_label;
    GtkWidget* detail_file_label;
    std::vector<FontInfo> fonts;

    void setupMenuBar(GtkWidget* main_box);
    void setupMainContent(GtkWidget* main_box);
    void setupFontList();
    void setupDetailsPane();
    void connectSignals();

public:
    FontConfigViewer();
    ~FontConfigViewer();
    
    void loadFonts();
    void populateList();
    void run();
};

#endif // FONTCONFIG_VIEWER_H
