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

	// Retorna solucao ingenua

	if (model.get(GRB_IntAttr_Status) != GRB_OPTIMAL) 
		return colorNaive(gd, color, lowerBound, upperBound, timeLimit);

	// Retorna solucao otima

	lowerBound = 1;	
	
	for (j = 0; j < gd.n; j++) {
		used = false;
		for (NodeIt n(gd.g); n != INVALID; ++n) {
			if (x[nodes[n]][j].get(GRB_DoubleAttr_X) == 1) {
				color[n] = lowerBound;
				used = true;
			}
		}
		if (used)
			lowerBound++;
	}

	lowerBound--;	
	
	lowerBound = model.get(GRB_DoubleAttr_ObjBound);
	upperBound = model.get(GRB_DoubleAttr_ObjVal);

	return 1;
	
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

	upperBound = gd.n;
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

		cout << "---------- ---------- RESOLVE O MODELO ---------- ----------" << endl;

		// Restricao de tempo 
		
		if ((timeLimit -= (clock() - t) / CLOCKS_PER_SEC) < 0)
			return colorNaive(gd, color, lowerBound, upperBound, timeLimit);
			
		// Resolve o modelo
	
		model.getEnv().set(GRB_DoubleParam_TimeLimit, timeLimit);
		model.update();
		model.optimize();
		
		// Retorna solucao ingenua

		if (model.get(GRB_IntAttr_Status) != GRB_OPTIMAL) 
			return colorNaive(gd, color, lowerBound, upperBound, timeLimit);
						
		// Heuristica: atribui pelo menos uma cor nova na solucao

		done = true;
		max = 0;
		k = -1;

		for (j = 0; j < gd.n; j++) {
			for (NodeIt n(gd.g); n != INVALID; ++n) {
				if (x[nodes[n]][j].get(GRB_DoubleAttr_X) > 0.5 && fixed[n] == false) {
					model.addConstr(x[nodes[n]][j] == 1, "");
					fixed[n] = true;
				}
				else if (x[nodes[n]][j].get(GRB_DoubleAttr_X) > max) {
					max = x[nodes[n]][j].get(GRB_DoubleAttr_X);
					v = n; k = j;
				}
			}
		}
		
		// Restricao: atribui alguma cor de maior valor 

		if (k >= 0) {
			model.addConstr(x[nodes[v]][k] == 1, "");		
			fixed[v] = true;
		}
	
	}
	
	// Retorna solucao otima

	cout << "---------- ---------- ATRIBUI SOLUCAO ---------- ----------" << endl;

	lowerBound = 1;	
	
	for (j = 0; j < gd.n; j++) {
		used = false;
		for (NodeIt n(gd.g); n != INVALID; ++n) {
			if (x[nodes[n]][j].get(GRB_DoubleAttr_X) == 1) {
				color[n] = lowerBound;
				used = true;
			}
		}
		if (used)
			lowerBound++;
	}

	lowerBound--;	
		
//	lowerBound = model.get(GRB_DoubleAttr_ObjBound);
//	upperBound = model.get(GRB_DoubleAttr_ObjVal);
	
	return 1;

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

