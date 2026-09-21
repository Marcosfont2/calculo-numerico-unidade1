#ifndef PONTO_FIXO_HPP
#define PONTO_FIXO_HPP

#include "Tipos.hpp"
#include <cmath>
#include <chrono>

class PontoFixo {
public:
    static ResultadoMetodo executar(
        const std::function<double(double)>& f,
        const std::function<double(double)>& phi,
        double x0,
        double tol = 1e-6,
        int maxIter = 1000
    ) {
        ResultadoMetodo res;
        res.nomeMetodo = "Ponto Fixo";

        // início da contagem do tempo de execução
        auto inicio = std::chrono::high_resolution_clock::now();

        double x = x0;
        int iter = 0;

        // loop das iterações até atingir a tolerância ou número máximo
        while (iter < maxIter) {
            // calculo da próxima aproximação pela função de iteração
            double proximo = phi(x);
            // verificação se o resultado da função de iteração é válido
            if (!std::isfinite(proximo)){
                res.raiz = x;
                res.fRaiz = f(x);
                res.iteracoes = iter;
                res.convergiu = false;
                res.mensagemErro = "A função de iteracao gerou um valor invalido";
                return res;
            }

            iter++;

            // verificação do critério de parada
            if (std::abs(proximo - x) < tol) {
                x = proximo;
                break;
            }

            // atualização da aproximaçâo para a próxima iteração
            x = proximo;
        }

        // fim da contagem do tempo de execução
        auto fim = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> duracao = fim - inicio;

        res.raiz = x;
        res.fRaiz = f(x);
        res.iteracoes = iter;
        res.tempoMicrosegundos = duracao.count();
        res.convergiu = (iter < maxIter);

        // mensagem de erro se o método não convergir dentro do limite
        if (!res.convergiu && res.mensagemErro.empty()) {
            res.mensagemErro = "Numero maximo de iteracoes atingido";
        }

        return res;
    }
};

#endif // PONTO_FIXO_HPP
