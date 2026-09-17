# Cálculo Numérico (DIM0404) — Trabalho 01

Repositório para a implementação computacional dos métodos de isolamento e refinamento de raízes de funções para a disciplina de Cálculo Numérico.

---

## Tecnologias e Estrutura

* **Linguagem:** C++23
* **Compilador Recomendado:** MinGW (GCC / g++)
* **Build System:** CMake
* **Relatório:** LaTeX (`docs/main.tex`)

## Como compilar

# 1. Criar e acessar o diretório de build
```
mkdir -p build && cd build

```
# 2. Configurar o gerador MinGW no CMake
```
cmake -G "MinGW Makefiles" ..
```
# 3. Compilar o projeto
```
cmake --build .
```
# 4. Executar o programa
```
.\calculo_numerico.exe
```