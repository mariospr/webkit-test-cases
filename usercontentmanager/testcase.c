#include <webkit2/webkit2.h>

#define HTML_CONTENT " \
<html> \
  <body> \
    <h1>Test case for WebKitUserContentManager</h1> \
    <h3>DESCRIPTION:</h3> \
    <p> \
      This test case checks that the injected JavaScript code is be executed each \
      time before the webview loads content, but for now this is only true when content \
      is <b>loaded in a WebView created BEFORE the associated Web Process has been spawned</b>. \
    </p> \
    <p> \
      This means that clicking on the 'Reload' button right after launch will \
      execute the JavaScript code every time it loads the content, <b>until you \
      click on 'Replace WebView'</b>. Once you replace the WebView once, further \
      loads of the content will <b>NOT</b> run the JavaScript code, which is a bug. \
    </p> \
    <h3>STEPS TO REPRODUCE:</h3> \
    <p> \
    <ol> \
      <li>Launch the test case and observe the alert dialog showing up before loading</li> \
      <li>Click on the 'Reload' button several times (feel free to specify a URI too)</li> \
      <li>Each time you 'Reload' you see the alert dialog showing up before loading</li> \
      <li>Now click on 'Replace WebView' and let the webview load</li> \
      <li>Now click on the 'Reload' button several times again</li> \
    </ol> \
    </p> \
    <h3>EXPECTED OUTCOME</h3> \
    <p> \
    You should see the alert dialog showing up each time you reload, before the actual \
    content is rendereg on the WebView. \
    </p> \
    <h3>ACTUAL OUTCOME</h3> \
    <p> \
    No alert dialog shows up this time before loading after having replaced the webview \
    (step 4), neither it will show up with further attempts to 'Reload' the webview. \
    </p> \
    <h3>Other comments</h3> \
    <p> \
    Note that manually killing the Web Process from the terminal after Step 4 will make\
    the alert dialog to show up again on reload, as a new Web Process will be spawned. \
    </p> \
  </body> \
</html> \
"

#define JAVASCRIPT_TO_INJECT "alert('You should see me each time you reload, seriously')"

static GtkWidget* webview = NULL;
static GtkWidget* entry = NULL;
static GtkWidget* main_vbox = NULL;

static GtkWidget* create_webview()
{
  WebKitUserContentManager* content_manager = webkit_user_content_manager_new();
  WebKitUserScript *user_script = webkit_user_script_new(JAVASCRIPT_TO_INJECT,
                                                         WEBKIT_USER_CONTENT_INJECT_TOP_FRAME,
                                                         WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START,
                                                         NULL, NULL);
  webkit_user_content_manager_add_script(content_manager, user_script);
  webview = webkit_web_view_new_with_user_content_manager(content_manager);
  g_object_ref_sink(webview);

  return webview;
}

static void reload_content()
{
  g_autofree gchar* current_uri = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));

  if (g_strcmp0(g_strstrip(current_uri), "")) {
    g_print("Loading existing URI (%s)...\n\n", current_uri);
    webkit_web_view_load_uri (WEBKIT_WEB_VIEW(webview), current_uri);
  } else {
    g_print("Loading default HTML content (no URI specified)...\n\n");
    webkit_web_view_load_html (WEBKIT_WEB_VIEW(webview), HTML_CONTENT, NULL);
  }
}

static void reload_button_clicked_cb(GtkButton* button, gpointer unused)
{
  reload_content();
}

static void replace_webview_button_clicked_cb(GtkButton* button, gpointer unused)
{
  g_print("Removing and destroying old webview...\n");
  gtk_container_remove(GTK_CONTAINER(main_vbox), webview);

  g_print("Creating new webview...\n");
  if (webview)
    g_clear_object(&webview);
  webview = create_webview();
  gtk_widget_show(webview);

  g_print("Re-attaching new webview...\n");
  gtk_box_pack_start(GTK_BOX(main_vbox), webview, TRUE, TRUE, 0);

  g_print("Reloading content...\n");
  reload_content();
}

int main (int argc, char** argv)
{
  gtk_init (&argc, &argv);

  GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  main_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER(window), main_vbox);

  GtkWidget* hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

  entry = gtk_entry_new();
  gtk_box_pack_start (GTK_BOX(hbox), entry, TRUE, TRUE, 0);

  GtkWidget* reload_button = gtk_button_new_with_label("Reload");
  gtk_box_pack_start (GTK_BOX(hbox), reload_button, FALSE, FALSE, 0);

  GtkWidget* replace_webview_button = gtk_button_new_with_label("Replace WebView");
  gtk_box_pack_start (GTK_BOX(hbox), replace_webview_button, FALSE, FALSE, 0);

  gtk_box_pack_start (GTK_BOX(main_vbox), hbox, FALSE, FALSE, 0);

  webview = create_webview();
  gtk_box_pack_start (GTK_BOX(main_vbox), webview, TRUE, TRUE, 0);

  g_signal_connect (reload_button, "clicked", G_CALLBACK(reload_button_clicked_cb), NULL);
  g_signal_connect (replace_webview_button, "clicked", G_CALLBACK(replace_webview_button_clicked_cb), NULL);
  g_signal_connect (window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
  gtk_widget_show_all (window);

  reload_content();

  gtk_main();

  return 0;
}
