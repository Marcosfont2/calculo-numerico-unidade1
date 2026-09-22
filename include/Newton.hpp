#ifndef NEWTON_HPP
#define NEWTON_HPP

#include "Tipos.hpp"

#include <chrono>
#include <cmath>
#include <limits>

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
        const double indisponivel = std::numeric_limits<double>::quiet_NaN();
        res.intervaloFinal = {indisponivel, indisponivel};
        const auto inicio = std::chrono::high_resolution_clock::now();
        const auto finalizar = [&res, &inicio]() {
            const auto fim = std::chrono::high_resolution_clock::now();
            res.tempoMicrosegundos = std::chrono::duration<double, std::micro>(fim - inicio).count();
            return res;
        };
        const auto avaliarFuncao = [&f, &res](double x, double& valor) {
            ++res.avaliacoesFuncao;
            valor = f(x);
            return std::isfinite(valor);
        };
        const auto avaliarDerivada = [&df, &res](double x, double& valor) {
            ++res.avaliacoesDerivada;
            valor = df(x);
            return std::isfinite(valor);
        };

        if (!f || !df || !std::isfinite(x0) || !std::isfinite(tol) || tol < 0.0 || maxIter <= 0) {
            res.status = MetodoStatus::ArgumentoInvalido;
            res.mensagemErro = "Funcao, derivada, aproximacao, tolerancia ou maximo de iteracoes invalido";
            return finalizar();
        }

        double atual = x0;
        double fAtual;
        if (!avaliarFuncao(atual, fAtual)) {
            res.status = MetodoStatus::AvaliacaoNaoFinita;
            res.mensagemErro = "A funcao gerou um valor nao finito";
            return finalizar();
        }
        if (fAtual == 0.0) {
            res.raiz = atual;
            res.fRaiz = fAtual;
            res.status = MetodoStatus::RaizExata;
            res.convergiu = true;
            return finalizar();
        }

        for (int iteracao = 1; iteracao <= maxIter; ++iteracao) {
            double derivada;
            if (!avaliarDerivada(atual, derivada)) {
                res.raiz = atual;
                res.fRaiz = fAtual;
                res.iteracoes = iteracao - 1;
                res.status = MetodoStatus::AvaliacaoNaoFinita;
                res.mensagemErro = "A derivada gerou um valor nao finito";
                return finalizar();
            }
            if (derivada == 0.0) {
                res.raiz = atual;
                res.fRaiz = fAtual;
                res.iteracoes = iteracao - 1;
                res.status = MetodoStatus::FalhaNumerica;
                res.mensagemErro = "Derivada nula na formula de Newton";
                return finalizar();
            }
            const double proximo = atual - fAtual / derivada;
            if (!std::isfinite(proximo)) {
                res.raiz = atual;
                res.fRaiz = fAtual;
                res.iteracoes = iteracao - 1;
                res.status = MetodoStatus::FalhaNumerica;
                res.mensagemErro = "O calculo de Newton gerou um valor nao finito";
                return finalizar();
            }
            double fProximo;
            if (!avaliarFuncao(proximo, fProximo)) {
                res.raiz = atual;
                res.fRaiz = fAtual;
                res.iteracoes = iteracao - 1;
                res.status = MetodoStatus::AvaliacaoNaoFinita;
                res.mensagemErro = "A funcao gerou um valor nao finito";
                return finalizar();
            }

            res.raiz = proximo;
            res.fRaiz = fProximo;
            res.iteracoes = iteracao;
            if (fProximo == 0.0) {
                res.status = MetodoStatus::RaizExata;
                res.convergiu = true;
                return finalizar();
            }
            if (std::abs(fProximo) <= tol) {
                res.status = MetodoStatus::ToleranciaFuncao;
                res.convergiu = true;
                return finalizar();
            }
            if (std::abs(proximo - atual) <= tol) {
                res.status = MetodoStatus::ToleranciaPasso;
                res.convergiu = true;
                return finalizar();
            }
            atual = proximo;
            fAtual = fProximo;
        }

        res.raiz = atual;
        res.fRaiz = fAtual;
        res.iteracoes = maxIter;
        res.status = MetodoStatus::MaximoDeIteracoes;
        res.mensagemErro = "Numero maximo de iteracoes atingido sem convergencia";
        return finalizar();
    }
};

#endif // NEWTON_HPP
