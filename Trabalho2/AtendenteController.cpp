#include "AtendenteController.h"

AtendenteController::AtendenteController(AtendenteModel& model, AtendenteView& view)
    : model(model), view(view) {}

void AtendenteController::incluirAtendente(const Atendente& atendente) {
    if (model.incluir(atendente)) {
        view.exibirMensagem("Atendente incluído com sucesso!");
    } else {
        view.exibirMensagem("Erro ao incluir atendente.");
    }
}

void AtendenteController::alterarAtendente(const Atendente& atendente) {
    if (model.alterar(atendente)) {
        view.exibirMensagem("Atendente alterado com sucesso!");
    } else {
        view.exibirMensagem("Erro ao alterar atendente.");
    }
}

void AtendenteController::excluirAtendente(const std::string& matricula) {
    if (model.excluir(matricula)) {
        view.exibirMensagem("Atendente excluído com sucesso!");
    } else {
        view.exibirMensagem("Erro ao excluir atendente.");
    }
}

void AtendenteController::consultarAtendente(const std::string& matricula) {
    Atendente atendente = model.consultar(matricula);
    if (!atendente.matricula.empty()) {
        view.exibirAtendente(atendente);
    } else {
        view.exibirMensagem("Atendente não encontrado.");
    }
}
