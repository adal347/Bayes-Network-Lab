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

struct node
{
	std::vector<struct node*> parents; //for 0 or N-parents; if the vector is empty, the event is Independient
	std::map<string, double> probabilityTable;  //key is X in P(X)
	string name;
};

void createNodes(map<string, struct node*> &nodes,string buffer){
	string builder;
	getline(cin,buffer);
	for(int i=0;i < buffer.length();i++){
		if(buffer[i]==','){
			nodes[builder] = new node;
			nodes[builder]->name = builder;
			//cout << builder << "\n";
		}else if(i==(buffer.length()-1)){
			builder+=buffer[i];
			//cout << builder << "\n";
			nodes[builder] = new node;
			nodes[builder]->name = builder;
		}else if(buffer[i]==' '){
			builder="";
		}else{
			builder+=buffer[i];
		}
	}
}

void createBayesNetwork(map<string, struct node*> &nodes,string buffer){
	string builder, aux;
	char devourer;
	double readerProb;

	while(buffer.compare("")!=0){
		getline(cin,buffer);
		builder="";
		//link nodes with their parents
		//cout << "----\n";
		bool indexNode=false;
		for(int i=0;i < buffer.length();i++){
			if(buffer[i]=='|'){
				indexNode = true; //the node have a parent
				aux = builder;
				builder="";
				//cout << "\nNode is: " << aux <<'\n';
				//cout << "Its parents are: ";
			}
			if(buffer[i]=='+'||buffer[i]=='-'||buffer[i]==' '||buffer[i]=='='||buffer[i]==','||buffer[i]=='|'||(buffer[i]>=48&&buffer[i]<=57));
			else{
				builder+=buffer[i]; //construct the string key for the map of nodes
				//cout << '.';
			}
			if(indexNode){ //to make the conection in base of the dependece
				/*if(find((nodes[aux])->parents.begin(),(nodes[aux])->parents.end(),nodes[builder])!=(nodes[aux])->parents.end()){
					cout << " [A.K.] ";
					builder = "";
				}
				else{*/
					if(buffer[i]==','){
						(nodes[aux])->parents.push_back(nodes[builder]); //to create N parents
						//cout << builder <<' ';
						builder="";
					}else if (buffer[i]=='=')
					{
						(nodes[aux])->parents.push_back(nodes[builder]); //if there is only one parent
						//cout << builder <<' ';
						builder="";
					}
				//}
			}

			if(buffer[i]=='='){
				if(!indexNode){
					aux=builder; //the node is root P(X) is independient
					//cout << "\nNode: " << aux <<" is a root\n";
				}
				break; //finish the creation of the edges
			}
		}
		//cout << '\n';
		//erase duplicated nodes

		sort( (nodes[aux])->parents.begin(), (nodes[aux])->parents.end() );
		(nodes[aux])->parents.erase( unique( (nodes[aux])->parents.begin(), (nodes[aux])->parents.end() ), (nodes[aux])->parents.end() );

		//probability table
		builder="";
		for(int i=0;i < buffer.length();i++){
			if(buffer[i]=='='){
				break;
			}else if(buffer[i]==' '&&buffer[i+1]=='='); //to ignore the last blank space " ="
			else{
				builder+=buffer[i];
			}
		}

		if(builder.compare("")!=0){
			stringstream extract(buffer);
			
			do{
				extract >> devourer; //eats all chars
			}while(devourer!='=');

			
			extract >> readerProb;

			(nodes[aux])->probabilityTable[builder]=readerProb; //store the true values P(+X|Y)

			//cout << "P("<< builder << ")= " << (nodes[aux])->probabilityTable[builder] << "\n";

			builder[0]='-';
			(nodes[aux])->probabilityTable[builder]=1 - readerProb; //store the true values P(-X|Y)

			//cout << "P("<< builder << ")= " << (nodes[aux])->probabilityTable[builder] << "\n";
		}

		// for(int i=0; i < (nodes[aux])->parents.size();i++){
		// 	cout << (nodes[aux])->parents[i]->name << " ";
		// }
		// cout << '\n';
	}
}

string hiddenNodes(map<string, struct node*> nodes, string nodeKey){
	string builder="";
	//cout << "-1-\n";
	if(nodes[nodeKey]->parents.empty()){
		builder+=nodeKey;
		//cout << "-2-\n";
		//cout << builder << "\n";
	}else{
		//cout << "-3-\n";
		for(int i= 0;i < nodes[nodeKey]->parents.size();i++){
			//cout << "-4-\n";
			//cout << i <<' ' <<nodes[nodeKey]->parents.size()<< '\n';
			builder+= hiddenNodes(nodes,nodes[nodeKey]->parents[i]->name) + " ";
			//cout << builder << "\n";
		}
		builder+=nodeKey;
		//cout << "-5-\n";
		//cout << builder << "\n";
	}
	return builder;
}

double chainRule(map<string, struct node*> nodes,string chainProb){
	string builder,name;
	char signs[]="+-";
	std::vector<string> index,statements,auxilarStmnt,probs;
	//cout << "\nCHAIN RULE\n";
	chainProb.erase (std::remove(chainProb.begin(), chainProb.end(), ','), chainProb.end()); //removes all ','
	//cout << "commas revomed " << chainProb << '\n';
	stringstream extract(chainProb);
	double acum=1.0;
	//cout << "extracting...\n";
	while(extract >> builder){
		statements.push_back(builder);
		for (int i = 0; i < strlen(signs); i++){
	      builder.erase (std::remove(builder.begin(), builder.end(), signs[i]), builder.end());
	  	}
	  	//cout << "signs revomed " << builder << '\n';
	  	index.push_back(builder);
	}
	//cout << "chain rule...\n";
	for (int i = 0; i < index.size(); i++)
	{
		//cout << index[i] <<"\n";
		//cout << "recover keys prob table...\n";
		std::map<string, double> copyMap = nodes[index[i]]->probabilityTable;
		for (std::map<string,double>::iterator ite = copyMap.begin(); ite !=  copyMap.end(); ++ite)
		{
			if (ite->first.find(statements[i]) != std::string::npos) {
			    probs.push_back(ite->first);
			    //cout << ite->first << ";;";
			}
		}
		//cout << "\n";
		//cout << "checking keys...\n";
		if(!nodes[index[i]]->parents.empty()){
			int dependeces = nodes[index[i]]->parents.size();
			int indexProbs;
			//cout << "dependece...\n";
			for(int j=0;j < probs.size();j++){
				int count=0;
				for (int k = 0; k < statements.size(); k++)
				{
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
			//cout << probs[indexProbs]<< " "<<copyMap[probs[indexProbs]] <<"*\n";

		}else{
			//int indexProbs;
			//cout << "independece...\n";
			// for(int j=0;j < probs.size();j++){
			// 	int count=0;
			// 	for (int k = 0; k < statements.size(); k++)
			// 	{
			// 		if(probs[j].find(statements[k]) != std::string::npos){
			// 			count++;
			// 			break;
			// 		}
			// 	}
			// 	if(count == 1){
			// 		indexProbs = j;
			// 		break;
			// 	}
			// }
			acum*=copyMap[probs[0]];
			//cout << "for the node " << index[i] << " ";
			//cout << probs[0]<< " "<<copyMap[probs[0]] <<"*\n";
		}
		while(!probs.empty()){
			probs.pop_back();
		}
	}

	//cout << acum <<"\n";

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
	//cout << buffer << '\n';
	double acum=0.0;
	for(int i=0;i < buffer.length();i++){
		if(buffer[i]==','){
			//cout << builder << '\n';
			aux = hiddenNodes(nodes,builder);
			builder="";
		}
		if(buffer[i]=='+'||buffer[i]=='-'||buffer[i]==' '||buffer[i]==',');
		else{
			builder+=buffer[i]; //construct the string key for the map of nodes
		}

		stringstream extract(aux);
			
		while(extract >> picker){
			//extract >> picker;
			if(hidden.find(picker)!=string::npos);
			else{
				hidden+= picker + " ";
			}
		}
	}
	//cout << builder << '\n';
	aux = hiddenNodes(nodes,builder);
	builder="";

	stringstream extract1(aux);
			
	while(extract1 >> picker){
		//extract >> picker;
		if(hidden.find(picker)!=string::npos);
		else{
			hidden+= picker + " ";
		}
	}

	stringstream extract2(hidden);

	std::vector<string> relevantNodes;

	while(extract2 >> picker){
		//extract >> picker;
		if(buffer.find(picker)!=string::npos);
		else{
			builder+= picker + " ";
			relevantNodes.push_back(picker);
		}
	}

	//cout << "\nRelevant hidden nodes: " << builder <<'\n';

	vector<string> sumExp;

	if(!relevantNodes.empty()){
		sumatoryExpand(buffer,relevantNodes,sumExp);

		for(int i=0;i<sumExp.size();i++){
			//cout << sumExp[i] << '\n';
			acum+=chainRule(nodes,sumExp[i]);
		}
	}else{
		acum = chainRule(nodes,buffer);
	}

	//cout << acum <<"\n";
	return acum;
	
}

void solveQuery(map<string, struct node*> nodes, string buffer){
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

	if(denominator.compare("")==0){
		//cout << numerator << "\n";
		res = solveJoint(nodes,numerator);
	}else{
		//cout << numerator << " / " << denominator << "\n";
		res = solveJoint(nodes,numerator) / solveJoint(nodes,denominator);
	}

	//cout << res <<"\n";
	printf("%.5G\n", res);
}


int main(int argc, char *argv[])
{
	std::map<string, struct node*> nodes;
	
	string buffer, builder, aux;

	char devourer;

	double readerProb;

	
	int sect = 0;
	//cout << "STARTING...\n";
	while(sect < 3){
		getline(cin,buffer);
		//cout << "Reading std\n";
		if(buffer.compare("[Nodes]")==0){
			//cout << "Nodes:\n";
			/*
			getline(cin,buffer);
			for(int i=0;i < buffer.length();i++){
				if(buffer[i]==','){
					nodes[builder] = new node;
					cout << builder << "\n";
				}else if(i==(buffer.length()-1)){
					builder+=buffer[i];
					cout << builder << "\n";
					nodes[builder] = new node;
				}else if(buffer[i]==' '){
					builder="";
				}else{
					builder+=buffer[i];
				}
			}
			*/
			createNodes(nodes,buffer);
			//cout << '\n';
			sect++;
			
		}else if(buffer.compare("[Probabilities]")==0){
			//cout << "Probabilities:\n";
			/*
			while(buffer.compare("")!=0){
				getline(cin,buffer);
				builder="";
				//link nodes with their parents
				//cout << "----\n";
				bool indexNode=false;
				for(int i=0;i < buffer.length();i++){
					if(buffer[i]=='|'){
						indexNode = true; //the node have a parent
						aux = builder;
						builder="";
						cout << "\nNode is: " << aux <<'\n';
						cout << "Its parents are: ";
					}
					if(buffer[i]=='+'||buffer[i]=='-'||buffer[i]==' '||buffer[i]=='='||buffer[i]==','||buffer[i]=='|'||(buffer[i]>=48&&buffer[i]<=57));
					else{
						builder+=buffer[i]; //construct the string key for the map of nodes
						//cout << '.';
					}
					if(indexNode){ //to make the conection in base of the dependece
						if(find((nodes[aux])->parents.begin(),(nodes[aux])->parents.end(),nodes[builder])!=(nodes[aux])->parents.end()){
							cout << " [A.K.] ";
							builder = "";
						}
						else{
							if(buffer[i]==','){
								(nodes[aux])->parents.push_back(nodes[builder]); //to create N parents
								cout << builder <<' ';
								builder="";
							}else if (buffer[i]=='=')
							{
								(nodes[aux])->parents.push_back(nodes[builder]); //if there is only one parent
								cout << builder <<' ';
								builder="";
							}
						}
					}
					if(buffer[i]=='='){
						if(!indexNode){
							aux=builder; //the node is root P(X) is independient
							cout << "\nNode: " << aux <<" is a root\n";
						}
						break; //finish the creation of the edges
					}
				}
				cout << '\n';
				//probability table
				builder="";
				for(int i=0;i < buffer.length();i++){
					if(buffer[i]=='='){
						break;
					}else if(buffer[i]==' '&&buffer[i+1]=='='); //to ignore the last blank space " ="
					else{
						builder+=buffer[i];
					}
				}
				if(builder.compare("")!=0){
					stringstream extract(buffer);
					
					do{
						extract >> devourer; //eats all chars
					}while(devourer!='=');
					
					extract >> readerProb;
					(nodes[aux])->probabilityTable[builder]=readerProb; //store the true values P(+X|Y)
					cout << "P("<< builder << ")= " << (nodes[aux])->probabilityTable[builder] << "\n";
				}
			}*/
			createBayesNetwork(nodes,buffer);
			sect++;
		}else if (buffer.compare("[Queries]")==0){
			//cout << "Queries:\n";
			getline(cin,buffer);
			while(buffer.compare("")!=0){
				solveQuery(nodes,buffer);
				getline(cin,buffer);
			}
			sect++;
		}
	}

	return 0;
}