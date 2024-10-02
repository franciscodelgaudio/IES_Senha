#include "AtendenteModel.h"
#include <fstream>
#include <iostream>

AtendenteModel::AtendenteModel(const std::string& arquivo) : arquivoAtendentes(arquivo) {
    carregarAtendentes();
}

void AtendenteModel::carregarAtendentes() {
    std::ifstream arquivo(arquivoAtendentes);
    if (arquivo.is_open()) {
        Atendente atendente;
        while (arquivo >> atendente.matricula >> atendente.nome >> atendente.login >> atendente.senha) {
            atendentes.push_back(atendente);
        }
        arquivo.close();
    }
}

void AtendenteModel::salvarAtendentes() {
    std::ofstream arquivo(arquivoAtendentes, std::ios::trunc);
    for (const auto& atendente : atendentes) {
        arquivo << atendente.matricula << " " << atendente.nome << " "
                << atendente.login << " " << atendente.senha << std::endl;
    }
}

bool AtendenteModel::incluir(const Atendente& atendente) {
    atendentes.push_back(atendente);
    salvarAtendentes();
    return true;
}

bool AtendenteModel::alterar(const Atendente& atendente) {
    for (auto& a : atendentes) {
        if (a.matricula == atendente.matricula) {
            a = atendente;
            salvarAtendentes();
            return true;
        }
    }
    return false;
}

bool AtendenteModel::excluir(const std::string& matricula) {
    for (auto it = atendentes.begin(); it != atendentes.end(); ++it) {
        if (it->matricula == matricula) {
            atendentes.erase(it);
            salvarAtendentes();
            return true;
        }
    }
    return false;
}

Atendente AtendenteModel::consultar(const std::string& matricula) {
    for (const auto& atendente : atendentes) {
        if (atendente.matricula == matricula) {
            return atendente;
        }
    }
    return {};
}
