#include "AtendenteView.h"
#include <iostream>

void AtendenteView::exibirAtendente(const Atendente& atendente) {
    std::cout << "Matrícula: " << atendente.matricula << std::endl;
    std::cout << "Nome: " << atendente.nome << std::endl;
    std::cout << "Login: " << atendente.login << std::endl;
    std::cout << "Senha: " << atendente.senha << std::endl;
}

void AtendenteView::exibirMensagem(const std::string& mensagem) {
    std::cout << mensagem << std::endl;
}
