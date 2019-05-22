#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parser_ll1.h"
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#define LOCAL

static multimap<VN,vector<string> > production;
static map<VN,set<TokenType> > first,follow;

struct tableKey{
	VN vn;
	TokenType t;
};
static map<tableKey,string> pTable; //ParsingTable; value(string):要输出的 

static void init();
static bool getVt(string x, TokenType& t);
static bool getVn(string x, VN& v);
static void getFirst();
static void getFollow();
static void initParsingTable();

TreeNode * parse_ll1(){
	#ifdef LOCAL
	freopen("test/grammar.txt","r",stdin);
	freopen("test/output.txt","w",stdout);
	#endif
	init();
	getFirst();
	getFollow();  //求出了first,follow
	initParsingTable();
}

void initParsingTable(){
	
}

//以下是求first,follow 
static bool getVt(string x, TokenType& t){
	if(x=="if"){t=IF; return true;} 
	else if(x=="else"){t=ELSE; return true;} 
	else if(x=="return"){t=RETURN; return true;} 
	else if(x=="while"){t=WHILE; return true;} 
	else if(x=="+"){t=PLUS; return true;} 
	else if(x=="-"){t=MINUS; return true;} 
	else if(x=="*"){t=TIMES; return true;} 
	else if(x=="/"){t=OVER; return true;} 
	else if(x=="ep"){t=NULL_TK; return true;} 
	else if(x==";"){t=SEMI; return true;} 
	else if(x==","){t=COMMA; return true;} 
	else if(x=="NUM"){t=NUM; return true;} 
	else if(x=="int"){t=INT; return true;} 
	else if(x=="void"){t=VOID; return true;} 
	else if(x=="["){t=LSQ; return true;} 
	else if(x=="]"){t=RSQ; return true;}
	else if(x=="("){t=LPAREN; return true;} 
	else if(x==")"){t=RPAREN; return true;} 
	else if(x=="{"){t=LCURL; return true;} 
	else if(x=="}"){t=RCURL; return true;}  
	else return false;
}

static bool getVn(string x, VN& v){
	if(x=="program"){v=Program; return true;} 
	else if(x=="declarationList"){v=DeList; return true;} 
	else if(x=="declarationList1"){v=DeList1; return true;} 
	else if(x=="declaration"){v=De; return true;} 
	else if(x=="varDeclaration"){v=VarDe; return true;} 
	else if(x=="funDeclaration"){v=FunDe; return true;} 
	else if(x=="typeSpecifier"){v=Type; return true;} 
	else if(x=="type1"){v=Type1; return true;} 
	else if(x=="params"){v=Params; return true;} 
	else if(x=="param"){v=Param; return true;} 
	else if(x=="paramList"){v=ParamList; return true;} 
	else if(x=="paramList1"){v=ParamList1; return true;} 
	else if(x=="compoundStmt"){v=CmpdStmt; return true;} 
	// else if(x==""){v=Program; return true;} 
	else return false;
}

static void init(){	//input & initialize
	string tvn,texp;
	VN vn;
	vector<string> exp;
	cin>>tvn;
	bool startSym=true;
	while(tvn!="0"){  //input file end with 0
		getVn(tvn,vn); //convert string to VN
		set<TokenType> ch; //insert an empty set to first&follow (value)
		first.insert(pair<VN,set<TokenType> >(vn,ch));
		if(startSym){
			ch.insert(PARSER_START);
		}
		startSym=false;
		follow.insert(pair<VN,set<TokenType> >(vn,ch));

		cin>>texp; //符号'-'
		cin>>texp; //production's right side starts
		vector<string> tstring;
		while(texp!="0"){
			tstring.push_back(texp);
			cout<<texp<<" ";
			cin>>texp;
		}
		cout<<endl;
		production.insert(pair<VN,vector<string> >(vn,tstring));
		
		cin>>tvn;
	}
}

static void getFirst(){
	bool isChanged = true;
	multimap<VN,vector<string> >::iterator it;
	while(isChanged){
		isChanged=false; //看一轮下来有没有变化
		for(it=production.begin(); it!=production.end(); it++){ //for every production
			bool insertE=true;
			vector<string> tmpStrSet=it->second; //right side of a production
			
			set<TokenType> tmpSet=(first.find(it->first)->second); //current first set
			int beforeLen=tmpSet.size(),afterLen;
			for(int i=0;i<tmpStrSet.size();i++){
				TokenType t;
				VN v; //v:产生式右边
				if(getVt(tmpStrSet[i],t)){ //terminal
					tmpSet.insert(t);
					insertE = false; 
					break;
				}
				else if(getVn(tmpStrSet[i],v)){  //non-terminal
					set<TokenType> vnSet=(first.find(v)->second);
					vnSet.erase(NULL_TK);
					tmpSet.insert(vnSet.begin(),vnSet.end());
					if(!(first.find(v)->second).count(NULL_TK)){ //v的first不含ep
						insertE = false;
						break;
					}
				}
			}
			if(insertE==true){
				tmpSet.insert(NULL_TK);
			}
			afterLen=tmpSet.size();
			if(afterLen>beforeLen){
				isChanged=true;
				(first.find(it->first)->second)=tmpSet;
			}
		}
	}
	//output first
	map<VN,set<TokenType> >::iterator firstIt;
	set<TokenType>::iterator firstSetIt;
	for(firstIt=first.begin();firstIt!=first.end();firstIt++){
		cout<<firstIt->first<<"  First:";
		for(firstSetIt=firstIt->second.begin();firstSetIt!=firstIt->second.end();firstSetIt++){
			cout<<*firstSetIt<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
}

static void getFollow(){
	//follow
	bool isChanged = true;
	multimap<VN,vector<string> >::iterator it;
	
	while(isChanged){
		isChanged=false;
		for(it=production.begin();it!=production.end();it++){
			vector<string> tmpStrSet=it->second;
			// cout<<"tmpStrSet = "<<tmpStrSet<<endl;
			for(int i=0;i<tmpStrSet.size();i++){ //产生式右边的从左到右的非终结符
				bool hasE=false;
				TokenType t;
				VN v;
				if(getVn(tmpStrSet[i],v)){ //non-terminal, v:current VN， 以下对于当前vn：
					// cout<<"now :"<<tmpStrSet[i]<<endl;
					set<TokenType> tmpSet=(follow.find(v)->second);
					int beforeLen=tmpSet.size(),afterLen;
					if(i!=tmpStrSet.size()-1){ 	//current not the last non-terminal
						if(getVt(tmpStrSet[i+1],t)){ 	//the next is terminal
							tmpSet.insert(t);
							// cout<<"insert: "<<tmpStrSet[i+1]<<endl;
							// break;
						}
						else{  //the next is non-terminal
							hasE=true;
							for(int j=i+1;j<tmpStrSet.size();j++){
								if(getVt(tmpStrSet[j],t)){ //the next is terminal
									set<TokenType> tmpSet1=follow.find(it->first)->second;
									tmpSet.insert(t);
									hasE=false;
									break;
								}
								//the next is non-terminal
								getVn(tmpStrSet[j],v);
								set<TokenType> tmpSet1=first.find(v)->second;
								if(!tmpSet1.count(NULL_TK)){  //j的first不含ep
									hasE=false;
								}
								tmpSet1.erase(NULL_TK);
								tmpSet.insert(tmpSet1.begin(),tmpSet1.end());
								if(hasE==false){
									break;
								}								
							}							
						}
					}
					if(i==tmpStrSet.size()-1 || hasE){ //if the last non-terminal || 该vn右边全为vn并且first都含ep
						set<TokenType> tmpSet1=follow.find(it->first)->second; //加入产生式左边的VN的follow
						tmpSet.insert(tmpSet1.begin(),tmpSet1.end());
					}
					afterLen=tmpSet.size();
					getVn(tmpStrSet[i],v);
					if(afterLen>beforeLen){
						isChanged=true;
						(follow.find(v)->second)=tmpSet;
					}
				}
			}
		}
	}
	//output follow
	map<VN,set<TokenType> >::iterator followIt;
	set<TokenType>::iterator followSetIt;
	for(followIt=follow.begin();followIt!=follow.end();followIt++){
		cout<<followIt->first<<"  Follow: ";
		for(followSetIt=followIt->second.begin();followSetIt!=followIt->second.end();followSetIt++){
			cout<<*followSetIt<<" ";
		}
		cout<<endl;
	}
}

