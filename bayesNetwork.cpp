#include <iostream>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>

using namespace std;

/**
 * Struct Node to add nodes at the Bayesian Network
 * Attributes
 * ----------
 * Parents: The parents of the node. For 0 or N-parents;
 *          if the vector is empty, the event is Independient.
 * Probability_table: Probability table for actual node. Key is X in P(X).
 * Name: Name of the node.
 */
struct node{
	std::vector<struct node*> parents; 
	std::map<string, double> probabilityTable;  
	string name;
};


/**
 * Function to find hidden nodes in the joint
 * Attributes
 * ----------
 * Nodes : The map of nodes to add their Probabilities.
 * key_node : The key node.
 */
string hiddenNodes(map<string, struct node*> nodes, string nodeKey){
	string builder="";
	if(nodes[nodeKey]->parents.empty()){
		builder+=nodeKey;
	}else{
		for(int i= 0;i < nodes[nodeKey]->parents.size();i++){
			builder+= hiddenNodes(nodes,nodes[nodeKey]->parents[i]->name) + " ";
		}
		builder+=nodeKey;
	}
	return builder;
}


/**
 * Function for the sumatory
 * Attributes
 * ----------
 * Buffer : To retrieve the input.
 * Relevant_nodes :
 * sum_expansion :
 */
void expandSum(string buffer, vector<string> nodes, vector<string> &sumExp){
	string builder1,builder2;
	if(nodes.size()==1){
		builder1 = buffer + ", +" + nodes[0];
		sumExp.push_back(builder1);
		builder2 = buffer + ", -" + nodes[0];
		sumExp.push_back(builder2);
	}else{
		builder1 = buffer  + ", +" + nodes.back();
		builder2 = buffer  + ", -" + nodes.back();
		nodes.pop_back();
		expandSum(builder1,nodes,sumExp);
		expandSum(builder2,nodes,sumExp);
	}
}

/**
 * Function for the chain rule
 * Attributes
 * ----------
 * Nodes : The map of nodes to add their Probabilities.
 * Chain_probability : The string probability of chain rule.
 */
double chainRule(map<string, struct node*> nodes,string chainProb){
	string builder, name;
	char signs[]="+-";
	std::vector<string> index,statements,auxilarStmnt,probs;
	chainProb.erase (std::remove(chainProb.begin(), chainProb.end(), ','), chainProb.end());
	stringstream extract(chainProb);
	double acum=1.0;
	while(extract >> builder){
		statements.push_back(builder);
		for (int i = 0; i < strlen(signs); i++){
	      builder.erase (std::remove(builder.begin(), builder.end(), signs[i]), builder.end());
	  	}
	  	index.push_back(builder);
	}
	for (int i = 0; i < index.size(); i++){
	    //
		std::map<string, double> copyMap = nodes[index[i]]->probabilityTable;
		for (std::map<string,double>::iterator ite = copyMap.begin(); ite !=  copyMap.end(); ++ite){
			if (ite->first.find(statements[i]) != std::string::npos) {
			    probs.push_back(ite->first);
			}
		}
		if(!nodes[index[i]]->parents.empty()){
			int dependeces = nodes[index[i]]->parents.size();
			int indexProbs;
			for(int j=0;j < probs.size();j++){
				int count=0;
				for (int k = 0; k < statements.size(); k++){
					if(probs[j].find(statements[k]) != std::string::npos){
						count++;
					}
				}
				if(count == (dependeces+1)){
					indexProbs = j;
					break;
				}
			}
			acum*=copyMap[probs[indexProbs]];

		}else{
			acum*=copyMap[probs[0]];
		}
		while(!probs.empty()){
			probs.pop_back();
		}
	}
	return acum;
}

/**
 * Function to solve joint in the queries
 * Attributes
 * ----------
 * Nodes : The map of nodes to add their Probabilities.
 * Buffer : To retrieve the input.
 */
double solve_joint(map<string, struct node*> nodes, string buffer){
	string builder,aux,picker,hidden="";
	vector<string> sumExp;
	std::vector<string> relevantNodes;
	double acum=0.0;
	
	for(int i=0;i < buffer.length();i++){
		if(buffer[i]==','){
			aux = hiddenNodes(nodes,builder);
			builder="";
		}
		if(buffer[i]=='+'||buffer[i]=='-'||buffer[i]==' '||buffer[i]==',');
		else{
			builder+=buffer[i]; 
		}
		stringstream extract(aux);
		while(extract >> picker){
			if(hidden.find(picker)== string::npos){
				hidden+= picker + " ";
			}
		}
	}
	aux = hiddenNodes(nodes,builder);
	builder="";

	stringstream extract1(aux);
	while(extract1 >> picker){
		if(hidden.find(picker)==string::npos){
			hidden+= picker + " ";
		}
	}
    
    //Hidden Nodes
	stringstream extract2(hidden);
	while(extract2 >> picker){
		if(buffer.find(picker)!=string::npos);
		else{
			builder+= picker + " ";
			relevantNodes.push_back(picker);
		}
	}

	if(!relevantNodes.empty()){
		expandSum(buffer,relevantNodes,sumExp);

		for(int i=0;i<sumExp.size();i++){
			acum+=chainRule(nodes,sumExp[i]);
		}
	}else{
		acum = chainRule(nodes,buffer);
	}
	return acum;
}

int main(int argc, char *argv[]){
	std::map<string, struct node*> nodes;
	string buffer, builder, bayesBuild, aux;
	double probs;
	char devourer;
	
	int sect = 0;
	while(sect < 3){
		getline(cin,buffer);
		if(buffer.compare("[Nodes]")==0){
    		//Create nodes
        	getline(cin,buffer);
        	for(int i=0;i < buffer.length();i++){
        	    //Nodes separated by commas
        		if(buffer[i]==','){
        			nodes[builder] = new node;
        			nodes[builder]->name = builder;
        		}else if(i==(buffer.length()-1)){
        			builder+=buffer[i];
        			nodes[builder] = new node;
        			nodes[builder]->name = builder;
        		}else if(buffer[i]==' '){
        			builder="";
        		}else{
        			builder+=buffer[i];
        		}
        	}
			sect++;
		}else if(buffer.compare("[Probabilities]")==0){
			//Create the Bayes Network
        	while(buffer.compare("")!=0){
        		getline(cin,buffer);
        		bayesBuild="";
        		//Child to parents
        		bool indexNode=false;
        		for(int i=0;i < buffer.length();i++){
        			if(buffer[i]=='|'){
        				indexNode = true; 
        				aux = bayesBuild;
        				bayesBuild="";
        			}
        			if(buffer[i]=='+'||buffer[i]=='-'||buffer[i]==' '||buffer[i]=='='||buffer[i]==','||buffer[i]=='|'||(buffer[i]>=48&&buffer[i]<=57));
        			else{
        				bayesBuild+=buffer[i]; 
        			}
        			if(indexNode){ 
        					if(buffer[i]==','){
        						(nodes[aux])->parents.push_back(nodes[bayesBuild]);
        						bayesBuild="";
        					}else if (buffer[i]=='='){
        						(nodes[aux])->parents.push_back(nodes[bayesBuild]); 
        						bayesBuild="";
        					}
        			}
        			if(buffer[i]=='='){
        				if(!indexNode){
        					aux=bayesBuild;
        				}
        				break;
        			}
        		}
        		//No Duplicate Nodes
        		sort( (nodes[aux])->parents.begin(), (nodes[aux])->parents.end() );
        		(nodes[aux])->parents.erase( unique( (nodes[aux])->parents.begin(), (nodes[aux])->parents.end() ), (nodes[aux])->parents.end() );
        
        		//Builds the probability table
        		bayesBuild="";
        		for(int i=0;i < buffer.length();i++){
        			if(buffer[i]=='='){
        				break;
        			}else if(buffer[i]==' '&&buffer[i+1]=='=');
        			else{
        				bayesBuild+=buffer[i];
        			}
        		}
                
                //Assign probs
        		if(bayesBuild.compare("")!=0){
        			stringstream extract(buffer);
        			do{
        				extract >> devourer; 
        			}while(devourer!='=');
        
        			
        			extract >> probs;
        
        			(nodes[aux])->probabilityTable[bayesBuild]=probs;
        
        			bayesBuild[0]='-';
        			(nodes[aux])->probabilityTable[bayesBuild]=1 - probs;
        		}
        	}
			sect++;
			
		}else if (buffer.compare("[Queries]")==0){
			getline(cin,buffer);
			while(buffer.compare("")!=0){
			    //Solve the Query
			    double res;
            	string numerator, denominator;
            	numerator="";
            	denominator="";
            	bool dependece=false;
            	for (int i = 0; i < buffer.length(); i++){
            		if(buffer[i]=='|'){
            			dependece=true;
            			numerator+=", ";
            		}
            		if(!dependece){
            			if(buffer[i]!='|'){
            				numerator+=buffer[i];
            			}
            		}else{
            			if(buffer[i]!='|'){
            				numerator+=buffer[i];
            				denominator+=buffer[i];
            			}
            		}
            	}
                //Print the prob
            	if(denominator.compare("")==0){
            		res = solve_joint(nodes,numerator);
            	}else{
            		res = solve_joint(nodes,numerator) / solve_joint(nodes,denominator);
            	}
            	printf("%.5G\n", res);
				getline(cin,buffer);
				if(res == 0.5){
					exit while;
				}
			}
			sect++;
		}
	}
	return 0;
}
