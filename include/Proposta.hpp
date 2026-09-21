#ifndef PROPOSTA_HPP
#define PROPOSTA_HPP

#include "Tipos.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>

// Reúne as opções usadas pelo método experimental proposto.
struct ProposedMethodOptions {
    double xAbsoluteTolerance = 1e-12;
    double xRelativeTolerance = 1e-12;
    double functionTolerance = 1e-12;
    std::size_t maxIterations = 100;
};

class ProposedMethod {
public:
    // Executa o método experimental que combina bissecção, falsa posição e extrapolação.
    // Entrada: função contínua, extremos a e b com sinais opostos e opções numéricas.
    // Saída: resultado com raiz aproximada, status, métricas e intervalo final.
    static ResultadoMetodo execute(
        const std::function<double(double)>& function,
        double a,
        double b,
        ProposedMethodOptions options = {}) {
        ResultadoMetodo result;
        result.nomeMetodo = "Metodo experimental proposto";
        result.intervaloFinal = {a, b};

        // Inicia a medição antes das validações para que até retornos por erro tenham tempo registrado.
        const auto start = std::chrono::high_resolution_clock::now();
        const auto finish = [&result, &start]() {
            const auto end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double, std::micro> elapsed = end - start;
            result.tempoMicrosegundos = elapsed.count();
            result.intervaloFinal = {result.intervaloFinal.a, result.intervaloFinal.b};
            return result;
        };

        // Primeiro confere se a função recebida e as tolerâncias permitem executar o método.
        if (!function || !validOptions(options)) {
            result.status = MetodoStatus::ArgumentoInvalido;
            result.mensagemErro = "Funcao ou opcoes invalidas";
            return finish();
        }
        // O algoritmo trabalha apenas com extremos finitos e ordenados para preservar o bracket.
        if (!std::isfinite(a) || !std::isfinite(b)) {
            result.status = MetodoStatus::EntradaNaoFinita;
            result.mensagemErro = "Os extremos do intervalo precisam ser finitos";
            return finish();
        }
        if (!(a < b)) {
            result.status = MetodoStatus::IntervaloInvalido;
            result.mensagemErro = "O intervalo deve obedecer a < b";
            return finish();
        }

        // Avalia os extremos uma única vez e guarda os valores para reutilizá-los nas iterações.
        double fa;
        double fb;
        if (!evaluate(function, a, result, fa) || !evaluate(function, b, result, fb)) {
            result.status = MetodoStatus::AvaliacaoNaoFinita;
            result.mensagemErro = "A funcao gerou um valor nao finito";
            return finish();
        }
        // Antes de procurar no interior, verifica se algum extremo já é uma raiz exata.
        if (fa == 0.0) {
            return finishExact(result, a, fa, finish);
        }
        if (fb == 0.0) {
            return finishExact(result, b, fb, finish);
        }
        // Sem sinais opostos, não há um bracket válido para o refinamento por mudança de sinal.
        if (!oppositeSigns(fa, fb)) {
            result.status = MetodoStatus::SemMudancaDeSinal;
            result.mensagemErro = "Nao existe mudanca de sinal nas extremidades";
            return finish();
        }
        // Um residual suficientemente pequeno em um extremo também encerra a busca.
        if (std::abs(fa) <= options.functionTolerance || std::abs(fb) <= options.functionTolerance) {
            finishBest(result, a, fa, b, fb, MetodoStatus::ToleranciaFuncao);
            return finish();
        }
        if (xToleranceMet(a, b, options)) {
            finishBest(result, a, fa, b, fb, MetodoStatus::ToleranciaIntervalo);
            return finish();
        }

        // Cada iteração reduz o intervalo usando os pontos de bissecção e falsa posição.
        for (std::size_t iteration = 1; iteration <= options.maxIterations; ++iteration) {
            result.iteracoes = static_cast<int>(iteration);

            // Calcula o ponto central. Se não existir double interno, não há como continuar progredindo.
            double xBis;
            if (!midpoint(a, b, xBis)) {
                finishBest(result, a, fa, b, fb, MetodoStatus::Estagnacao);
                return finish();
            }

            // Calcula o ponto da falsa posição a partir dos mesmos extremos.
            double xFalsePosition;
            const bool validFalsePosition = falsePositionPoint(a, b, fa, fb, xFalsePosition);
            double fBis;
            if (!evaluate(function, xBis, result, fBis)) {
                finishBest(result, a, fa, b, fb, MetodoStatus::AvaliacaoNaoFinita);
                result.mensagemErro = "A funcao gerou um valor nao finito";
                return finish();
            }
            if (fBis == 0.0) {
                return finishExact(result, xBis, fBis, finish);
            }
            if (std::abs(fBis) <= options.functionTolerance) {
                finishPoint(result, xBis, fBis, a, b, MetodoStatus::ToleranciaFuncao);
                return finish();
            }

            // Quando a falsa posição é inválida ou coincide com a bissecção, reduz apenas pela bissecção.
            if (!validFalsePosition || xFalsePosition == xBis) {
                ++result.fallbacksBisseccao;
                ++result.fallbacksFalsaPosicao;
                updateBracket(a, fa, b, fb, xBis, fBis);
                if (xToleranceMet(a, b, options)) {
                    finishBest(result, a, fa, b, fb, MetodoStatus::ToleranciaIntervalo);
                    return finish();
                }
                continue;
            }

            // Avalia o segundo ponto interno somente depois de confirmar que ele é diferente do ponto médio.
            double fFalsePosition;
            if (!evaluate(function, xFalsePosition, result, fFalsePosition)) {
                finishBest(result, a, fa, b, fb, MetodoStatus::AvaliacaoNaoFinita);
                result.mensagemErro = "A funcao gerou um valor nao finito";
                return finish();
            }
            if (fFalsePosition == 0.0) {
                return finishExact(result, xFalsePosition, fFalsePosition, finish);
            }
            if (std::abs(fFalsePosition) <= options.functionTolerance) {
                finishPoint(result, xFalsePosition, fFalsePosition, a, b, MetodoStatus::ToleranciaFuncao);
                return finish();
            }

            // Ordena os dois pontos e mantém cada valor de função ligado ao seu ponto correspondente.
            double xLeft = xBis;
            double fLeft = fBis;
            double xRight = xFalsePosition;
            double fRight = fFalsePosition;
            if (xRight < xLeft) {
                std::swap(xLeft, xRight);
                std::swap(fLeft, fRight);
            }

            // As duas amostras internas dividem o bracket em três regiões; a raiz deve estar em exatamente uma delas.
            const bool leftRegion = oppositeSigns(fa, fLeft);
            const bool middleRegion = oppositeSigns(fLeft, fRight);
            const bool rightRegion = oppositeSigns(fRight, fb);
            const int regions = static_cast<int>(leftRegion) + static_cast<int>(middleRegion) + static_cast<int>(rightRegion);
            if (regions != 1) {
                finishBest(result, a, fa, b, fb,
                    regions > 1 ? MetodoStatus::MultiplasMudancasDeSinal : MetodoStatus::FalhaNumerica);
                return finish();
            }

            // Na região central, os dois valores já foram avaliados e formam imediatamente o novo intervalo.
            if (middleRegion) {
                a = xLeft;
                fa = fLeft;
                b = xRight;
                fb = fRight;
                if (xToleranceMet(a, b, options)) {
                    finishBest(result, a, fa, b, fb, MetodoStatus::ToleranciaIntervalo);
                    return finish();
                }
                continue;
            }

            // Nas regiões externas, primeiro mantém somente a região que apresentou mudança de sinal.
            if (leftRegion) {
                b = xLeft;
                fb = fLeft;
            } else {
                a = xRight;
                fa = fRight;
            }
            if (xToleranceMet(a, b, options)) {
                finishBest(result, a, fa, b, fb, MetodoStatus::ToleranciaIntervalo);
                return finish();
            }

            // O chute avança a distância entre falsa posição e bissecção a partir da frente voltada para a raiz.
            const double delta = std::abs(xFalsePosition - xBis);
            double candidate = leftRegion ? xLeft - delta : xRight + delta;
            ++result.tentativasExtrapolacao;
            // Só aceita a extrapolação se ela permanecer finita e estritamente dentro do intervalo reduzido.
            if (std::isfinite(delta) && std::isfinite(candidate) && a < candidate && candidate < b) {
                ++result.extrapolacoesAceitas;
            } else {
                ++result.fallbacksBisseccao;
                ++result.fallbacksExtrapolacao;
                if (!midpoint(a, b, candidate)) {
                    finishBest(result, a, fa, b, fb, MetodoStatus::Estagnacao);
                    return finish();
                }
            }

            // Avalia o chute aceito, ou o ponto médio usado como fallback, e atualiza novamente o bracket.
            double fCandidate;
            if (!evaluate(function, candidate, result, fCandidate)) {
                finishBest(result, a, fa, b, fb, MetodoStatus::AvaliacaoNaoFinita);
                result.mensagemErro = "A funcao gerou um valor nao finito";
                return finish();
            }
            if (fCandidate == 0.0) {
                return finishExact(result, candidate, fCandidate, finish);
            }
            if (std::abs(fCandidate) <= options.functionTolerance) {
                finishPoint(result, candidate, fCandidate, a, b, MetodoStatus::ToleranciaFuncao);
                return finish();
            }
            updateBracket(a, fa, b, fb, candidate, fCandidate);
            if (xToleranceMet(a, b, options)) {
                finishBest(result, a, fa, b, fb, MetodoStatus::ToleranciaIntervalo);
                return finish();
            }
        }

        // Se nenhuma condição anterior encerrou a busca, devolve o melhor extremo após atingir o limite configurado.
        finishBest(result, a, fa, b, fb, MetodoStatus::MaximoDeIteracoes);
        return finish();
    }

private:
    // Verifica se as opções numéricas têm valores utilizáveis pelo algoritmo.
    // Entrada: conjunto de tolerâncias e máximo de iterações. Saída: verdadeiro quando são válidas.
    static bool validOptions(const ProposedMethodOptions& options) {
        return std::isfinite(options.xAbsoluteTolerance) && options.xAbsoluteTolerance >= 0.0 &&
               std::isfinite(options.xRelativeTolerance) && options.xRelativeTolerance >= 0.0 &&
               std::isfinite(options.functionTolerance) && options.functionTolerance >= 0.0 &&
               options.maxIterations > 0;
    }

    // Detecta sinais opostos sem multiplicar os valores e evitar problemas de overflow.
    // Entrada: dois valores reais não nulos. Saída: verdadeiro quando possuem sinais diferentes.
    static bool oppositeSigns(double left, double right) {
        return (left < 0.0 && right > 0.0) || (left > 0.0 && right < 0.0);
    }

    // Avalia a função e contabiliza a chamada real realizada pelo método.
    // Entrada: função, ponto de avaliação, resultado para métrica e referência de saída. Saída: falso para valor não finito.
    static bool evaluate(const std::function<double(double)>& function, double x, ResultadoMetodo& result, double& value) {
        ++result.avaliacoesFuncao;
        value = function(x);
        return std::isfinite(value);
    }

    // Calcula o ponto médio com uma alternativa quando a diferença b-a não cabe em double.
    // Entrada: extremos ordenados a e b. Saída: verdadeiro e o ponto estritamente interno quando possível.
    static bool midpoint(double a, double b, double& value) {
        const double span = b - a;
        value = std::isfinite(span) ? a + span / 2.0 : a / 2.0 + b / 2.0;
        return std::isfinite(value) && a < value && value < b;
    }

    // Calcula o ponto de falsa posição e confirma que ele pertence ao intervalo aberto.
    // Entrada: extremos a e b e valores fa e fb. Saída: verdadeiro e o ponto calculado quando a divisão é válida.
    static bool falsePositionPoint(double a, double b, double fa, double fb, double& value) {
        const double span = b - a;
        const double denominator = fb - fa;
        const double numerator = fa * span;
        if (!std::isfinite(span) || !std::isfinite(denominator) || denominator == 0.0 || !std::isfinite(numerator)) {
            return false;
        }
        const double quotient = numerator / denominator;
        value = a - quotient;
        return std::isfinite(quotient) && std::isfinite(value) && a < value && value < b;
    }

    // Verifica a tolerância combinada do intervalo.
    // Entrada: extremos atuais e opções do método. Saída: verdadeiro quando a largura já é suficiente.
    static bool xToleranceMet(double a, double b, const ProposedMethodOptions& options) {
        const double width = b - a;
        const double scale = std::max(std::abs(a), std::abs(b));
        if (std::isfinite(width)) {
            const double tolerance = options.xAbsoluteTolerance + options.xRelativeTolerance * scale;
            return std::isinf(tolerance) || width <= tolerance;
        }
        return scale != 0.0 && b / scale - a / scale <= options.xAbsoluteTolerance / scale + options.xRelativeTolerance;
    }

    // Atualiza o bracket mantendo a região que preserva a mudança de sinal.
    // Entrada: extremos e valores atuais, além de um ponto interno avaliado. Saída: extremos e valores atualizados por referência.
    static void updateBracket(double& a, double& fa, double& b, double& fb, double x, double fx) {
        if (oppositeSigns(fa, fx)) {
            b = x;
            fb = fx;
        } else {
            a = x;
            fa = fx;
        }
    }

    // Registra uma raiz encontrada exatamente durante uma avaliação.
    // Entrada: resultado, ponto e valor nulo da função, além da finalização do cronômetro. Saída: resultado finalizado.
    template <typename Finish>
    static ResultadoMetodo finishExact(ResultadoMetodo& result, double x, double fx, Finish finish) {
        result.status = MetodoStatus::RaizExata;
        result.convergiu = true;
        result.raiz = x;
        result.fRaiz = fx;
        result.intervaloFinal = {x, x};
        return finish();
    }

    // Registra o extremo de menor residual para uma parada que mantém o bracket.
    // Entrada: resultado, extremos com seus valores e status final. Saída: resultado preenchido com o melhor extremo.
    static void finishBest(ResultadoMetodo& result, double a, double fa, double b, double fb, MetodoStatus status) {
        result.status = status;
        result.convergiu = status == MetodoStatus::ToleranciaFuncao || status == MetodoStatus::ToleranciaIntervalo;
        result.intervaloFinal = {a, b};
        if (std::abs(fa) <= std::abs(fb)) {
            result.raiz = a;
            result.fRaiz = fa;
        } else {
            result.raiz = b;
            result.fRaiz = fb;
        }
    }

    // Registra uma parada por residual sem mudar o bracket que a originou.
    // Entrada: resultado, ponto avaliado, intervalo atual e status. Saída: resultado preenchido com a aproximação.
    static void finishPoint(ResultadoMetodo& result, double x, double fx, double a, double b, MetodoStatus status) {
        result.status = status;
        result.convergiu = true;
        result.raiz = x;
        result.fRaiz = fx;
        result.intervaloFinal = {a, b};
    }
};

#endif // PROPOSTA_HPP
