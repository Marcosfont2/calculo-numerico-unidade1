#ifndef FALSA_POSICAO_HPP
#define FALSE_POSICAO_HPP

#include "Tipos.hpp"
#include <cmath>
#include <chrono>

class FalsaPosicao {
    public:
    static ResultadoMetodo executar(
        const std::function<double(double)>& f,
        double a_in,
        double b_in,
        double tol = 1e-6,
        int maxIter = 1000
    ) {
        ResultadoMetodo res;
        res.nomeMetodo = "Falsa Posicao";

        // inicio da contagem do tempo de execução
        auto inicio = std::chrono::high_resolution_clock::now();

        double a = a_in;
        double b = b_in;
        double fa = f(a);
        double fb = f(b);

        // validação inicial do teorema de Bolzano: f(a) e f(b) têm que ter sinais opostos
        if (fa * fb > 0.0){
            res.raiz = a;
            res.fRaiz = fa;
            res.iteracoes = 0;
            res.convergiu = false;
            res.mensagemErro = "Intervalor invalid: f(a) e f(b) possuem o mesmo sinal";
            return res; 
        }

        double x_atual = a;
        int iter = 0;

        while (iter < maxIter) {
            // evita divisão por zero se f(a) e f(b) forem quase iguais 
            if (std::abs(fb - fa) < 1e-12){
                res.raiz = x_atual;
                res.fRaiz = f(x_atual);
                res.iteracoes = iter;
                res.convergiu = false;
                res.mensagemErro = "Divisao por zero: f(a) e f(b) sao muito proximos";
                return res;
            }

            // média ponderada pela interpolação linear
            double x_novo = (a * fb - b * fa) / (fb - fa);
            double fx = f(x_novo);

            // verificação se o x_novo é válido
            if (!std::isfinite(x_novo)){
                res.raiz = x_atual;
                res.fRaiz = fx;
                res.iteracoes = iter;
                res.convergiu = false;
                res.mensagemErro = "O calculo gerou um valor numerico invalido";
                return res;
            }

            iter++;

            // critério de parada: variação em x ou f(x) próximo de zero
            if (iter > 1 && (std::abs(x_novo - x_atual) < tol || std::abs(fx) < tol)){
                x_atual = x_novo;
                break;
            }

            x_atual = x_novo;

            // mantém o enquadramento do sinal da raiz
            if (fa * fx < 0.0){
                b = x_novo;
                fb + fx;
            } else {
                a = x_novo;
                fa = fx;
            }
        }

        // fim da contagem do tempo de execução
        auto fim = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> duracao = fim - inicio;

        res.raiz = x_atual;
        res.fRaiz = f(x_atual);
        res.iteracoes = iter;
        res.tempoMicrosegundos = duracao.count();
        res.convergiu = (iter < maxIter);
        if (!res.convergiu && res.mensagemErro.empty()){
            res.mensagemErro = "Numero maximo de iteracoes atingido sem convergencia";
        }

        return res;
    }
};

#endif // FALSA_POSICAO_HPP