#include <iostream>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>

using namespace std;

struct node{
	std::vector<struct node*> parents; 
	std::map<string, double> probabilityTable;  
	string name;
};

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

void sumatoryExpand(string buffer, vector<string> relevantNodes, vector<string> &sumExp){
	string builder1,builder2;
	if(relevantNodes.size()==1){
		builder1 = buffer + ", +" + relevantNodes[0];
		sumExp.push_back(builder1);
		builder2 = buffer + ", -" + relevantNodes[0];
		sumExp.push_back(builder2);
	}else{
		builder1 = buffer  + ", +" + relevantNodes.back();
		builder2 = buffer  + ", -" + relevantNodes.back();
		relevantNodes.pop_back();
		sumatoryExpand(builder1,relevantNodes,sumExp);
		sumatoryExpand(builder2,relevantNodes,sumExp);
	}
}

double solveJoint(map<string, struct node*> nodes, string buffer){
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
		sumatoryExpand(buffer,relevantNodes,sumExp);

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
	double readerProb;
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
        
        			
        			extract >> readerProb;
        
        			(nodes[aux])->probabilityTable[bayesBuild]=readerProb;
        
        			bayesBuild[0]='-';
        			(nodes[aux])->probabilityTable[bayesBuild]=1 - readerProb;
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
            		res = solveJoint(nodes,numerator);
            	}else{
            		res = solveJoint(nodes,numerator) / solveJoint(nodes,denominator);
            	}
            	printf("%.5G\n", res);
				getline(cin,buffer);
			}
			sect++;
		}
	}
	return 0;
}
