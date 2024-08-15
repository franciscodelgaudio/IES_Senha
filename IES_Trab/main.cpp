#include <iostream>
#include "gerarSenha.h"

using namespace std;

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    GtkWidget *janela, *botao1, *botao2, *botao3, *caixa;

    janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Gerador de Senha");
    gtk_container_set_border_width(GTK_CONTAINER(janela), 10);

    caixa = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(janela), caixa);

    botao1 = gtk_button_new_with_label("Gerar Senha Normal");
    botao2 = gtk_button_new_with_label("Gerar Senha Preferencial");
    botao3 = gtk_button_new_with_label("Chamar Próxima Senha");

    gtk_box_pack_start(GTK_BOX(caixa), botao1, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(caixa), botao2, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(caixa), botao3, TRUE, TRUE, 0);

    // Label para exibir a senha
    etiqueta = gtk_label_new("Senha irá aparecer aqui");
    gtk_box_pack_start(GTK_BOX(caixa), etiqueta, TRUE, TRUE, 0);

    arquivoLog.open("log_senhas.txt", ios::app);

    g_signal_connect(botao1, "clicked", G_CALLBACK(gerarNormal), NULL);
    g_signal_connect(botao2, "clicked", G_CALLBACK(gerarPreferencial), NULL);
    g_signal_connect(botao3, "clicked", G_CALLBACK(chamarProxima), NULL);
    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(janela);
    gtk_main();

    arquivoLog.close();
    return 0;
}