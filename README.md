# Exibidor de Senha com Interface Gráfica

Este projeto implementa uma aplicação em C++ com interface gráfica usando GTK para exibir senhas e a fila de atendimento de uma aplicação em um ambiente de monitoramento de arquivos. A aplicação é projetada para ser executada em um dispositivo como uma Raspberry Pi.

## Requisitos

Antes de começar, certifique-se de que seu ambiente de desenvolvimento atende aos seguintes requisitos:

### Hardware

- Raspberry Pi ou qualquer outro dispositivo compatível com Linux.

### Software

- Sistema operacional baseado em Linux (recomendado Raspberry Pi OS).
- GTK 3.0 ou superior.
- Inotify para monitoramento de arquivos.
- Compilador C++ (g++).
- Ferramentas de desenvolvimento essenciais (`build-essential`, `libgtk-3-dev`).

## Configuração do Ambiente

1. **Atualize e instale as dependências necessárias:**

   ```bash
   sudo apt-get update
   sudo apt-get install -y build-essential libgtk-3-dev inotify-tools
