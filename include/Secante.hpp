#ifndef SECANTE_HPP
#define SECANTE_HPP

#include "Tipos.hpp"
#include <cmath>
#include <chrono>

class Secante {
    public:
    static ResultadoMetodo executar(
        const std::function<double(double)>& f,
        double x0,
        double x1,
        double tol = 1e-6,
        int maxIter = 1000
    ) {
        ResultadoMetodo res;
        res.nomeMetodo = "Secante";

        // inicio da contagem do tempo de execução
        auto inicio = std::chrono::high_resolution_clock::now();

        double x_ant = x0;
        double x_atual = x1;
        int iter = 0;

        while (iter < maxIter){
            double f_ant = f(x_ant);
            double f_atual = f(x_atual);

            // evita divisão por zero se f(x_atual) for quase igual a f(x_ant)
            if (std::abs(f_atual - f_ant) < 1e-12){
                res.raiz = x_atual;
                res.fRaiz = f_atual;
                res.iteracoes = iter;
                res.convergiu = false;
                res.mensagemErro = "Divisao por zero: f(x_k) e f(x_{k-1}) sao muito proximos";
                return res;
            }

            // proximo pronto pela fórmula da secante
            double proximo = x_atual - f_atual * (x_atual - x_ant) / (f_atual - f_ant);

            // verificação de valores invalidos
            if (!std::isfinite(proximo)){
                res.raiz = x_atual;
                res.fRaiz = f_atual;
                res.iteracoes = iter;
                res.convergiu = false;
                res.mensagemErro = "O calculo da Secante gerou um valor numerico invalido";
                return res;
            }

            iter++;

            // critério de parada: variação em x ou f(x) próximo de zero
            if (std::abs(proximo - x_atual) < tol || std::abs(f(proximo)) < tol){
                x_atual = proximo;
                break;
            }

            // atualização para a próxima iteração
            x_ant = x_atual;
            x_atual = proximo;
        }

        // fim da contagem do tempo de execução
        auto fim = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> duracao = fim - inicio;

        res.raiz = x_atual;
        res.fRaiz = f(x_atual);
        res.iteracoes = iter;
        res.tempoMicrosegundos = duracao.count();
        res.convergiu = (iter < maxIter);
        if(!res.convergiu && res.mensagemErro.empty()){
            res.mensagemErro = "Numero maximo de iteracoes atingido sem convergencia";
        }

        return res;
    }
};


#endif // SECANTE_HPP