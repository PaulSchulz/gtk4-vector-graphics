// Vector Graphics

#include <gtk/gtk.h>
#include <math.h>
#include <time.h>

#define RESOURCE "/org/mawsonlakes/vector.ui"

float m_radius     = 0.42;
float m_line_width = 0.001;

static void
draw_setup (GtkDrawingArea *area,
            cairo_t *cr,
            int width,
            int height,
            gpointer user_data) {

    float line_width;

    // Centre and scale the window
    if (width < height) {
        cairo_scale(cr, width, width);
        line_width = m_line_width * 1000.0 / width;
    } else {
        cairo_scale(cr, height, height);
        line_width = m_line_width * 1000.0 / height;
    }

    // Center Screen
    cairo_translate(cr, 0.5, 0.5);

    // Set the line width and save the cairo drawing state.
    cairo_set_line_width(cr, line_width);
}

static void
draw_clock (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {

    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
    cairo_paint(cr);

    // Resore back to precious drawing state and draw the circular path
    // representing the clockface. Save this state (including the path) so we
    // can reuse it.
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.7);
    cairo_arc(cr, 0.0, 0.0, m_radius, 0.0, 2.0 * M_PI);
    cairo_save(cr);

    cairo_stroke(cr);
    cairo_restore(cr); /* stack-pen-size */

    // Clock ticks
    for (int i = 0; i < 60; i++)
    {
        // Major tick size
        double inset = 0.06;

        // Save the graphics state, restore after drawing tick to maintain pen
        // size
        cairo_save(cr);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

        float_t angle;

        angle = 2.0 * M_PI / 60.0;

        // Minor ticks are shorter, and narrower.
        if(i % 5 != 0)
        {
            inset *= 0.5;
            // cairo_set_line_width(cr, m_line_width);
        }

        // Draw tick mark
        cairo_move_to(
            cr,
            (m_radius - inset) * cos (i * angle),
            (m_radius - inset) * sin (i * angle));
        cairo_line_to(
            cr,
            m_radius * cos (i * angle),
            m_radius * sin (i * angle));

        cairo_stroke(cr);
        cairo_restore(cr); /* stack-pen-size */

    }

    // Draw the analog hands

    // Get the current Unix time, convert to the local time and break into time
    // structure to read various time parts.

    // TODO For some reason this is getting the wrong hour. Out by 6 hours.

    time_t rawtime;
    time(&rawtime);
    struct tm * timeinfo = localtime (&rawtime);

    // Calculate the angles of the hands of our clock
    double hours   = timeinfo->tm_hour * M_PI / 12.0;
    double minutes = timeinfo->tm_min * M_PI / 30.0;
    double seconds = timeinfo->tm_sec * M_PI / 30.0;

    // Save the graphics state
    cairo_save(cr);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    cairo_save(cr);

    // Draw the seconds hand
    // cairo_set_line_width(cr, m_line_width / 3.0);
    // cairo_set_line_width(cr, m_line_width);
    // cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, 0.8);   // gray
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.7);
    cairo_move_to(cr, 0.0, 0.0);
    cairo_line_to(cr,
                  sin(seconds) * (m_radius * 0.9),
                  -cos(seconds) * (m_radius * 0.9));
    cairo_stroke(cr);
    cairo_restore(cr);

    // Draw the minutes hand
    // cairo_set_source_rgba(cr, 0.117, 0.337, 0.612, 0.9);   // blue
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.7);
    cairo_move_to(cr, 0.0, 0.0);
    cairo_line_to(cr,
                  sin(minutes + seconds / 60) * (m_radius * 0.8),
                  -cos(minutes + seconds / 60) * (m_radius * 0.8));
    cairo_stroke(cr);

    // draw the hours hand
    // cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9);   // green
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.7);
    cairo_move_to(cr, 0.0, 0.0);
    cairo_line_to(cr,
                  sin(hours + minutes / 12.0) * (m_radius * 0.5),
                  -cos(hours + minutes / 12.0) * (m_radius * 0.5));
    cairo_stroke(cr);
    cairo_restore(cr);
}

//////////////////////////////////////////////////////////////////////////////
static void
draw_digital_clock (GtkDrawingArea *area,
                    cairo_t *cr,
                    int width,
                    int height,
                    gpointer user_data) {

    int x = 0.5;
    int y = 0.0;
    float fontsize = 0.05;

    time_t rawtime;
    time(&rawtime);

    struct tm *timeinfo = localtime (&rawtime);

    char text[256];
    cairo_select_font_face(cr, "Roboto Thin",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, fontsize);
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);

    snprintf(text, 256, "%02d:%02d:%02d",
             timeinfo->tm_hour,
             timeinfo->tm_min,
             timeinfo->tm_sec);
    cairo_move_to(cr, -0.45, -0.4);
    cairo_show_text(cr, text);

    char *unit = "Text goes here!";
    cairo_move_to (cr, x, y);
    cairo_show_text (cr, unit);

}

//////////////////////////////////////////////////////////////////////////////
typedef struct {
    float x;
    float y;
    float i; // Intensity 0.0 - Off (Min), 1.0 -  On (Max)
} Segment;

// Will modify 'figure' variable if initially NULL, otherwise element will be
// added to end.
GList*
figure_segment_add (GList* figure, float x, float y, float i) {
    GList*   fig = figure;
    Segment* segment = NULL;

    segment = (Segment*) malloc(sizeof(Segment));
    segment->x =  x;
    segment->y =  y;
    segment->i =  i;

    figure = g_list_append(figure, segment);

    return figure;
}

// Square
GList*
create_square (GList* figure){
    figure = figure_segment_add(figure, 0.0, 0.0, 0.0);
    figure = figure_segment_add(figure, 1.0, 0.0, 1.0);
    figure = figure_segment_add(figure, 1.0, 1.0, 1.0);
    figure = figure_segment_add(figure, 0.0, 1.0, 1.0);
    figure = figure_segment_add(figure, 0.0, 0.0, 1.0);
    return figure;
}

GList*
create_spaceship (GList* figure){
    figure = figure_segment_add(figure,  0.0,  0.0, 0.0);
    figure = figure_segment_add(figure,  1.0, -1.0, 1.0);
    figure = figure_segment_add(figure,  0.0,  1.0, 1.0);
    figure = figure_segment_add(figure, -1.0, -1.0, 1.0);
    figure = figure_segment_add(figure,  0.0,  0.0, 1.0);
    return figure;
}


GList*
create_spaceship2 (GList* figure) {
    figure = figure_segment_add(figure,  -1.0,  -2.0, 0.0);
    figure = figure_segment_add(figure,   0.0,   2.0, 1.0);
    figure = figure_segment_add(figure,   1.0,  -2.0, 1.0);
    figure = figure_segment_add(figure,  -0.75, -1.0, 0.0);
    figure = figure_segment_add(figure,   0.75, -1.0, 1.0);
    return figure;
}


GList*
create_radar (GList* figure) {
    float radius = 1.0;

    figure = figure_segment_add(figure, 0.0, 0.0, 0.0);
    figure = figure_segment_add(figure, radius, 0.0, 0.0);
    return figure;
}

GList*
create_box (GList* figure, float x, float y, float width, float height) {
    figure = figure_segment_add(figure, x, y, 0.0);
    figure = figure_segment_add(figure, x+width,      0.0, 1.0);
    figure = figure_segment_add(figure, x+width, y+height, 1.0);
    figure = figure_segment_add(figure, x,       y+height, 1.0);
    figure = figure_segment_add(figure, x,              y, 1.0);
    return figure;
}

GList*
create_army (GList* figure) {
    float width = 0.1;

    figure = create_box(figure, 0.0, 0.0, width, width*0.75);

    figure = figure_segment_add(figure,   0.0,         0.0, 0.0);
    figure = figure_segment_add(figure, width, width * 0.75, 1.0);

    figure = figure_segment_add(figure,   0.0, width * 0.75, 0.0);
    figure = figure_segment_add(figure, width,         0.0, 1.0);

    return figure;
}

GList*
create_cavalry (GList* figure) {
    float width = 0.1;

    figure = create_box(figure, 0.0, 0.0, width, width*0.75);

    figure = figure_segment_add(figure,   0.0, width * 0.75, 0.0);
    figure = figure_segment_add(figure, width,         0.0, 1.0);

    return figure;
}


//////////////////////////////////////////////////////////////////////////////
typedef struct {
    float x;
    float y;
    float z;
    float i; // Intensity 0.0 - Off (Min), 1.0 -  On (Max)
} Segment3D;

// Will modify 'figure' variable if initially NULL, otherwise element will be
// added to end.
GList*
figure3d_segment3d_add (GList* figure3d, float x, float y, float z, float i) {
    Segment3D* segment3d = NULL;

    segment3d = (Segment3D*) malloc(sizeof(Segment3D));
    segment3d->x =  x;
    segment3d->y =  y;
    segment3d->z =  z;
    segment3d->i =  i;

    figure3d = g_list_append(figure3d, segment3d);

    return figure3d;
}

#define  X0  1.0
#define  nX (-1.0 * X0)
#define  Y0 (X0 + 1/sqrt(2.0))
#define nY0 (-1.0 * Y0)

GList*
create_icosohedron (GList* figure3d) {
    Segment3D* segment3d;

    figure3d = figure3d_segment3d_add(figure3d, X0,  Y0, 0.0, 0.0);
    figure3d = figure3d_segment3d_add(figure3d, X0, nY0, 0.0, 1.0);

    return figure3d;
}

//////////////////////////////////////////////////////////////////////////////
GList*
figure3d_project(GList* figure3d, GList* figure) {
    Segment3D* segment3d = NULL;
    Segment*   segment   = NULL;
    GList*     element3d = NULL;
    GList*     element   = NULL;

    for (element3d = figure3d; element3d; element3d = element3d->next){
        segment3d = element3d->data;
        figure = figure_segment_add(figure,
                                    segment3d->x,
                                    segment3d->y,
                                    segment3d->i); // Ignore z
    }

    return figure;
}


//////////////////////////////////////////////////////////////////////////////
// Figure Utilities
//
// A 'figure' is a GList of Segment points. The 'intensity' of the first point
// is ignored. An intensity of 0.0 allows a 'move', rather than a 'draw'.

// TODO Create free_figure function
void
free_figure (GList* figure) {

}

// Translate all of the figure points in a fixed direction.
void
translate_figure (GList* figure, float dx, float dy) {
    Segment* segment;
    GList*   element;

    for (element = figure; element; element = element->next) {
        segment = element->data;
        segment->x = segment->x + dx;;
        segment->y = segment->y + dy;;
    }
}

// Scale a figure by an amount, centered on the origin.
void
scale_figure (GList* figure, float scale) {
    Segment* segment;
    GList*   element;

    for (element = figure; element; element = element->next) {
        segment = element->data;
        segment->x = segment->x * scale;
        segment->y = segment->y * scale;
    }
}

// Rotate a figure by an amount, centered on the origin.
// Angle is in radians.
// TODO Create rotate_figure()
void
rotate_figure (GList* figure, float angle) {

}

// Rotate a figure by an amount, centered on a point.
// Angle is in radians.
// TODO Create rotate_point_figure()
void
rotate_point_figure (GList* figure, float x, float y, float angle) {

}

//////////////////////////////////////////////////////////////////////////////
GList* figure    = NULL;

// Draw a single figure on the screen
static void
draw_figure (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    Segment* segment;
    GList*   element;

    segment = figure->data;
    cairo_move_to(cr, segment->x, segment->y);

    // Iterate from second element
    // TODO Add intensity properly (rather than on/off)
    for(element = figure->next; element; element = element->next) {
        segment = element->data;
        if (segment->i == 0.0){
            cairo_move_to(cr, segment->x, segment->y);
        } else {
            cairo_line_to(cr, segment->x, segment->y);
        }
    }

    cairo_stroke(cr);
}

//////////////////////////////////////////////////////////////////////////////
GList* spaceship  = NULL;
GList* spaceship2 = NULL;
GList* square     = NULL;

GList* army       = NULL;
GList* cavalry    = NULL;


static void
setup_display (void) {
    GList* spaceship = NULL;

    square    = create_square(square);
    scale_figure(square, 0.02);
    translate_figure(square, -0.2, 0.0);

    spaceship = create_spaceship(spaceship);
    scale_figure(spaceship, 0.02);

    spaceship2 = create_spaceship2(spaceship2);
    scale_figure(spaceship2, 0.01);
    translate_figure(spaceship2, 0.2, 0.0);

    army = create_army(army);
    translate_figure(army, 0.4, 0.0);

    cavalry = create_cavalry(cavalry);
    translate_figure(cavalry, 0.6, 0.0);

    figure = g_list_concat(figure, square);
    figure = g_list_concat(figure, spaceship);
    figure = g_list_concat(figure, spaceship2);
    figure = g_list_concat(figure, army);
    figure = g_list_concat(figure, cavalry);

}

float pos = 0.0;
float dpos = 0.001;

static void
draw_display (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {

    int x = 0;
    int y = 0;

    translate_figure(figure, 0.0, dpos);
    pos = pos + dpos;

    if ( pos > 0.1 ) {
        dpos = -0.001;
    } else if (pos < -0.1) {
        dpos = 0.001;
    }

    draw_setup (area, cr, width, height, user_data);
    draw_clock (area, cr, width, height, user_data);
    draw_digital_clock (area, cr, width, height, user_data);
    draw_figure (area, cr, width, height, user_data);
}

//////////////////////////////////////////////////////////////////////////////
gboolean
time_handler(GtkWidget* widget) {
    gtk_widget_queue_draw(widget);

    return TRUE;
}

static void
app_activate (GApplication *app, gpointer user_data) {
    GtkWidget *win;
    GtkWidget *display;
    GtkBuilder *build;

    build = gtk_builder_new_from_resource (RESOURCE);
    win = GTK_WIDGET (gtk_builder_get_object (build, "win"));
    gtk_window_set_application (GTK_WINDOW (win), GTK_APPLICATION (app));

    display = GTK_WIDGET (gtk_builder_get_object (build, "display"));
    g_object_unref(build);

    setup_display();
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (display), draw_display, NULL, NULL);
    g_timeout_add(10, (GSourceFunc) time_handler, (gpointer) display);
    gtk_widget_show(win);

}

static void
app_open (GApplication *app, GFile **files, gint n_files, gchar *hint, gpointer user_data) {
    app_activate(app,user_data);
}

int
main (int argc, char **argv) {
    GtkApplication *app;
    int stat;

    app = gtk_application_new ("com.mawsonlakes.vector", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
    g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
    stat = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return stat;
}
