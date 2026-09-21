#ifndef PROBLEMAS_HPP
#define PROBLEMAS_HPP

#include "Tipos.hpp"

#include <cmath>
#include <vector>

namespace Problems {

// Calcula a primeira função do enunciado.
// Entrada: valor real x. Saída: 2x^4 + 4x^3 + 3x^2 - 10x - 15.
inline double f1(double x) {
    return 2.0 * std::pow(x, 4) + 4.0 * std::pow(x, 3) + 3.0 * std::pow(x, 2) - 10.0 * x - 15.0;
}

// Calcula a derivada de f1 para métodos que precisam dela.
// Entrada: valor real x. Saída: f1'(x).
inline double df1(double x) {
    return 8.0 * std::pow(x, 3) + 12.0 * std::pow(x, 2) + 6.0 * x - 10.0;
}

// Calcula a função de iteração escolhida para f1 no método do ponto fixo.
// Entrada: aproximação atual x. Saída: próxima aproximação phi(x).
inline double phi1(double x) {
    return std::sqrt((10.0 * x + 15.0) / (2.0 * x * x + 4.0 * x + 3.0));
}

// Calcula a segunda função do enunciado.
// Entrada: valor real x. Saída: x^5 - 2x^4 - 9x^3 + 22x^2 + 4x - 24.
inline double f2(double x) {
    return std::pow(x, 5) - 2.0 * std::pow(x, 4) - 9.0 * std::pow(x, 3) +
           22.0 * std::pow(x, 2) + 4.0 * x - 24.0;
}

// Calcula a derivada de f2 para métodos que precisam dela.
// Entrada: valor real x. Saída: f2'(x).
inline double df2(double x) {
    return 5.0 * std::pow(x, 4) - 8.0 * std::pow(x, 3) - 27.0 * std::pow(x, 2) + 44.0 * x + 4.0;
}

// Calcula a função de iteração escolhida para f2 no método do ponto fixo.
// Entrada: aproximação atual x. Saída: próxima aproximação phi(x).
inline double phi2(double x) {
    return x - std::cbrt(f2(x) / 12.0);
}

// Calcula a terceira função do enunciado.
// Entrada: valor real x. Saída: 5x^3 + x^2 - e^(1-2x) + cos(x) + 20.
inline double f3(double x) {
    return 5.0 * std::pow(x, 3) + std::pow(x, 2) - std::exp(1.0 - 2.0 * x) + std::cos(x) + 20.0;
}

// Calcula a derivada de f3 para métodos que precisam dela.
// Entrada: valor real x. Saída: f3'(x).
inline double df3(double x) {
    return 15.0 * std::pow(x, 2) + 2.0 * x + 2.0 * std::exp(1.0 - 2.0 * x) - std::sin(x);
}

// Calcula a função de iteração escolhida para f3 no método do ponto fixo
// Entrada: aproximação atual x. Saída: próxima aproximação phi(x).
inline double phi3(double x){
    return (1.0 - std::log(5.0 * std::pow(x, 3) + x * x + std::cos(x) + 20.0)) / 2.0;
}

// Calcula a quarta função do enunciado.
// Entrada: valor real x. Saída: sen(x)x + 4.
inline double f4(double x) {
    return std::sin(x) * x + 4.0;
}

// Calcula a derivada de f4 para métodos que precisam dela.
// Entrada: valor real x. Saída: f4'(x).
inline double df4(double x) {
    return std::cos(x) * x + std::sin(x);
}

// Calcula a função de iteração escolhida para f4 no método do ponto fixo.
// Entrada: aproximação atual x. Saída: próxima aproximação phi(x).
inline double phi4(double x){
    return M_PI + std::asin(4.0 / x);
}

// Fornece todos os problemas definidos no enunciado em uma única fonte de dados.
// Entrada: não recebe argumentos. Saída: referências às funções, derivadas, intervalos e passos h.
inline const std::vector<ProblemaFuncao>& all() {
    static const std::vector<ProblemaFuncao> problems = {
        {"f1", "f1(x) = 2x^4 + 4x^3 + 3x^2 - 10x - 15", f1, df1, phi1, {0.0, 3.0}, 0.6},
        {"f2", "f2(x) = x^5 - 2x^4 - 9x^3 + 22x^2 + 4x - 24", f2, df2, phi2, {0.0, 5.0}, 0.7},
        {"f3", "f3(x) = 5x^3 + x^2 - e^(1-2x) + cos(x) + 20", f3, df3, phi3, {-5.0, 5.0}, 0.5},
        {"f4", "f4(x) = sen(x)*x + 4", f4, df4, phi4, {1.0, 5.0}, 0.5}
    };
    return problems;
}

} // namespace Problems

#endif // PROBLEMAS_HPP
