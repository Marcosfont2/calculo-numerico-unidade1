#ifndef BISSECCAO_HPP
#define BISSECCAO_HPP

#include "Tipos.hpp"
#include <cmath>
#include <chrono>

class Bisseccao {
public:
    static ResultadoMetodo executar(
        const std::function<double(double)>& f,
        double a,
        double b,
        double tol = 1e-6,
        int maxIter = 1000
    ) {
        ResultadoMetodo res;
        res.nomeMetodo = "Bisseccao";

        auto inicio = std::chrono::high_resolution_clock::now();

        double fa = f(a);
        double fb = f(b);

        if (fa * fb > 0.0) {
            res.convergiu = false;
            res.mensagemErro = "Sinais iguais nas extremidades do intervalo [a, b]";
            return res;
        }

        double c = a;
        int iter = 0;

        while ((b - a) / 2.0 > tol && iter < maxIter) {
            iter++;
            c = a + (b - a) / 2.0;
            double fc = f(c);

            if (std::abs(fc) < 1e-15 || (b - a) / 2.0 < tol) {
                break;
            }

            if (fa * fc < 0.0) {
                b = c;
                fb = fc;
            } else {
                a = c;
                fa = fc;
            }
        }

        auto fim = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> duracao = fim - inicio;

        res.raiz = c;
        res.fRaiz = f(c);
        res.iteracoes = iter;
        res.tempoMicrosegundos = duracao.count();
        res.convergiu = (iter < maxIter);

        return res;
    }
};

#endif // BISSECCAO_HPP