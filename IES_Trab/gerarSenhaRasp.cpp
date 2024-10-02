#include <gtk/gtk.h>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sys/inotify.h>

// Estrutura para armazenar o texto da senha e fila
struct DadosAtualizacao {
    GtkWidget *etiquetaSenha;
    GtkWidget *etiquetaFila;
    char *senha;
    char *textoFila;
};

/**
 * Função: atualizarEtiquetaSenha
 * Descrição: Função chamada por `g_idle_add` para atualizar a senha e a fila na interface gráfica.
 * Pré-condição: `DadosAtualizacao` deve conter as informações de senha e fila atualizadas e válidas.
 * Pós-condição: As etiquetas da interface gráfica serão atualizadas com os novos valores de senha e fila.
 */
gboolean atualizarEtiquetaSenha(gpointer data) {
    DadosAtualizacao *dadosAtualizacao = (DadosAtualizacao *)data;
    gtk_label_set_text(GTK_LABEL(dadosAtualizacao->etiquetaSenha), dadosAtualizacao->senha);
    gtk_label_set_text(GTK_LABEL(dadosAtualizacao->etiquetaFila), dadosAtualizacao->textoFila);
    g_free(dadosAtualizacao->senha);
    g_free(dadosAtualizacao->textoFila);
    delete dadosAtualizacao;
    return FALSE;
}

/**
 * Função: monitorarArquivoFila
 * Descrição: Monitora o arquivo de fila e atualiza a interface gráfica quando há modificações.
 * Pré-condição: A interface gráfica deve estar inicializada e o diretório `/home/pi` deve ser monitorável via `inotify`.
 * Pós-condição: A interface gráfica será atualizada quando o arquivo de fila ou senha for modificado.
 */
void monitorarArquivoFila(GtkWidget *etiquetaSenha, GtkWidget *etiquetaFila) {
    int fd = inotify_init();
    if (fd < 0) {
        std::cerr << "Falha ao inicializar inotify." << std::endl;
        return;
    }

    int wd = inotify_add_watch(fd, "/home/pi", IN_MODIFY);
    if (wd < 0) {
        std::cerr << "Falha ao adicionar monitoramento para o diretório: /home/pi" << std::endl;
        close(fd);
        return;
    }

    char buffer[1024];
    while (true) {
        int length = read(fd, buffer, 1024);
        if (length < 0) {
            std::cerr << "Falha ao ler eventos inotify." << std::endl;
            break;
        }

        for (int i = 0; i < length;) {
            struct inotify_event *evento = (struct inotify_event *)&buffer[i];
            if (evento->len) {
                if (evento->mask & IN_MODIFY) {
                    if (std::string(evento->name) == "password_queue.txt") {
                        // Ler o arquivo atualizado e exibir na fila
                        std::ifstream infile("/home/pi/password_queue.txt");
                        if (infile.is_open()) {
                            std::string linha;
                            std::string textoFila;
                            while (std::getline(infile, linha)) {
                                textoFila += linha + "\n";
                            }
                            infile.close();

                            // Ler o arquivo current_password.txt para obter a senha atual
                            std::ifstream arquivoSenha("/tmp/current_password.txt");
                            std::string senha;
                            if (arquivoSenha.is_open()) {
                                std::getline(arquivoSenha, senha);
                                arquivoSenha.close();
                            }

                            DadosAtualizacao *dadosAtualizacao = new DadosAtualizacao;
                            dadosAtualizacao->etiquetaSenha = etiquetaSenha;
                            dadosAtualizacao->etiquetaFila = etiquetaFila;
                            dadosAtualizacao->senha = g_strdup(senha.c_str());
                            dadosAtualizacao->textoFila = g_strdup(textoFila.c_str());

                            g_idle_add(atualizarEtiquetaSenha, dadosAtualizacao);
                        }
                    }
                }
            }
            i += sizeof(struct inotify_event) + evento->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}

/**
 * Função: ativar
 * Descrição: Configura e ativa a interface gráfica da aplicação.
 * Pré-condição: A aplicação GTK deve estar inicializada corretamente.
 * Pós-condição: A interface gráfica será configurada e exibida, e o monitoramento do arquivo de fila será iniciado.
 */
void ativar(GtkApplication *app, gpointer user_data) {
    GtkWidget *janela;
    GtkWidget *grade;
    GtkWidget *etiquetaTituloSenha, *etiquetaTituloGuiche, *etiquetaTituloFila;
    GtkWidget *etiquetaSenha, *etiquetaGuiche, *etiquetaFila;

    janela = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(janela), "Exibidor de Senha");
    gtk_window_fullscreen(GTK_WINDOW(janela)); // Tela cheia

    // Configurar a grade para organizar os widgets
    grade = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(janela), grade);

    // Labels para títulos
    etiquetaTituloSenha = gtk_label_new("Senha:");
    etiquetaTituloGuiche = gtk_label_new("Guichê:");
    etiquetaTituloFila = gtk_label_new("Fila");

    // Labels para os valores
    etiquetaSenha = gtk_label_new("");
    etiquetaGuiche = gtk_label_new("");
    etiquetaFila = gtk_label_new("");

    // Definir identificadores de estilo (IDs) para os labels
    gtk_widget_set_name(etiquetaTituloSenha, "etiquetaTitulo");
    gtk_widget_set_name(etiquetaTituloGuiche, "etiquetaTitulo");
    gtk_widget_set_name(etiquetaTituloFila, "etiquetaTitulo");
    gtk_widget_set_name(etiquetaSenha, "etiquetaSenha");
    gtk_widget_set_name(etiquetaGuiche, "etiquetaGuiche");
    gtk_widget_set_name(etiquetaFila, "etiquetaFila");

    // Configurar a grade
    gtk_grid_attach(GTK_GRID(grade), etiquetaTituloSenha, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grade), etiquetaSenha, 1, 0, 2, 2);
    gtk_grid_attach(GTK_GRID(grade), etiquetaTituloGuiche, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grade), etiquetaGuiche, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grade), etiquetaTituloFila, 3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grade), etiquetaFila, 3, 1, 1, 2);

    // Expandir os widgets para preencher a janela
    gtk_widget_set_hexpand(etiquetaSenha, TRUE);
    gtk_widget_set_vexpand(etiquetaSenha, TRUE);
    gtk_widget_set_hexpand(etiquetaGuiche, TRUE);
    gtk_widget_set_vexpand(etiquetaGuiche, TRUE);
    gtk_widget_set_hexpand(etiquetaFila, TRUE);
    gtk_widget_set_vexpand(etiquetaFila, TRUE);

    // Alinhamento do texto dos labels
    gtk_label_set_xalign(GTK_LABEL(etiquetaSenha), 0.5);
    gtk_label_set_yalign(GTK_LABEL(etiquetaSenha), 0.5);
    gtk_label_set_xalign(GTK_LABEL(etiquetaGuiche), 0.5);
    gtk_label_set_yalign(GTK_LABEL(etiquetaGuiche), 0.5);
    gtk_label_set_xalign(GTK_LABEL(etiquetaFila), 0.5);
    gtk_label_set_yalign(GTK_LABEL(etiquetaFila), 0.0); // Alinhar no topo

    // Carregar o CSS
    GtkCssProvider *provedorCss = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provedorCss,
        "#etiquetaTitulo { font-size: 50px; font-weight: bold; }\n"
        "#etiquetaSenha { font-size: 150px; font-weight: bold; color: red; }\n"  // Fonte maior e em vermelho
        "#etiquetaGuiche { font-size: 60px; font-weight: bold; }\n"
        "#etiquetaFila { font-size: 40px; font-weight: bold; }\n", 
        -1, NULL);

    GtkStyleContext *contexto;
    contexto = gtk_widget_get_style_context(GTK_WIDGET(janela));
    gtk_style_context_add_provider(contexto, GTK_STYLE_PROVIDER(provedorCss), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all(janela);

    // Iniciar monitoramento do arquivo de fila em uma thread separada
    std::thread(monitorarArquivoFila, etiquetaSenha, etiquetaFila).detach();

    g_object_unref(provedorCss);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.exibidorSenha", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(ativar), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
