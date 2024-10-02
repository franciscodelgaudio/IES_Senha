#ifndef ATENDENTE_MODEL_H
#define ATENDENTE_MODEL_H

#include <string>
#include <vector>
#include <fstream>

struct Atendente {
    std::string matricula;  // 5 dígitos
    std::string nome;
    std::string login;      // 5 caracteres
    std::string senha;      // 5 caracteres
};

class AtendenteModel {
public:
    AtendenteModel(const std::string& arquivo);
    
    bool incluir(const Atendente& atendente);
    bool alterar(const Atendente& atendente);
    bool excluir(const std::string& matricula);
    Atendente consultar(const std::string& matricula);
    
private:
    std::vector<Atendente> atendentes;
    std::string arquivoAtendentes;
    
    void carregarAtendentes();
    void salvarAtendentes();
};

#endif
