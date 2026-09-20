#include <iostream>
#include <vector>
#include <cmath>

#include "include/Tipos.hpp"
#include "include/Isolamento.hpp"
#include "include/Bisseccao.hpp"
#include "include/PontoFixo.hpp"
#include "include/Utils.hpp"

int main() {

    // Definição dos problemas propostos no enunciado do trabalho
    std::vector<ProblemaFuncao> problemas = {
        {
            "f1",
            "f1(x) = 2x^4 + 4x^3 + 3x^2 - 10x - 15",
            [](double x) { return 2*std::pow(x, 4) + 4*std::pow(x, 3) + 3*std::pow(x, 2) - 10*x - 15; },
            [](double x) { return 8*std::pow(x, 3) + 12*std::pow(x, 2) + 6*x - 10; },
            [](double x) { return (2*std::pow(x, 4) + 4*std::pow(x, 3) + 3*std::pow(x, 2) - 15) / 10.0; }, // para o ponto fixo
            {0.0, 3.0},
            0.6
        },
        {
            "f2",
            "f2(x) = x^5 - 2x^4 - 9x^3 + 22x^2 + 4x - 24",
            [](double x) { return std::pow(x, 5) - 2*std::pow(x, 4) - 9*std::pow(x, 3) + 22*std::pow(x, 2) + 4*x - 24; },
            [](double x) { return 5*std::pow(x, 4) - 8*std::pow(x, 3) - 27*std::pow(x, 2) + 44*x + 4; },
            [](double x) { return (std::pow(x, 5) - 2*std::pow(x, 4) - 9*std::pow(x, 3) + 22*std::pow(x, 2) - 24) / 4.0; }, // para o ponto fixo
            {0.0, 5.0},
            0.7
        },
        {
            "f3",
            "f3(x) = 5x^3 + x^2 - e^(1-2x) + cos(x) + 20",
            [](double x) { return 5*std::pow(x, 3) + std::pow(x, 2) - std::exp(1.0 - 2.0*x) + std::cos(x) + 20.0; },
            [](double x) { return 15*std::pow(x, 2) + 2*x + 2.0*std::exp(1.0 - 2.0*x) - std::sin(x); },
            {-5.0, 5.0},
            0.5
        },
        {
            "f4",
            "f4(x) = sen(x)*x + 4",
            [](double x) { return std::sin(x)*x + 4.0; },
            [](double x) { return std::cos(x)*x + std::sin(x); },
            {1.0, 5.0},
            0.5
        }
    };

    std::vector<ResultadoMetodo> todosResultados;

    for (const auto& prob : problemas) {
        std::cout << "Analisando: " << prob.expressao << "\n";
        std::cout << "\n";
        std::cout << "Intervalo: [" << prob.intervaloInicial.a << ", " << prob.intervaloInicial.b 
                  << "] | h = " << prob.passoH << "\n";
        std::cout << "\n";

        // Etapa 1: Isolamento
        auto subintervalos = Isolamento::buscarSubintervalos(prob);
        std::cout << "Raizes isoladas nos subintervalos:\n";
        std::cout << "\n";
        for (const auto& sub : subintervalos) {
            std::cout << "[" << sub.a << ", " << sub.b << "]\n";
        }

        // Etapa 2: Refinamento
        std::cout << "\nResultados do refinamento :\n";
        for (const auto& sub : subintervalos) {
            // Executa a Bissecção para cada subintervalo encontrado
            ResultadoMetodo resBisseccao = Bisseccao::executar(prob.f, sub.a, sub.b, 1e-6);
            Utils::imprimirResultadoTerminal(resBisseccao);
            todosResultados.push_back(resBisseccao);

            // Chamem seus métodos de refinamento aqui depois de implementados...
        }
        std::cout << "\n";
    }

}
