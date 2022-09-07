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
    float linewidth = 0.1;

    time_t rawtime;
    time(&rawtime);

    struct tm *timeinfo = localtime (&rawtime);

    char text[256];
    cairo_select_font_face(cr, "Courier",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, linewidth);
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);

    snprintf(text, 256, "%02d:%02d:%02d",
             timeinfo->tm_hour,
             timeinfo->tm_min,
             timeinfo->tm_sec);
    cairo_move_to(cr, -0.45, -0.4);
    cairo_show_text(cr, text);



    char *unit = "Text goes here!";

    cairo_select_font_face (cr, "Roboto Thin",
                            CAIRO_FONT_SLANT_NORMAL,
                            CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, linewidth);
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
    // cairo_text_extents (cr, unit, &extents);
    // x = width/2.0-(extents.width/2 + extents.x_bearing);
    // y = height/2.0 - (extents.height/2 );
    cairo_move_to (cr, x, y);
    cairo_show_text (cr, unit);

}

typedef struct {
    float x;
    float y;
    float i; // Intensity 0.0 - Off (Min), 1.0 -  On (Max)
} Segment;

GList* figure;

static void
create_figure (GList* figure){
    Segment* segment = (Segment*)malloc(sizeof(Segment));
    segment->x = 0.0;
    segment->y = 0.0;

    figure = g_list_append(figure, segment);
}

static void
draw_figure (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {

}


//////////////////////////////////////////////////////////////////////////////
static void
draw_display (GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {

    draw_setup (area, cr, width, height, user_data);
    draw_clock (area, cr, width, height, user_data);
    draw_digital_clock (area, cr, width, height, user_data);

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

    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA (display), draw_display, NULL, NULL);
    g_timeout_add(1000, (GSourceFunc) time_handler, (gpointer) display);
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
