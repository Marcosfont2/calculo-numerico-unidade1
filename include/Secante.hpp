#ifndef SECANTE_HPP
#define SECANTE_HPP

#include "Tipos.hpp"

#include <chrono>
#include <cmath>
#include <limits>

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
        const double indisponivel = std::numeric_limits<double>::quiet_NaN();
        res.intervaloFinal = {indisponivel, indisponivel};
        const auto inicio = std::chrono::high_resolution_clock::now();
        const auto finalizar = [&res, &inicio]() {
            const auto fim = std::chrono::high_resolution_clock::now();
            res.tempoMicrosegundos = std::chrono::duration<double, std::micro>(fim - inicio).count();
            return res;
        };
        const auto avaliar = [&f, &res](double x, double& valor) {
            ++res.avaliacoesFuncao;
            valor = f(x);
            return std::isfinite(valor);
        };

        if (!f || !std::isfinite(x0) || !std::isfinite(x1) || !std::isfinite(tol) || tol < 0.0 || maxIter <= 0) {
            res.status = MetodoStatus::ArgumentoInvalido;
            res.mensagemErro = "Funcao, aproximacoes, tolerancia ou maximo de iteracoes invalido";
            return finalizar();
        }

        double anterior = x0;
        double atual = x1;
        double fAnterior;
        double fAtual;
        if (!avaliar(anterior, fAnterior) || !avaliar(atual, fAtual)) {
            res.status = MetodoStatus::AvaliacaoNaoFinita;
            res.mensagemErro = "A funcao gerou um valor nao finito";
            return finalizar();
        }
        if (fAnterior == 0.0 || fAtual == 0.0) {
            res.raiz = fAnterior == 0.0 ? anterior : atual;
            res.fRaiz = fAnterior == 0.0 ? fAnterior : fAtual;
            res.status = MetodoStatus::RaizExata;
            res.convergiu = true;
            return finalizar();
        }

        for (int iteracao = 1; iteracao <= maxIter; ++iteracao) {
            const double denominador = fAtual - fAnterior;
            if (!std::isfinite(denominador) || denominador == 0.0) {
                res.raiz = atual;
                res.fRaiz = fAtual;
                res.iteracoes = iteracao - 1;
                res.status = MetodoStatus::FalhaNumerica;
                res.mensagemErro = "Denominador nulo ou nao finito na formula da secante";
                return finalizar();
            }
            const double proximo = atual - fAtual * (atual - anterior) / denominador;
            if (!std::isfinite(proximo)) {
                res.raiz = atual;
                res.fRaiz = fAtual;
                res.iteracoes = iteracao - 1;
                res.status = MetodoStatus::FalhaNumerica;
                res.mensagemErro = "A formula da secante gerou um valor nao finito";
                return finalizar();
            }
            double fProximo;
            if (!avaliar(proximo, fProximo)) {
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
            anterior = atual;
            fAnterior = fAtual;
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

#endif // SECANTE_HPP
