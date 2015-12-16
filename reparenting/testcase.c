#include <glib.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#define POSTER_CIRCLE_URI "https://www.webkit.org/blog-files/3d-transforms/poster-circle.html"

#define HTML_CONTENT " \
<html> \
  <body> \
    <h1>Hello World!</h1> \
    <p style='transform: rotateX(0deg);'>Text forcing Accelerated Compositing</p> \
  </body> \
</html> \
"

static GtkWidget* webview = NULL;
static GtkWidget* box1 = NULL;
static GtkWidget* box2 = NULL;
static GtkWidget* parent = NULL;

static gboolean _on_timeout_callback(gpointer data)
{
  g_print("Reparenting the WebView...\n");
  g_print("Old parent: %p\n", parent);

  gtk_container_remove(GTK_CONTAINER(parent), webview);
  parent = (parent == box2 ? box1 : box2);
  gtk_box_pack_start(GTK_BOX(parent), webview, TRUE, TRUE, 0);

  g_print("New parent: %p\n", parent);

  return G_SOURCE_CONTINUE;
}

int main(int argc, char** argv)
{
  gtk_init(&argc, &argv);

  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

  GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(vbox), box1, TRUE, TRUE, 0);

  box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(vbox), box2, TRUE, TRUE, 0);

  webview = webkit_web_view_new();
  g_object_ref_sink(webview);

  webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), POSTER_CIRCLE_URI);
  //webkit_web_view_load_html(WEBKIT_WEB_VIEW(webview), HTML_CONTENT, NULL);
  gtk_box_pack_start(GTK_BOX(box1), webview, TRUE, TRUE, 0);
  parent = box1;

  g_timeout_add(150, _on_timeout_callback, NULL);
  g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
