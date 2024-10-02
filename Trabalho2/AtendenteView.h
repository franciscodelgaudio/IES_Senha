#ifndef ATENDENTE_VIEW_H
#define ATENDENTE_VIEW_H

#include <string>
#include "AtendenteModel.h"

class AtendenteView {
public:
    void exibirAtendente(const Atendente& atendente);
    void exibirMensagem(const std::string& mensagem);
};

#endif
