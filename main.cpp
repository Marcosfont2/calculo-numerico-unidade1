#include <iostream>
#include "include/Tipos.hpp"
#include "include/Isolamento.hpp"
#include "include/Bisseccao.hpp"
#include "include/PontoFixo.hpp"
#include "include/Problemas.hpp"
#include "include/Proposta.hpp"
#include "include/Utils.hpp"

int main() {

    // Os problemas ficam centralizados em Problems para que todos os métodos reutilizem as mesmas funções.
    const auto& problemas = Problems::all();

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

            ProposedMethodOptions opcoesProposta;
            opcoesProposta.xAbsoluteTolerance = 1e-6;
            opcoesProposta.xRelativeTolerance = 0.0;
            opcoesProposta.functionTolerance = 1e-12;
            opcoesProposta.maxIterations = 1000;
            ResultadoMetodo resProposta = ProposedMethod::execute(prob.f, sub.a, sub.b, opcoesProposta);
            Utils::imprimirResultadoTerminal(resProposta);
            todosResultados.push_back(resProposta);
        }
        std::cout << "\n";
    }

}
