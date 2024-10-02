#ifndef ATENDENTE_CONTROLLER_H
#define ATENDENTE_CONTROLLER_H

#include "AtendenteModel.h"
#include "AtendenteView.h"

class AtendenteController {
public:
    AtendenteController(AtendenteModel& model, AtendenteView& view);

    void incluirAtendente(const Atendente& atendente);
    void alterarAtendente(const Atendente& atendente);
    void excluirAtendente(const std::string& matricula);
    void consultarAtendente(const std::string& matricula);
    
private:
    AtendenteModel& model;
    AtendenteView& view;
};

#endif
