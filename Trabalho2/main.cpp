#include "AtendenteController.h"
#include "AtendenteModel.h"
#include "AtendenteView.h"

int main() {
    AtendenteModel model("atendente.txt");
    AtendenteView view;
    AtendenteController controller(model, view);

    Atendente atendente1 = {"12345", "João", "joao1", "12345"};
    controller.incluirAtendente(atendente1);

    Atendente atendente2 = {"54321", "Maria", "maria", "54321"};
    controller.alterarAtendente(atendente2);

    controller.consultarAtendente("12345");
    controller.excluirAtendente("12345");

    return 0;
}
