#include "gerarSenha.h"

using namespace std;

// Link com a biblioteca do Windows
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "libssh2.lib")

queue<string> filaNormal;
queue<string> filaPreferencial;
ofstream arquivoLog;

GtkWidget *etiqueta;  // Label para exibir a senha gerada

/**
 * Função: gerarSenha
 * Descrição: Gera uma senha, seja preferencial ou normal, e a registra no log.
 * Pré-condição: `arquivoLog` deve estar aberto para gravação. O valor de `preferencial` deve ser verdadeiro para gerar uma senha preferencial e falso para uma senha normal.
 * Pós-condição: Uma senha será gerada e registrada no log com a data e hora atual.
 */
string gerarSenha(bool preferencial) {
    string senha = (preferencial ? "P" : "N") + string(3, 'A' + rand() % 26) + to_string(rand() % 900 + 100);
    time_t agora = time(0);
    char* dt = ctime(&agora);
    arquivoLog << "Gerada: " << senha << " em " << dt << endl;
    return senha;
}

/**
 * Função: enviarArquivoFilaParaRaspberry
 * Descrição: Envia um arquivo da fila de senhas para uma Raspberry Pi usando SSH e SCP.
 * Pré-condição: A conexão com a Raspberry Pi deve ser possível usando o IP, usuário, e senha especificados.
 * Pós-condição: O arquivo será enviado para o caminho especificado na Raspberry Pi.
 */
void enviarArquivoFilaParaRaspberry(const char* caminhoArquivoLocal, const char* caminhoArquivoRemoto) {
    const char *hostname = "192.168.174.221"; // Substitua pelo IP da Raspberry Pi
    const char *usuario = "rasp";             // Usuário da Raspberry Pi
    const char *senhaSSH = "12345678";        // Senha da Raspberry Pi
    SOCKET socket;
    struct sockaddr_in sin;
    LIBSSH2_SESSION *sessao;
    LIBSSH2_CHANNEL *canal;
    struct stat infoArquivo;
    FILE *local;

    // Inicializar Winsock
    WSADATA dadosWSA;
    if (WSAStartup(MAKEWORD(2, 2), &dadosWSA) != 0) {
        cerr << "WSAStartup falhou!" << endl;
        return;
    }

    // Criando o socket
    socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    inet_pton(AF_INET, hostname, &sin.sin_addr);

    if (connect(socket, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) {
        cerr << "Falha ao conectar!" << endl;
        closesocket(socket);
        WSACleanup();
        return;
    }

    // Iniciando a sessão SSH
    sessao = libssh2_session_init();
    if (libssh2_session_handshake(sessao, socket)) {
        cerr << "Falha ao estabelecer sessão SSH!" << endl;
        closesocket(socket);
        libssh2_session_free(sessao);
        WSACleanup();
        return;
    }

    // Autenticando com usuário e senha
    if (libssh2_userauth_password(sessao, usuario, senhaSSH)) {
        cerr << "Falha na autenticação por senha!" << endl;
        closesocket(socket);
        libssh2_session_free(sessao);
        WSACleanup();
        return;
    }

    // Enviar o arquivo via SCP
    stat(caminhoArquivoLocal, &infoArquivo);
    local = fopen(caminhoArquivoLocal, "rb");

    if (!local) {
        cerr << "Falha ao abrir arquivo local: " << caminhoArquivoLocal << endl;
        closesocket(socket);
        libssh2_session_free(sessao);
        WSACleanup();
        return;
    }

    canal = libssh2_scp_send(sessao, caminhoArquivoRemoto, infoArquivo.st_mode & 0644, (unsigned long)infoArquivo.st_size);

    if (!canal) {
        cerr << "Incapaz de abrir sessão SCP" << endl;
        fclose(local);
        closesocket(socket);
        libssh2_session_free(sessao);
        WSACleanup();
        return;
    }

    char mem[1024];
    size_t lido;
    while ((lido = fread(mem, 1, sizeof(mem), local)) > 0) {
        char *ptr = mem;
        do {
            int rc = libssh2_channel_write(canal, ptr, lido);
            if (rc < 0) {
                cerr << "Erro ao enviar dados via SCP" << endl;
                break;
            }
            ptr += rc;
            lido -= rc;
        } while (lido);
    }

    fclose(local);
    libssh2_channel_send_eof(canal);
    libssh2_channel_close(canal);
    libssh2_channel_free(canal);

    libssh2_session_disconnect(sessao, "Desligamento Normal");
    libssh2_session_free(sessao);
    closesocket(socket);
    WSACleanup();
}

/**
 * Função: atualizarArquivoFila
 * Descrição: Atualiza o arquivo que contém as filas de senhas e o envia para a Raspberry Pi.
 * Pré-condição: As filas `filaNormal` e `filaPreferencial` devem estar preenchidas com senhas geradas.
 * Pós-condição: O arquivo será atualizado com as senhas da fila e enviado para a Raspberry Pi.
 */
void atualizarArquivoFila() {
    ofstream arquivoFila("fila_senhas.txt", ios::trunc);
    if (arquivoFila.is_open()) {
        queue<string> filaTemp = filaPreferencial;
        while (!filaTemp.empty()) {
            arquivoFila << filaTemp.front() << "\n";
            filaTemp.pop();
        }
        filaTemp = filaNormal;
        while (!filaTemp.empty()) {
            arquivoFila << filaTemp.front() << "\n";
            filaTemp.pop();
        }
        arquivoFila.close();

        // Enviar o arquivo atualizado para a Raspberry Pi
        enviarArquivoFilaParaRaspberry("fila_senhas.txt", "/home/rasp/fila_senhas.txt");

    } else {
        cerr << "Incapaz de abrir arquivo de fila para escrita." << endl;
    }
}

/**
 * Função: enviarComandoSSH
 * Descrição: Envia um comando SSH para a Raspberry Pi, atualizando a senha atual no guichê especificado.
 * Pré-condição: A conexão SSH com a Raspberry Pi deve ser possível usando o IP, usuário, e senha especificados.
 * Pós-condição: A senha atual será enviada e registrada na Raspberry Pi.
 */
void enviarComandoSSH(const string &senha, int numeroGuiche) {
    const char *hostname = "192.168.174.221"; // Substitua pelo IP da Raspberry Pi
    const char *usuario = "rasp";             // Usuário da Raspberry Pi
    const char *senhaSSH = "12345678";        // Senha da Raspberry Pi
    SOCKET socket;
    struct sockaddr_in sin;
    LIBSSH2_SESSION *sessao;
    LIBSSH2_CHANNEL *canal;

    // Inicializar Winsock
    WSADATA dadosWSA;
    if (WSAStartup(MAKEWORD(2, 2), &dadosWSA) != 0) {
        cerr << "WSAStartup falhou!" << endl;
        return;
    }

    // Criando o socket
    socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    inet_pton(AF_INET, hostname, &sin.sin_addr);

    if (connect(socket, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) {
        cerr << "Falha ao conectar!" << endl;
        closesocket(socket);
        WSACleanup();
        return;
    }

    // Iniciando a sessão SSH
    sessao = libssh2_session_init();
    if (libssh2_session_handshake(sessao, socket)) {
        cerr << "Falha ao estabelecer sessão SSH!" << endl;
        closesocket(socket);
        libssh2_session_free(sessao);
        WSACleanup();
        return;
    }

    // Autenticando com usuário e senha
    if (libssh2_userauth_password(sessao, usuario, senhaSSH)) {
        cerr << "Falha na autenticação por senha!" << endl;
        closesocket(socket);
        libssh2_session_free(sessao);
        WSACleanup();
        return;
    }

    // Abrindo um canal
    canal = libssh2_channel_open_session(sessao);
    if (!canal) {
        cerr << "Incapaz de abrir uma sessão" << endl;
        closesocket(socket);
        libssh2_session_free(sessao);
        WSACleanup();
        return;
    }

    // Enviar a senha e o guichê para a Raspberry Pi
    string comando = "echo '" + senha + "' > /tmp/senha_atual.txt && sync"; 
    libssh2_channel_exec(canal, comando.c_str());

    // Limpando
    libssh2_channel_close(canal);
    libssh2_channel_free(canal);
    libssh2_session_disconnect(sessao, "Desligamento Normal");
    libssh2_session_free(sessao);
    closesocket(socket);
    WSACleanup();
}

/**
 * Função: gerarNormal
 * Descrição: Gera uma senha normal e atualiza a fila e a interface gráfica.
 * Pré-condição: A interface gráfica deve estar inicializada e a fila `filaNormal` deve estar disponível.
 * Pós-condição: Uma nova senha normal será gerada, adicionada à fila, e exibida na interface gráfica.
 */
void gerarNormal(GtkWidget *widget, gpointer data) {
    string senha = gerarSenha(false);
    filaNormal.push(senha);
    atualizarArquivoFila();
    gtk_label_set_text(GTK_LABEL(etiqueta), ("Senha Normal: " + senha).c_str());
}

/**
 * Função: gerarPreferencial
 * Descrição: Gera uma senha preferencial e atualiza a fila e a interface gráfica.
 * Pré-condição: A interface gráfica deve estar inicializada e a fila `filaPreferencial` deve estar disponível.
 * Pós-condição: Uma nova senha preferencial será gerada, adicionada à fila, e exibida na interface gráfica.
 */
void gerarPreferencial(GtkWidget *widget, gpointer data) {
    string senha = gerarSenha(true);
    filaPreferencial.push(senha);
    atualizarArquivoFila();
    gtk_label_set_text(GTK_LABEL(etiqueta), ("Senha Preferencial: " + senha).c_str());
}

/**
 * Função: chamarProxima
 * Descrição: Chama a próxima senha na fila (preferencial ou normal) e atualiza a interface gráfica.
 * Pré-condição: As filas `filaNormal` e `filaPreferencial` devem conter senhas para serem chamadas.
 * Pós-condição: A próxima senha será chamada e exibida na interface gráfica, e a fila será atualizada.
 */
void chamarProxima(GtkWidget *widget, gpointer data) {
    string proxima;
    if (!filaPreferencial.empty()) {
        proxima = filaPreferencial.front();
        filaPreferencial.pop();
    } else if (!filaNormal.empty()) {
        proxima = filaNormal.front();
        filaNormal.pop();
    }
    if (!proxima.empty()) {
        int numeroGuiche = rand() % 10 + 1;  // Guichê aleatório de 1 a 10

        // Enviar a senha atual antes de atualizar a fila
        enviarComandoSSH(proxima, numeroGuiche);

        // Preparar a fila de senhas restantes
        string fila;
        queue<string> filaTemp = filaPreferencial;
        while (!filaTemp.empty()) {
            fila += filaTemp.front() + "\n";
            filaTemp.pop();
        }
        filaTemp = filaNormal;
        while (!filaTemp.empty()) {
            fila += filaTemp.front() + "\n";
            filaTemp.pop();
        }

        atualizarArquivoFila(); // Atualizar o arquivo de fila

        gtk_label_set_text(GTK_LABEL(etiqueta), ("Próxima Senha: " + proxima).c_str());
    }
}
