#ifndef ISOLAMENTO_HPP
#define ISOLAMENTO_HPP

#include "Tipos.hpp"
#include <iostream>
#include <vector>
#include <cmath>

class Isolamento {
public:
    // Realiza a busca por varredura no intervalo I com passo h
    static std::vector<Intervalo> buscarSubintervalos(const ProblemaFuncao& prob) {
        std::vector<Intervalo> subintervalos;
        double a = prob.intervaloInicial.a;
        double b = prob.intervaloInicial.b;
        double h = prob.passoH;

        double x_atual = a;
        while (x_atual < b) {
            double x_proximo = std::min(x_atual + h, b);
            
            double f_a = prob.f(x_atual);
            double f_b = prob.f(x_proximo);

            // Se f(a) * f(b) < 0, existe pelo menos uma raiz no intervalo
            if (f_a * f_b < 0.0) {
                subintervalos.push_back({x_atual, x_proximo});
            } else if (std::abs(f_a) < 1e-12) {
                // Caso o ponto amostrado seja exatamente uma raiz
                subintervalos.push_back({x_atual, x_atual});
            }

            x_atual = x_proximo;
        }

        return subintervalos;
    }
};

#endif // ISOLAMENTO_HPP