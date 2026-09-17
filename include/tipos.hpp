#ifndef TIPOS_HPP
#define TIPOS_HPP

#include <string>
#include <vector>
#include <functional>

// Representa um intervalo [a, b]
struct Intervalo {
    double a;
    double b;
};

// Estrutura para armazenar o resultado de qualquer método de refinamento
struct ResultadoMetodo {
    std::string nomeMetodo;
    double raiz = 0.0;
    double fRaiz = 0.0;
    int iteracoes = 0;
    double tempoMicrosegundos = 0.0;
    bool convergiu = false;
    std::string mensagemErro = "";
};

// Representa a função e seus parâmetros associados
struct ProblemaFuncao {
    std::string id;                                // Ex: "f1", "f2"
    std::string expressao;                         // Ex: "2x^4 + 4x^3 + 3x^2 - 10x - 15"
    std::function<double(double)> f;              // Função f(x)
    std::function<double(double)> df;             // Derivada f'(x) (nos casos que for necessária)
    // Adicionem aqui depois o método para a função de ativação (phi) quando for necessário
    Intervalo intervaloInicial;                    // Intervalo I = [a, b]
    double passoH;                                 // Passo h para varredura
};

#endif // TIPOS_HPP