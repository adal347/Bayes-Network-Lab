#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <utility>

using namespace std;

typedef pair<string,double> P;

/**
 * Struct Node to add nodes at the Bayesian Network
 *
 * Attributes
 * ----------
 * Parents: The parents of the node. For 0 or N-parents;
 *          if the vector is empty, the event is Independient.
 * Probability_table: Probability table for actual node. Key is X in P(X).
 * Name: Name of the node.
 */
struct node {
	std::vector<struct node*> parents;
	std::map<string, double> probability_table;
	string name;
};

/**
 * Function to create the nodes for the bayesian network
 *
 * Attributes
 * ----------
 * Nodes : we add the node in map to know what are the nodes
 * Buffer : to retrieve the input
 */
void create_nodes(map<string, struct node*> &nodes, string buffer) {

  // The builder to build tha name string.
  string builder;
  // Retrieve the line.
	getline(cin, buffer);

  for(int i = 0; i < buffer.length(); i++){
    // if we find ',' we add the node
    if(buffer[i]==',') {
      nodes[builder] = new node;
			nodes[builder]->name = builder;
    } else if(i == (buffer.length() - 1)) {
      // else if is the end we add the last node
			builder += buffer[i];
			nodes[builder] = new node;
			nodes[builder]->name = builder;
    } else if(buffer[i]==' ') {
			builder="";
		} else {
			builder += buffer[i];
		}
	}
}

/**
 * Function to create bayes network
 *
 * Attributes
 * ----------
 * Nodes : The map of nodes to add their Probabilities.
 * Buffer :  to retrieve the input.
 */
void create_bayes_network(map<string, struct node*> &nodes, string buffer) {

  string builder, aux;
	char devourer;
	double readerProb;

	while(buffer.compare("") != 0) {
    getline(cin, buffer);
		builder = "";
		bool indexNode = false;
    for(int i = 0; i < buffer.length(); i++) {
      if(buffer[i] == '|'){
        // if find '|' the node have a parent
        indexNode = true;
				aux = builder;
				builder="";
			}
      if(buffer[i] == '+' || buffer[i] == '-' || buffer[i] == ' ' || buffer[i] == '=' || buffer[i] == ',' || buffer[i] == '|' || (buffer[i] >= 48 && buffer[i] <= 57));
			else{
        // Start to construct the string key for the map of nodes
        builder += buffer[i];
			}
      // To make the conection in base of the dependece
			if(indexNode) {
        if(buffer[i] == ',') {
          // To create N parents
          (nodes[aux])->parents.push_back(nodes[builder]);
					builder="";
        } else if (buffer[i] == '=') {
          // If there is only one parent
					(nodes[aux])->parents.push_back(nodes[builder]);
					builder="";
				}
			}
			if(buffer[i] == '=') {
        if(!indexNode){
          // The node is root P(X) is independient
					aux=builder;
				}
        // Finish the creation of the edges
				break;
			}
		}

    //erase duplicated nodes
    sort( (nodes[aux])->parents.begin(), (nodes[aux])->parents.end() );
		(nodes[aux])->parents.erase( unique( (nodes[aux])->parents.begin(), (nodes[aux])->parents.end() ), (nodes[aux])->parents.end() );

		// Create the probability table
		builder="";
		for(int i = 0; i < buffer.length(); i++) {
			if(buffer[i] == '=') {
				break;
			} else if(buffer[i] == ' ' && buffer[i + 1] == '=');
        // To ignore the last blank space " ="
			else {
				builder+=buffer[i];
			}
		}

		if(builder.compare("") != 0) {
			stringstream extract(buffer);
			do {
        // To eat all chars
				extract >> devourer;
			} while(devourer!='=');

			extract >> readerProb;

      // Store the true values P(+X|Y)
			(nodes[aux])->probability_table[builder]=readerProb;
      // Store the true values P(-X|Y)
			builder[0]='-';
			(nodes[aux])->probability_table[builder]=1 - readerProb;
		}
	}
}

/**
 * Function to find hidden nodes in the joint
 *
 * Attributes
 * ----------
 * Nodes : The map of nodes to add their Probabilities.
 * key_node : The key node.
 */
string hidden_nodes(map<string, struct node*> nodes, string key_node) {

  string builder = "";

  if(nodes[key_node]->parents.empty()) {
		builder += key_node;
	} else {
		for(int i = 0; i < nodes[key_node]->parents.size(); i++) {
			builder += hidden_nodes(nodes, nodes[key_node]->parents[i]->name) + " ";
		}
		builder += key_node;
	}
	return builder;
}

/**
 * Function for the chain rule
 *
 * Attributes
 * ----------
 * Nodes : The map of nodes to add their Probabilities.
 * Chain_probability : The string probability of chain rule.
 */
double chain_rule(map<string, struct node*> nodes,string chain_probability){

  string builder, name;
	char signs[] = "+-";
	std::vector<string> index, statements, probs;
  // To remove all ','
	chain_probability.erase (std::remove(chain_probability.begin(), chain_probability.end(), ','), chain_probability.end());
	stringstream extract(chain_probability);
	double acum = 1.0;

  while(extract >> builder) {
		statements.push_back(builder);
		for (int i = 0; i < strlen(signs); i++) {
	      builder.erase (std::remove(builder.begin(), builder.end(), signs[i]), builder.end());
	  	}
	  	index.push_back(builder);
	}
	for (int i = 0; i < index.size(); i++) {
		std::map<string, double> copyMap = nodes[index[i]]->probability_table;
		for (std::map<string,double>::iterator ite = copyMap.begin(); ite !=  copyMap.end(); ++ite) {
			if (ite->first.find(statements[i]) != std::string::npos) {
			    probs.push_back(ite->first);
			}
		}
		if(!nodes[index[i]]->parents.empty()) {
			int dependeces = nodes[index[i]]->parents.size();
			int indexProbs;
			for(int j = 0; j < probs.size(); j++){
				int count = 0;
				for (int k = 0; k < statements.size(); k++) {
					if(probs[j].find(statements[k]) != std::string::npos){
						count++;
					}
				}
				if(count == (dependeces + 1)) {
					indexProbs = j;
					break;
				}
			}
			acum *= copyMap[probs[indexProbs]];
		} else {
			acum *= copyMap[probs[0]];
		}
		while(!probs.empty()){
			probs.pop_back();
		}
	}
	return acum;
}

/**
 * Function for the sumatory
 *
 * Attributes
 * ----------
 * Buffer : To retrieve the input.
 * Relevant_nodes :
 * sum_expansion :
 */
void sumatory_expand(string buffer, vector<string> relevant_nodes, vector<string> &sum_expansion) {

  string builder1, builder2;

  if(relevant_nodes.size() == 1) {
		builder1 = buffer + ", +" + relevant_nodes[0];
		sum_expansion.push_back(builder1);
		builder2 = buffer + ", -" + relevant_nodes[0];
		sum_expansion.push_back(builder2);
	} else {
		builder1 = buffer  + ", +" + relevant_nodes.back();
		builder2 = buffer  + ", -" + relevant_nodes.back();
		relevant_nodes.pop_back();
		sumatory_expand(builder1, relevant_nodes, sum_expansion);
		sumatory_expand(builder2, relevant_nodes, sum_expansion);
	}
}


/**
 * Function to solve joint in the queries
 *
 * Attributes
 * ----------
 * Nodes : The map of nodes to add their Probabilities.
 * Buffer : To retrieve the input.
 */
double solve_joint(map<string, struct node*> nodes, string buffer) {

  string builder,aux,picker,hidden = "";
	double acum = 0.0;

  for(int i = 0; i < buffer.length(); i++) {
		if(buffer[i] == ',') {
			aux = hidden_nodes(nodes, builder);
			builder = "";
		}
		if(buffer[i] == '+' || buffer[i] == '-' || buffer[i] == ' ' || buffer[i] == ',');
		else{
      // To construct the string key for the map of nodes
			builder += buffer[i];
		}

		stringstream extract(aux);

		while(extract >> picker){
			if(hidden.find(picker) != string::npos);
			else{
				hidden += picker + " ";
			}
		}
	}
	aux = hidden_nodes(nodes, builder);
	builder = "";

	stringstream extract1(aux);

	while(extract1 >> picker){
		if(hidden.find(picker) != string::npos);
		else{
			hidden += picker + " ";
		}
	}

	stringstream extract2(hidden);

	std::vector<string> relevant_nodes;

	while(extract2 >> picker){
		if(buffer.find(picker) != string::npos);
		else{
			builder += picker + " ";
			relevant_nodes.push_back(picker);
		}
	}

	vector<string> sum_expansion;

	if(!relevant_nodes.empty()) {
		sumatory_expand(buffer, relevant_nodes, sum_expansion);
		for(int i = 0; i < sum_expansion.size(); i++) {
			acum += chain_rule(nodes, sum_expansion[i]);
		}
	} else {
		acum = chain_rule(nodes, buffer);
	}
	return acum;
}

/**
 * Function to solve the queries in the input
 * Attributes
 * ----------
 * Nodes : The map of nodes to add their Probabilities.
 * Buffer : To retrieve the input.
 */
void solve_query(map<string, struct node*> nodes, string buffer){

  double res;
	string numerator, denominator;
	numerator = "";
	denominator = "";
	bool dependece = false;

  for(int i = 0; i < buffer.length(); i++){
		if(buffer[i] == '|') {
      // To know if has dependece
			dependece = true;
			numerator += ", ";
		}
		if(!dependece) {
			if(buffer[i] != '|') {
				numerator += buffer[i];
			}
		} else {
			if(buffer[i] != '|') {
				numerator += buffer[i];
				denominator += buffer[i];
			}
		}
	}

	if(denominator.compare("") == 0){
		res = solve_joint(nodes, numerator);
	}else{
		res = solve_joint(nodes,numerator) / solve_joint(nodes,denominator);
	}

	printf("%.5G\n", res);
}


int main(int argc, char *argv[]) {

	std::map<string, struct node*> nodes;
	string buffer, builder, aux;
	char devourer;
	double readerProb;
	int sect = 0;

	while(sect < 3){
		getline(cin, buffer);
		if(buffer.compare("[Nodes]") == 0){
			create_nodes(nodes, buffer);
			sect++;
		} else if(buffer.compare("[Probabilities]") == 0){
			create_bayes_network(nodes, buffer);
			sect++;
		}else if (buffer.compare("[Queries]") == 0){
			while(getline(cin, buffer)){
				solve_query(nodes, buffer);
			}
			sect++;
		}
	}
	return 0;
}
