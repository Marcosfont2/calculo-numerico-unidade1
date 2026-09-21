#ifndef BISSECCAO_HPP
#define BISSECCAO_HPP

#include "Tipos.hpp"
#include <cmath>
#include <chrono>

class Bisseccao {
public:
    // Executa o método da bissecção para refinar uma raiz em um intervalo isolado.
    // Entrada: função f, extremos a e b com sinais opostos, tolerância e máximo de iterações.
    // Saída: raiz aproximada, residual, métricas e motivo de término no ResultadoMetodo.
    static ResultadoMetodo executar(
        const std::function<double(double)>& f,
        double a,
        double b,
        double tol = 1e-6,
        int maxIter = 1000
    ) {
        ResultadoMetodo res;
        res.nomeMetodo = "Bisseccao";
        res.intervaloFinal = {a, b};

        auto inicio = std::chrono::high_resolution_clock::now();

        const auto finalizarTempo = [&res, &inicio]() {
            const auto fim = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double, std::micro> duracao = fim - inicio;
            res.tempoMicrosegundos = duracao.count();
        };
        const auto avaliar = [&f, &res](double x) {
            ++res.avaliacoesFuncao;
            return f(x);
        };

        double fa = avaliar(a);
        double fb = avaliar(b);

        if (fa * fb > 0.0) {
            res.convergiu = false;
            res.status = MetodoStatus::SemMudancaDeSinal;
            res.mensagemErro = "Sinais iguais nas extremidades do intervalo [a, b]";
            finalizarTempo();
            return res;
        }

        double c = a;
        int iter = 0;

        while ((b - a) / 2.0 > tol && iter < maxIter) {
            iter++;
            c = a + (b - a) / 2.0;
            double fc = avaliar(c);

            if (std::abs(fc) < 1e-15 || (b - a) / 2.0 < tol) {
                if (std::abs(fc) < 1e-15) {
                    res.status = MetodoStatus::RaizExata;
                }
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

        res.raiz = c;
        res.fRaiz = avaliar(c);
        res.iteracoes = iter;
        res.convergiu = (iter < maxIter);
        res.intervaloFinal = {a, b};
        if (res.status == MetodoStatus::NaoExecutado) {
            res.status = res.convergiu ? MetodoStatus::ToleranciaIntervalo : MetodoStatus::MaximoDeIteracoes;
        }
        finalizarTempo();

        return res;
    }
};

#endif // BISSECCAO_HPP
