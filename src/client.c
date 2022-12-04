#include <mysql/mysql.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define DB_SERVER "localhost"
#define DB_USER "ic21lsm"
#define DB_PASSWD "200490"
#define DB_NAME "ic21lsm"

// for debug
static void print_hello (GtkWidget *widget, gpointer data);

//button functions
void devButton_clicked_cb(GtkWidget *widget, gpointer data);
void presButton_clicked_cb(GtkWidget *widget, gpointer data);
void buscaLibro_clicked_cb(GtkWidget *widget, gpointer data);
void buscaUser_clicked_cb(GtkWidget *widget, gpointer data);
void updateUser_clicked_cb(GtkWidget *widget, gpointer data);
void regUser_clicked_cb(GtkWidget *widget, gpointer data);
void msgButton_clicked_cb(GtkWidget *widget, gpointer data); // done
void loginButtonLogin_clicked_cb(GtkWidget *widget, gpointer data); // done


GtkBuilder *builder;
MYSQL mysql;

int main(int argc, char* argv[]){
    gtk_init (&argc, &argv);

    MYSQL_RES *res;
    MYSQL_ROW row;
    MYSQL_FIELD *fields;

    GObject *loginWin;
    GObject *button;
    GError *error = NULL;

    GtkTreeViewColumn *lsc[4];
    GtkCellRenderer *lsr[4];

    GtkTreeViewColumn *lac[4];
    GtkCellRenderer *lar[4];

    GtkTreeViewColumn *uac[6];
    GtkCellRenderer *uar[6];

    char gladeid[14];

    mysql_init(&mysql);

    if(!mysql_real_connect(&mysql, DB_SERVER, DB_USER, DB_PASSWD, DB_NAME, 0, NULL, 0)) {
        fprintf(stderr, "Error al conectarse: %s\n", mysql_error(&mysql));
        exit(EXIT_FAILURE);
    }

    builder = gtk_builder_new ();
    if (gtk_builder_add_from_file (builder, "resources/gui.glade", &error) == 0) {
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
    }

    gtk_builder_connect_signals(builder, NULL);

    /*
    for(int i = 0; i < 4; i++){
        sprintf(gladeid, "lsc%d", i);
        lsc[i] = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, gladeid));
        sprintf(gladeid, "lsr%d", i);
        lsr[i] = GTK_CELL_RENDERER(gtk_builder_get_object(builder, gladeid));
        gtk_tree_view_column_add_attribute(lsc[i], lsr[i], "text", i);
    }
    */

    gtk_main ();

    mysql_close(&mysql);

    return 0;
}

static void print_hello (GtkWidget *widget, gpointer data){
    g_print ("Hello World\n");
}

void loginButton_clicked_cb(GtkWidget *widget, gpointer data){
    MYSQL_RES *res;
    MYSQL_ROW row;

    char user[10];
    char passwd[21];
    int admin;

    GObject *userEntry = gtk_builder_get_object(builder, "usrEntryLogin");
    GObject *passwdEntry = gtk_builder_get_object(builder, "passwdEntryLogin");
    GObject *loginWin = gtk_builder_get_object(builder, "loginWindow");
    GObject *msgWin = gtk_builder_get_object(builder, "msgWindow");
    GObject *msgLabel = gtk_builder_get_object(builder, "labelMsg");
    GObject *userWin;

    char queryBuffer[1024];
    char errorBuffer[1024];

    strcpy(user, gtk_entry_get_text(GTK_ENTRY(userEntry)));
    strcpy(passwd, gtk_entry_get_text(GTK_ENTRY(passwdEntry)));

    char *s = user;

    while (*s) {
        if (isdigit(*s++) == 0){
            sprintf(errorBuffer, "El número de cuenta sólo incluye números!!");
            gtk_label_set_text(GTK_LABEL(msgLabel), errorBuffer);
            gtk_widget_set_visible(GTK_WIDGET(msgWin), TRUE);
            mysql_free_result(res);
            return;
        }
    }

    sprintf(queryBuffer, "SELECT tipo_usuario FROM py_usuarios WHERE n_cuenta = %s AND pswd = \"%s\"", user, passwd);

    if(mysql_query(&mysql, queryBuffer)){
        sprintf(errorBuffer, "Error: %s", mysql_error(&mysql));
        gtk_label_set_text(GTK_LABEL(msgLabel), errorBuffer);
        gtk_widget_set_visible(GTK_WIDGET(msgWin), TRUE);
        return;
    }

    if(!(res = mysql_store_result(&mysql))){
        sprintf(errorBuffer, "Error: %s", mysql_error(&mysql));
        gtk_label_set_text(GTK_LABEL(msgLabel), errorBuffer);
        gtk_widget_set_visible(GTK_WIDGET(msgWin), TRUE);
        return;
    }

    if(row = mysql_fetch_row(res)){
        sscanf(row[0], "%d", &(admin));
        if (admin == 1)
            userWin = gtk_builder_get_object(builder, "adminWindow");
        else
            userWin = gtk_builder_get_object(builder, "solicWindow");

        gtk_widget_hide(GTK_WIDGET(loginWin));
        gtk_widget_set_visible(GTK_WIDGET(userWin), TRUE);
    }else{
        gtk_label_set_text(GTK_LABEL(msgLabel), "Usuario y/o Contraseña incorrectos");
        gtk_widget_set_visible(GTK_WIDGET(msgWin), TRUE);
        mysql_free_result(res);
        return;
    }
    mysql_free_result(res);
}

void regUser_clicked_cb(GtkWidget *widget, gpointer data){

    MYSQL_RES *res;
    MYSQL_ROW row;

    GObject *msgWin = gtk_builder_get_object(builder, "msgWindow");
    GObject *msgLabel = gtk_builder_get_object(builder, "labelMsg");

    GObject *cuenta_e = gtk_builder_get_object(builder, "cuentaEntryReg");
    GObject *nombre_e = gtk_builder_get_object(builder, "nombreEntryReg");
    GObject *apaterno_e = gtk_builder_get_object(builder, "apatEntryReg");
    GObject *amaterno_e = gtk_builder_get_object(builder, "amatEntryReg");
    GObject *carrera_e = gtk_builder_get_object(builder, "carreraEntryReg");
    GObject *semestre_c = gtk_builder_get_object(builder, "semestreComboReg");
    GObject *fnac_c = gtk_builder_get_object(builder, "fnacCalReg");
    GObject *mail_e = gtk_builder_get_object(builder, "emailEntryReg");
    GObject *passwd_e = gtk_builder_get_object(builder, "passwordEntryReg");
    GObject *admin_b = gtk_builder_get_object(builder, "adminCheckReg");

    int semestre, admin, fnac_d, fnac_m, fnac_a;
    const char *cuenta, *nombre, *apaterno, *amaterno, *carrera, *mail, *passwd;
    char fnac[12];

    char queryBuffer[1024], errorBuffer[1024];

    cuenta = gtk_entry_get_text(GTK_ENTRY(cuenta_e));
    nombre = gtk_entry_get_text(GTK_ENTRY(nombre_e));
    apaterno = gtk_entry_get_text(GTK_ENTRY(apaterno_e));
    amaterno = gtk_entry_get_text(GTK_ENTRY(amaterno_e));
    carrera = gtk_entry_get_text(GTK_ENTRY(carrera_e));
    semestre = gtk_combo_box_get_active(GTK_COMBO_BOX(semestre_c)) + 1;
    gtk_calendar_get_date(GTK_CALENDAR(fnac_c), &fnac_a, &fnac_m, &fnac_d);
    fnac_m++;
    mail = gtk_entry_get_text(GTK_ENTRY(mail_e));
    passwd = gtk_entry_get_text(GTK_ENTRY(passwd_e));
    admin = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(admin_b));

    sprintf(fnac, "%d/%d/%d", fnac_a, fnac_m, fnac_d);

    const char *s = cuenta;

    while (*s) {
        if (isdigit(*s++) == 0){
            sprintf(errorBuffer, "El número de cuenta sólo incluye números!!");
            gtk_label_set_text(GTK_LABEL(msgLabel), errorBuffer);
            gtk_widget_set_visible(GTK_WIDGET(msgWin), TRUE);
            return;
        }
    }

    sprintf(queryBuffer, "CALL p_insertar_usuario(%s, \"%s\", \"%s\", \"%s\", \"%s\", %d, \"%s\", \"%s\", \"%s\", %d)",
            cuenta, nombre, apaterno, amaterno, carrera, semestre, fnac, mail, passwd, admin);

    if(mysql_query(&mysql, queryBuffer)){
        sprintf(errorBuffer, "Error: %s", mysql_error(&mysql));
        gtk_label_set_text(GTK_LABEL(msgLabel), errorBuffer);
        gtk_widget_set_visible(GTK_WIDGET(msgWin), TRUE);
        return;
    }

    gtk_label_set_text(GTK_LABEL(msgLabel), "El usuario ha sido agregado con exito.");
    gtk_widget_set_visible(GTK_WIDGET(msgWin), TRUE);
}

void msgButton_clicked_cb(GtkWidget *widget, gpointer data){
    GObject *win = gtk_builder_get_object(builder, "msgWindow");
    gtk_widget_hide(GTK_WIDGET(win));
}

void devButton_clicked_cb(GtkWidget *widget, gpointer data){
    // CALL p_resuelve_solicitud(usuario activo)
}

void presButton_clicked_cb(GtkWidget *widget, gpointer data){
    // CALL p_alta_solicitud(usuario activo, libro seleccionado)
    // usar gtk_tree_view_selection
}

void buscaLibro_clicked_cb(GtkWidget *widget, gpointer data) {

    MYSQL_RES *res;
    MYSQL_ROW row;

    GObject *msgWin = gtk_builder_get_object(builder, "msgWindow");
    GObject *msgLabel = gtk_builder_get_object(builder, "labelMsg");

    GObject *columnCombo = gtk_builder_get_object(builder, "librosComboBusca");
    GObject *searchEntry = gtk_builder_get_object(builder, "librosEntryBusca");

    char queryBuffer[1024], errorBuffer[1024];

    const char *searchV = gtk_entry_get_text(GTK_ENTRY(searchEntry));
    int column_i = gtk_combo_box_get_active(GTK_COMBO_BOX(columnCombo)); // isbn, nombre, editorial, disponible

    switch (column_i) {
        case 0:
            const char *s = searchV;
            while (*s) {
                if (isdigit(*s++) == 0){
                    sprintf(errorBuffer, "El isbn sólo incluye números!!");
                    gtk_label_set_text(GTK_LABEL(msgLabel), errorBuffer);
                    gtk_widget_set_visible(GTK_WIDGET(msgWin), TRUE);
                    return;
                }
            }
            sprintf(queryBuffer, "SELECT isbn, libro, editorial, prestados, disponibles FROM py_libros WHERE isbn = %s", searchV);
            break;
        case 1:
            sprintf(queryBuffer, "SELECT isbn, libro, editorial, prestados, disponibles FROM py_libros WHERE libro LIKE \"%s%s%s\"", "%", searchV, "%");
            break;
        case 2:
            sprintf(queryBuffer, "SELECT isbn, libro, editorial, prestados, disponibles FROM py_libros WHERE editorial LIKE \"%s%s%s\"", "%", searchV, "%");
            break;
        case 3:
            sprintf(queryBuffer, "SELECT isbn, libro, editorial, prestados, disponibles FROM py_libros WHERE disponibles > 0");
            break;
    }

    if(mysql_query(&mysql, queryBuffer)){
        sprintf(errorBuffer, "Error: %s", mysql_error(&mysql));
        gtk_label_set_text(GTK_LABEL(msgLabel), errorBuffer);
        gtk_widget_set_visible(GTK_WIDGET(msgWin), TRUE);
        return;
    }

    if(!(res = mysql_store_result(&mysql))){
        sprintf(errorBuffer, "Error: %s", mysql_error(&mysql));
        gtk_label_set_text(GTK_LABEL(msgLabel), errorBuffer);
        gtk_widget_set_visible(GTK_WIDGET(msgWin), TRUE);
        return;
    }

    GtkTreeStore *libros_ts = GTK_TREE_STORE(gtk_builder_get_object(builder, "librosQuery"));
    GtkTreeIter columna;

    int isbn_r, disp_r, pres_r;
    char titulo_r[51], editorial_r[31];

    gtk_tree_store_clear(libros_ts);

    while(row = mysql_fetch_row(res)){
        sscanf(row[0], "%d", &isbn_r);
        strcpy(titulo_r, row[1]);
        strcpy(editorial_r, row[2]);
        sscanf(row[3], "%d", &pres_r);
        sscanf(row[4], "%d", &disp_r);
        gtk_tree_store_append(libros_ts, &columna, NULL);
        gtk_tree_store_set(libros_ts, &columna, 0, isbn_r, -1);
        gtk_tree_store_set(libros_ts, &columna, 1, titulo_r, -1);
        gtk_tree_store_set(libros_ts, &columna, 2, editorial_r, -1);
        gtk_tree_store_set(libros_ts, &columna, 3, pres_r, -1);
        gtk_tree_store_set(libros_ts, &columna, 4, disp_r, -1);
    }

}

void buscaUser_clicked_cb(GtkWidget *widget, gpointer data){
    // SELECT n_cuenta, nombre, apat, isbn, libro, f_solicitud, f_limite
}

void updateUser_clicked_cb(GtkWidget *widget, gpointer data){
    // UPDATE py_usuarios SET %s = \'%s\'    , columna, valor nuevo
}

