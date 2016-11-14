/*******************************************************************************
 * MC658 - Projeto e Análise de Algoritmos III - 2s2016
 * Prof: Flavio Keidi Miyazawa
 * PED: Mauro Henrique Mulati
 * Usa ideias e código de Rafael Arakaki e Flávio Keidi Miyazawa 
 ******************************************************************************/

// Wendrey Lustosa Cardoso 148234

#include <iostream>
#include <float.h>
#include <time.h>
#include <stdlib.h>
#include <lemon/list_graph.h>
#include "mygraphlib.h"
#include "color_exact_heuristic.h"
#include "gurobi_c++.h"

using namespace std;

int colorNaive(GraphData& gd, NodeIntMap& color, int& lowerBound, int& upperBound, int timeLimit);

//------------------------------------------------------------------------------

/* Algoritmo ingenuo para o problema de coloracao de vertices */

int colorExact(GraphData& gd, NodeIntMap& color, int& lowerBound, int& upperBound, int timeLimit) {

try {

	int i, j, k;
	bool used;

	// Associa um vertice a uma posicao

	k = 0;
	NodeIntMap nodes(gd.g);

	for (ListGraph::NodeIt n(gd.g); n != INVALID; ++n)
		nodes[n] = k++;		

	// Inicializa o modelo

	lowerBound = 1;
	upperBound = gd.n + 1;
	GRBEnv env = GRBEnv();
	GRBModel model = GRBModel(env);
	model.set(GRB_StringAttr_ModelName, "GraphColoringProblem");
	model.getEnv().set(GRB_DoubleParam_TimeLimit, timeLimit);
	model.getEnv().set(GRB_DoubleParam_Cutoff, upperBound);

	// Variaveis Y que indicam se a cor J eh selecionada ou nao

	GRBVar* y = new GRBVar[gd.n];
	
	for (j = 0; j < gd.n; j++)
		y[j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "");

	// Variaveis X que indicam se o vertice I possui a cor J

	GRBVar** x = new GRBVar*[gd.n];
	
	for (i = 0; i < gd.n; i++)
		x[i] = new GRBVar[gd.n];
		
	for (i = 0; i < gd.n; i++)
		for (j = 0; j < gd.n; j++) 
			x[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "");

	// Indica funcao objetivo de minimizacao
	
	GRBLinExpr obj = 0;
	
	for (j = 0; j < gd.n; j++)
		obj += y[j];
	
	model.setObjective(obj,GRB_MINIMIZE);
	
	// Restricao: todo vertice deve possuir uma unica cor
	
	for (i = 0; i < gd.n; i++) {
		GRBLinExpr expr = 0;
		for (j = 0; j < gd.n; j++)
			expr += x[i][j];
		model.addConstr(expr == 1, "");	
	}

	// Restricao: um vertice so pode usar uma cor selecionada
	
	for (i = 0; i < gd.n; i++)
		for (j = 0; j < gd.n; j++)
			model.addConstr(x[i][j] <= y[j], "");	
	
	// Restricao: dois vertices adjacentes nao podem ter a mesma cor
	
	for (EdgeIt e(gd.g); e != INVALID; ++e)
		for (j = 0; j < gd.n; j++)
			model.addConstr(x[nodes[gd.g.u(e)]][j] + x[nodes[gd.g.v(e)]][j] <= 1, "");
	
	// Resolve o modelo
	
	model.update();
	model.optimize();

	// Atribui a solucao

	k = 1;	
	
	for (j = 0; j < gd.n; j++) {
		used = false;
		for (NodeIt n(gd.g); n != INVALID; ++n) {
			if (x[nodes[n]][j].get(GRB_DoubleAttr_X) == 1) {
				color[n] = k;
				used = true;
			}
		}
		if (used)
			k++;
	}
	
	lowerBound = model.get(GRB_DoubleAttr_ObjBound);
	upperBound = model.get(GRB_DoubleAttr_ObjVal);

	if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL)
		return 1;	
		
	return 0;
	
}

catch (GRBException e) {

	cout << "Error code : " << e.getErrorCode() << endl;
	cout << e.getMessage() << endl;

}
	
return colorNaive(gd, color, lowerBound, upperBound, timeLimit);
	
}

//------------------------------------------------------------------------------

/* Algoritmo ingenuo para o problema de coloracao de vertices */

int colorHeuristic(GraphData& gd, NodeIntMap& color, int& lowerBound, int& upperBound, int timeLimit) {

try {

	int i, j, k;
	Node v;
	double max;
	bool used, done;
	clock_t t = clock();

	// Associa um vertice a uma posicao

	k = 0;
	NodeIntMap nodes(gd.g);
	NodeBoolMap fixed(gd.g);

	for (ListGraph::NodeIt n(gd.g); n != INVALID; ++n) {
		nodes[n] = k++;		
		fixed[n] = false;
	}

	// Inicializa o modelo

	lowerBound = 1;
	upperBound = gd.n + 1;
	GRBEnv env = GRBEnv();
	GRBModel model = GRBModel(env);
	model.set(GRB_StringAttr_ModelName, "GraphColoringProblem");
	model.getEnv().set(GRB_DoubleParam_TimeLimit, timeLimit);
	model.getEnv().set(GRB_DoubleParam_Cutoff, upperBound);
	
	// Variaveis Y que indicam se a cor J eh selecionada ou nao

	GRBVar* y = new GRBVar[gd.n];
	
	for (j = 0; j < gd.n; j++)
		y[j] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "");

	// Variaveis X que indicam se o vertice I possui a cor J

	GRBVar** x = new GRBVar*[gd.n];
	
	for (i = 0; i < gd.n; i++)
		x[i] = new GRBVar[gd.n];
		
	for (i = 0; i < gd.n; i++)
		for (j = 0; j < gd.n; j++) 
			x[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "");

	// Indica funcao objetivo de minimizacao
	
	GRBLinExpr obj = 0;
	
	for (j = 0; j < gd.n; j++)
		obj += y[j];
	
	model.setObjective(obj,GRB_MINIMIZE);

	// Restricao: todo vertice deve possuir uma unica cor
	
	for (i = 0; i < gd.n; i++) {
		GRBLinExpr expr = 0;
		for (j = 0; j < gd.n; j++)
			expr += x[i][j];
		model.addConstr(expr == 1, "");	
	}

	// Restricao: um vertice so pode usar uma cor selecionada
	
	for (i = 0; i < gd.n; i++)
		for (j = 0; j < gd.n; j++)
			model.addConstr(x[i][j] <= y[j], "");	
	
	// Restricao: dois vertices adjacentes nao podem ter a mesma cor
	
	for (EdgeIt e(gd.g); e != INVALID; ++e)
		for (j = 0; j < gd.n; j++)
			model.addConstr(x[nodes[gd.g.u(e)]][j] + x[nodes[gd.g.v(e)]][j] <= 1, "");
	
	// Resolve o modelo de PL com heuristica
	
	done = false;
	
	while (done == false) {
			
		// Resolve o modelo
	
		model.getEnv().set(GRB_DoubleParam_TimeLimit, timeLimit);
		model.update();
		model.optimize();

		// Trata inviabilidade
		
		if (model.get(GRB_IntAttr_Status) == GRB_INFEASIBLE) 
			break;
						
		// Trata restricao de tempo

		if ((timeLimit -= (clock() - t) / CLOCKS_PER_SEC) < 0) {
			cout << "---------- TIME LIMIT : " << TimeLimit << " ----------" << endl;
			break;
	}
		// Heuristica: adiciona nova restricao no modelo

		done = true;
		max = 0;
		k = -1;

		for (j = 0; j < gd.n; j++) {
			for (NodeIt n(gd.g); n != INVALID; ++n) {

				if (fixed[n] == false) {

					done = false;

					if (x[nodes[n]][j].get(GRB_DoubleAttr_X) > 0.5) {
						model.addConstr(x[nodes[n]][j] == 1, "");
						fixed[n] = true;
					}

					else if (x[nodes[n]][j].get(GRB_DoubleAttr_X) > max) {
						max = x[nodes[n]][j].get(GRB_DoubleAttr_X);
						v = n; k = j;
					}
				}

			}
		}
		
		if (k >= 0) {
			model.addConstr(x[nodes[v]][k] == 1, "");
			fixed[v] = true;
		}

		// Atribui solucao

		upperBound = 1;	
	
		for (j = 0; j < gd.n; j++) {
			used = false;
			for (NodeIt n(gd.g); n != INVALID; ++n) {
				if (x[nodes[n]][j].get(GRB_DoubleAttr_X) == 1) {
					color[n] = upperBound;
					used = true;
				}
			}
			if (used)
				upperBound++;
		}
	
		for (NodeIt n(gd.g); n != INVALID; ++n)
			if (color[n] == 0)
				color[n] = upperBound++;
	
		upperBound--;

	}
	
	if (done == false)
		return colorNaive(gd, color, lowerBound, upperBound, timeLimit);
			
	return 0;

}

catch (GRBException e) {

	cout << "Error code : " << e.getErrorCode() << endl;
	cout << e.getMessage() << endl;

}

return colorNaive(gd, color, lowerBound, upperBound, timeLimit);

}

//------------------------------------------------------------------------------

/* Algoritmo ingenuo para o problema de coloracao de vertices */

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

