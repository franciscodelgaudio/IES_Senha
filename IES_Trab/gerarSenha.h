#ifndef GERARSENHA_H
#define GERARSENHA_H

#include <gtk/gtk.h>
#include <fstream>
#include <string>
#include <ctime>
#include <queue>
#include <cstdlib>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <iostream>
#include <sys/stat.h>

/**
 * Função: gerarSenha
 * Descrição: Gera uma senha, seja preferencial ou normal, e a registra no log.
 * Pré-condição: `arquivoLog` deve estar aberto para gravação. O valor de `preferencial` deve ser verdadeiro para gerar uma senha preferencial e falso para uma senha normal.
 * Pós-condição: Uma senha será gerada e registrada no log com a data e hora atual.
 */
string gerarSenha(bool preferencial);

/**
 * Função: enviarArquivoFilaParaRaspberry
 * Descrição: Envia um arquivo da fila de senhas para uma Raspberry Pi usando SSH e SCP.
 * Pré-condição: A conexão com a Raspberry Pi deve ser possível usando o IP, usuário, e senha especificados.
 * Pós-condição: O arquivo será enviado para o caminho especificado na Raspberry Pi.
 */
void enviarArquivoFilaParaRaspberry(const char* caminhoArquivoLocal, const char* caminhoArquivoRemoto);

/**
 * Função: atualizarArquivoFila
 * Descrição: Atualiza o arquivo que contém as filas de senhas e o envia para a Raspberry Pi.
 * Pré-condição: As filas `filaNormal` e `filaPreferencial` devem estar preenchidas com senhas geradas.
 * Pós-condição: O arquivo será atualizado com as senhas da fila e enviado para a Raspberry Pi.
 */
void atualizarArquivoFila();

/**
 * Função: enviarComandoSSH
 * Descrição: Envia um comando SSH para a Raspberry Pi, atualizando a senha atual no guichê especificado.
 * Pré-condição: A conexão SSH com a Raspberry Pi deve ser possível usando o IP, usuário, e senha especificados.
 * Pós-condição: A senha atual será enviada e registrada na Raspberry Pi.
 */
void enviarComandoSSH(const string &senha, int numeroGuiche);

/**
 * Função: gerarNormal
 * Descrição: Gera uma senha normal e atualiza a fila e a interface gráfica.
 * Pré-condição: A interface gráfica deve estar inicializada e a fila `filaNormal` deve estar disponível.
 * Pós-condição: Uma nova senha normal será gerada, adicionada à fila, e exibida na interface gráfica.
 */
void gerarNormal(GtkWidget *widget, gpointer data);

/**
 * Função: gerarPreferencial
 * Descrição: Gera uma senha preferencial e atualiza a fila e a interface gráfica.
 * Pré-condição: A interface gráfica deve estar inicializada e a fila `filaPreferencial` deve estar disponível.
 * Pós-condição: Uma nova senha preferencial será gerada, adicionada à fila, e exibida na interface gráfica.
 */
void gerarPreferencial(GtkWidget *widget, gpointer data);

/**
 * Função: chamarProxima
 * Descrição: Chama a próxima senha na fila (preferencial ou normal) e atualiza a interface gráfica.
 * Pré-condição: As filas `filaNormal` e `filaPreferencial` devem conter senhas para serem chamadas.
 * Pós-condição: A próxima senha será chamada e exibida na interface gráfica, e a fila será atualizada.
 */
void chamarProxima(GtkWidget *widget, gpointer data);

#endif