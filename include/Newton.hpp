#ifndef NEWTON_HPP
#define NEWTON_HPP

#include "Tipos.hpp"
#include <cmath>
#include <chrono>

class Newton {
    public:
    static ResultadoMetodo executar(
        const std::function<double(double)>& f,
        const std::function<double(double)>& df,
        double x0,
        double tol = 1e-6,
        int maxIter = 1000
    ) {
        ResultadoMetodo res;
        res.nomeMetodo = "Newton";

        // início da contagem do tempo de execução
        auto inicio = std::chrono::high_resolution_clock::now();

        double x = x0;
        int iter = 0;

        while (iter < maxIter) {
            double fx = f(x);
            double dfx = df(x);

            // verifica se é divisão por zero ou derivada nula
            if (std::abs(dfx) < 1e-12){
                res.raiz = x;
                res.fRaiz = fx;
                res.iteracoes = iter;
                res.convergiu = false;
                res.mensagemErro = "Derivada nula ou muito proxima de zero (divisao por zero)";
                return res;
            }

            // proximo valor pela fórmula de newton-raphson
            double proximo = x - (fx / dfx);

            // verificação se o valor gerado é válido
            if (!std::isfinite(proximo)){
                res.raiz = x;
                res.fRaiz = fx;
                res.iteracoes = iter;
                res.convergiu = false;
                res.mensagemErro = "O calculo de Newton gerou um valor invalido";
                return res;
            }

            iter++;

            // critério de parada
            if(std::abs(proximo - x) < tol || std::abs(f(proximo)) < tol){
                x = proximo;
                break;
            }

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
        if(!res.convergiu && res.mensagemErro.empty()){
            res.mensagemErro = "Numero maximo de iteracoes atingido sem convergencia";
        }

        return res;
    }

};

#endif // NEWTON_HPP