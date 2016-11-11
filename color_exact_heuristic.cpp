/*******************************************************************************
 * MC658 - Projeto e Análise de Algoritmos III - 2s2016
 * Prof: Flavio Keidi Miyazawa
 * PED: Mauro Henrique Mulati
 * Usa ideias e código de Rafael Arakaki e Flávio Keidi Miyazawa 
 ******************************************************************************/

/* IMPLEMENTE AS FUNCOES INDICADAS
 * DIGITE SEU RA: 148234
 * SUBMETA SOMENTE ESTE ARQUIVO */

#include <iostream>
#include <float.h>
#include <lemon/list_graph.h>
#include "mygraphlib.h"
#include "color_exact_heuristic.h"
#include "gurobi_c++.h"

int colorNaive(GraphData& gd, NodeIntMap& color, int& lowerBound, int& upperBound, int timeLimit);

//------------------------------------------------------------------------------

int colorExact(GraphData& gd, NodeIntMap& color, int& lowerBound, int& upperBound, int timeLimit) {

	int i, j, k;
	int u, v;
	bool used;

	// Associa um vertice a uma posicao

	k = 0;
	NodeIntMap nodes(gd.g);

	for (ListGraph::NodeIt n(tsp.g); n != INVALID; ++n)
		nodes[n] = k++;		

	// Inicializa o modelo

	upperBound = gd.n;
	GRBEnv* env = GRBEnv();
	GRBModel model = GRBModel(env);
	model.set(GRB_StringAttr_ModelName, "GraphColoringProblem");
	model.getEnv().set(GRB_DoubleParam_TimeLimit, timeLimit);
	model.getEnv().set(GRB_DoubleParam_Cutoff, upperBound);

	// Variaveis Y que indicam se a cor J eh selecionada ou nao

	GRBVars* y = new GRBVar[gd.n];
	
	for (j = 0; j < gd.n; j++)
		y[j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "y_"+itos(j));

	// Variaveis X que indicam se o vertice I possui a cor J

	GRBVars** x = new GRBVar*[gd.n];
	
	for (i = 0; i < gd.n; i++)
		x[i] = new GRBVars[gd.n]
		
	for (i = 0; i < gd.n; i++)
		for (j = 0; j < gd.n; j++) 
			x[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x_"+itos(i)+"_"+itos(j));

	// Indica funcao objetivo de minimizacao
	
	GRBLinExp obj = 0
	
	for (j = 0; j < gd.n; j++)
		obj += y[j]
	
	model.setObjective(obj,GRB_MINIMIZE);
	
	// Restricao: todo vertice deve possuir uma unica cor
	
	for (i = 0; i < gd.n; i++) {
		GRBLinExpr expr = 0;
		for (j = 0; j < gd.n; j++)
			expr += x[i][j];
		model.addConstr(expr == 1, "c_"+itos(i));	
	}

	// Restricao: um vertice so pode usar uma cor selecionada
	
	for (i = 0; i < gd.n; i++)
		for (j = 0; j < gd.n; j++)
			model.addConstr(x[i][j] <= y[j], "c_"+itos(i)+"_"+itos(j));	
	
	// Restricao: dois vertices adjacentes nao podem ter a mesma cor
	
	for (IncEdgeIt e(tsp.g, u); e != INVALID; ++e) {
		for (j = 0; j < gd.n; j++) {
			u = nodes[tsp.g.u(e)];
			v = nodes[tsp.g.v(e)];
			model.addConstr(x[u][j] + x[v][j] <= 1, "c_"+itos(u)+"_"+itos(v)+"_"+itos(j));
		}	
	}
	
	// Resolve o modelo
	
	model.optimize();

	// Retorna solucao ingenua

	if (model.get(GRB_IntAttr_Status) != GRB_OPTIMAL) 
		return colorNaive(gd, color, lowerBound, upperBound, timeLimit);

	// Retorna solucao otima

	lowerBound = 1;	
	
	for (j = 0; j < gd.n; j++) {
		used = false;
		for (NodeIt n(gd.g); n != INVALID; ++i) {
			if (x[nodes[n]][j].get(GRB_DoubleAttr_X) == 1) {
				color[n] = lowerBound;
				used = true;
			}
		}
		if (used)
			lowerBound++;
	}

	lowerBound--;	
	
	return 1;
	
}

//------------------------------------------------------------------------------

int colorHeuristic(GraphData& gd, NodeIntMap& color, int& lowerBound, int& upperBound, int timeLimit) {

	return colorNaive(gd, color, lowerBound, upperBound, timeLimit);

}

//------------------------------------------------------------------------------

/* Algoritmo ingênuo para o problema de coloração de vértices */
int colorNaive(GraphData& gd, NodeIntMap& color, int& lowerBound, int& upperBound, int timeLimit) {

   lowerBound = 1;
   int next = lowerBound;
	
	for(NodeIt i(gd.g); i != INVALID; ++i) {
      color[i] = next;
      next++;
	}
	next--;
	upperBound = next;
	
	return 0;

}
