#include <gtk/gtk.h>

#define IGNORE(val) ((void)val)

typedef struct {
	GtkWindow*   window;
	GtkTextView* textView;
} BtnClickArgs;

static void On_OpenBtnClick(GtkWidget* btn, gpointer args);
static void On_SaveBtnClick(GtkWidget* btn, gpointer args);

static void On_OpenFileDialog(GtkFileChooserDialog* dialog, int response, gpointer textView);
static void On_SaveFileDialog(GtkFileChooserDialog* dialog, int response, gpointer textView);

static void On_FontBtnClick(GtkWidget* widget, gpointer _);

static void activate(GtkApplication *app, gpointer user_data) {
	IGNORE(user_data);

	GtkWidget* window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "GtkTextEditor");

	GtkWidget* textView = gtk_text_view_new();
	gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView)), "Start writing!", -1);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(textView), TRUE);
	gtk_widget_set_hexpand(textView, TRUE);
	gtk_widget_set_vexpand(textView, TRUE);

	BtnClickArgs* openBtnArgs = g_new0(BtnClickArgs, 1);
	openBtnArgs->window = GTK_WINDOW(window);
	openBtnArgs->textView = GTK_TEXT_VIEW(textView);
	GtkWidget* openBtn = gtk_button_new_with_label("Open");
	g_signal_connect(openBtn, "clicked", G_CALLBACK(On_OpenBtnClick), openBtnArgs);

	BtnClickArgs* saveBtnArgs = g_new0(BtnClickArgs, 1);
	saveBtnArgs->window = GTK_WINDOW(window);
	saveBtnArgs->textView = GTK_TEXT_VIEW(textView);
	GtkWidget* saveBtn = gtk_button_new_with_label("Save");
	g_signal_connect(saveBtn, "clicked", G_CALLBACK(On_SaveBtnClick), saveBtnArgs);

	GtkWidget* FontSelectButton = gtk_font_button_new();
	g_signal_connect(FontSelectButton, "font-set", G_CALLBACK(On_FontBtnClick), NULL);

	GtkWidget* controlsContainer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_append(GTK_BOX(controlsContainer), openBtn);
	gtk_box_append(GTK_BOX(controlsContainer), saveBtn);
	gtk_box_append(GTK_BOX(controlsContainer), FontSelectButton);

	GtkWidget* mainContainer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_box_append(GTK_BOX(mainContainer), controlsContainer);
	gtk_box_append(GTK_BOX(mainContainer), textView);

	gtk_window_set_child(GTK_WINDOW(window), mainContainer);
	gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char** argv) {
	GtkApplication *app;
	int status;

	GApplicationFlags flags;
#if GLIB_CHECK_VERSION(2, 74, 0)
	flags = G_APPLICATION_DEFAULT_FLAGS;
#else
	flags = G_APPLICATION_FLAGS_NONE;
#endif

	app = gtk_application_new("io.github.pegvin", flags);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}

static void On_OpenBtnClick(GtkWidget* btn, gpointer _args) {
	IGNORE(btn);

	BtnClickArgs* args = _args;

	GtkFileChooserDialog* dialog = GTK_FILE_CHOOSER_DIALOG(
		gtk_file_chooser_dialog_new(
			"Open File", args->window, GTK_FILE_CHOOSER_ACTION_OPEN,
			"Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL
		)
	);
	g_signal_connect(dialog, "response", G_CALLBACK(On_OpenFileDialog), args->textView);
	gtk_window_set_transient_for(GTK_WINDOW(dialog), args->window);

	gtk_widget_set_sensitive(GTK_WIDGET(args->textView), FALSE);
	gtk_window_present(GTK_WINDOW(dialog));
}

static void On_SaveBtnClick(GtkWidget* btn, gpointer _args) {
	IGNORE(btn);

	BtnClickArgs* args = _args;

	GtkFileChooserDialog* dialog = GTK_FILE_CHOOSER_DIALOG(
		gtk_file_chooser_dialog_new(
			"Save File", args->window, GTK_FILE_CHOOSER_ACTION_SAVE,
			"Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL
		)
	);
	g_signal_connect(dialog, "response", G_CALLBACK(On_SaveFileDialog), args->textView);
	gtk_window_set_transient_for(GTK_WINDOW(dialog), args->window);

	gtk_widget_set_sensitive(GTK_WIDGET(args->textView), FALSE);
	gtk_window_present(GTK_WINDOW(dialog));
}

static void On_OpenFileDialog(GtkFileChooserDialog* dialog, int response, gpointer textView) {
	if (response == GTK_RESPONSE_ACCEPT) {
		GFile* file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));

		gchar* filePath = g_file_get_path(file);
		gchar* text = NULL;

		if (g_file_get_contents(filePath, &text, NULL, NULL)) {
			GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
			gtk_text_buffer_set_text(buffer, text, -1);
			g_free(text);
		}

		g_free(filePath);
		g_object_unref(file);
	}

	g_object_unref(dialog);
	gtk_widget_set_sensitive(GTK_WIDGET(textView), TRUE);
}

static void On_SaveFileDialog(GtkFileChooserDialog* dialog, int response, gpointer textView) {
	if (response == GTK_RESPONSE_ACCEPT) {
		GFile* file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));

		GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
		GtkTextIter start, end;
		gtk_text_buffer_get_bounds(buffer, &start, &end);

		gchar* text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
		gchar* filePath = g_file_get_path(file);

		g_file_set_contents(filePath, text, -1, NULL);

		g_free(text);
		g_free(filePath);
		g_object_unref(file);
	}

	g_object_unref(dialog);
	gtk_widget_set_sensitive(GTK_WIDGET(textView), TRUE);
}

static void On_FontBtnClick(GtkWidget* widget, gpointer _) {
	IGNORE(_);

	PangoFontDescription* desc = gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(widget));

	const gchar* fontFamily = pango_font_description_get_family(desc);
	gint fontSize = pango_font_description_get_size(desc) / (gdouble) PANGO_SCALE;

	GString* CssCode = g_string_new("");
	g_string_printf(
		CssCode,
		"textview {"
		"	font-family: %s;"
		"	font-size: %d%s;"
		"	font-weight: %d;"
		"}",
		fontFamily == NULL ? "monospace" : fontFamily,
		fontSize, pango_font_description_get_size_is_absolute(desc) ? "px" : "pt",
		pango_font_description_get_weight(desc)
	);

	GtkCssProvider* provider = gtk_css_provider_new();
	GdkDisplay* display = gdk_display_get_default();
	gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_css_provider_load_from_data(provider, CssCode->str, CssCode->len);
	g_object_unref(provider);
	g_string_free(CssCode, TRUE);
}
