#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parser_ll1.h"
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <set>
#include <string>
#include <cstring>
#define LOCAL

struct P_Token{	//һ������ĸ����Ӧ����vn/vt
	union{ VN vn; TokenType t; } attr;
	bool isVn; //�Ƿ��Ƿ��ս��
};
struct TableItem{
	TokenType t;
	string p;	//t��Ӧ��production
	vector<P_Token> p_tk;
};
struct TableKey{
	VN vn;
	TokenType t;
};
struct  TableVal{
	string p_str;
	vector<P_Token> p_tk;
};

static multimap<string,vector<string> > production;
static map<VN,set<TableItem> > first;
static map<VN,set<TableItem> > follow;

static map<TableKey,TableVal> pTable; //ParsingTable; value(string):Ҫ����� 
static stack<P_Token> s;

TokenType token;

static void init();
static bool getVt(string x, TokenType& t);
static bool getVn(string x, VN& v);
static void getFirst();
static void getFollow();
static void initParsingTable();
static string getProEp(string vn);
static string getProStr(string Vn,vector<string> v);
static void TableItemErase(set<TableItem> tbItem,TokenType t);
static bool TableItemHasNull(set<TableItem> tbItem);
static string convertToEpPro(char* str);
static TreeNode * parse(); 
static string convertToEpPro(char* str);
static void getProEpToken(vector<P_Token> & p);
static string getProEp(string vn);
static string vt(TokenType t);

static void syntaxError(char * message)
{ fprintf(listing,"\n>>> ");
  fprintf(listing,"Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}

TreeNode * parse(){
	P_Token start,program;
	start.attr.t=PARSER_START;
	start.isVn=false;
	program.attr.vn=Program;
	program.isVn=true;
	s.push(start);
	s.push(program);
	bool isGetTk=true;	//�ж��Ƿ���ҪgetToken 
	while(s.size()>1){
		fprintf(listing,"----------------------\n");
		P_Token s_top=s.top(); //top of stack
		if(isGetTk){
			token = getToken();
		}
		if(s_top.isVn==false){	//match
			
			s.pop();
			if( s_top.attr.t==token){
				isGetTk = true;  
				cout<<"match:	"<<vt(s_top.attr.t)<<endl;
			}
			else{
				isGetTk = false;
				cout<<"ep	"<<endl;
			}			
		}
		else{
			isGetTk = false;
			s.pop();
			TableVal tbVal=pTable[{s_top.attr.vn,token}];
			vector<P_Token> pToken=tbVal.p_tk;
			if(pToken.size() == 0){
				cout<<"error"<<endl; 
			} 
			else{
				fprintf(listing,"%s\n",tbVal.p_str.c_str());
			}
			
			cout<<"push "<<pToken.size()<<"%%%";
			for(int i=0;i<pToken.size();i++){
				if(pToken[i].isVn==false && pToken[i].attr.t==NULL_TK)	break;
				s.push(pToken[i]);
			}
		}
		cout<<"stack size = "<<s.size()<<endl;
	}
	return NULL;
}

bool operator< ( TableKey a, TableKey b ){
	return std::make_pair(a.vn,a.t) < std::make_pair(b.vn,b.t) ;
}

string convertToEpPro(char* str){
	const char* split=" ";
	char *p;
	string ep="ep";
	p=strtok(str,split); //vn
	string vn=p;
	p=strtok(str,split); //->
	p=strtok(str,split); //����ʽ�ұߵ�һ��string
	string t=p;
	if(t!=ep){
		return vn+" -> ep";
	}
}

void initParsingTable(){
	map<VN,set<TableItem> >::iterator firstIt,followIt;
	set<TableItem>::iterator tbItemSetIt;
	for(firstIt=first.begin();firstIt!=first.end();firstIt++){ //����ÿ�����ս��
		set<TableItem> tmpTbSet=(*firstIt).second;
		for(tbItemSetIt=tmpTbSet.begin();tbItemSetIt!=tmpTbSet.end();tbItemSetIt++){	//����ÿ�����ս����first����
			pTable[{(*firstIt).first,(*tbItemSetIt).t}]={(*tbItemSetIt).p,(*tbItemSetIt).p_tk};
//			fprintf(listing,"p_tk = %d\n",(*tbItemSetIt).p_tk.size());
			// cout<<"{"<<(*firstIt).first<<","<<(*tbItemSetIt).t<<" = "<<(*tbItemSetIt).p<<endl;
		}
		if(TableItemHasNull(tmpTbSet)){	//��first��ep������follow
			set<TableItem> followTbSet;
			set<TableItem>::iterator followTbSetIt;
			followTbSet=follow.find((*firstIt).first)->second;	//��vn��follow 
			for(followTbSetIt=followTbSet.begin();followTbSetIt!=followTbSet.end();followTbSetIt++){
//				string pro=(*followTbSetIt).p;
//				
//				char *cstr = new char[pro.length() + 1];	//convert string to char*
//				strcpy(cstr, pro.c_str());
//				string pro1=convertToEpPro(cstr);	//pro1:	vn -> ep
//				delete [] cstr;
//				string pro=getProEp((*firstIt).first);
				
//				vector<P_Token> p;
//				getProEpToken(p);
				pTable[{(*firstIt).first,(*followTbSetIt).t}]={(*followTbSetIt).p,(*followTbSetIt).p_tk};
//				pTable[{(*firstIt).first,(*followTbSetIt).t}]={pro1,(*followTbSetIt).p_tk};
//				fprintf(listing,"p_tk = %d\n",(*tbItemSetIt).p_tk.size());
			}
		}
	}
}

TreeNode * parse_ll1(){
	#ifdef LOCAL
	freopen("test/grammar.txt","r",stdin);
//	freopen("test/output.txt","w",stdout);
	#endif
	init();
	getFirst();
	getFollow();  //���first,follow
	initParsingTable();	//����parsing table
	
	TreeNode * t;
//	t=parse();
	parse();
	if (token!=ENDFILE)
    	syntaxError("Code ends before file\n");
 	 return t;
}


bool getVt(string x, TokenType& t){
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
	else if(x=="ID"){t=ID; return true;} 
	else if(x=="int"){t=INT; return true;} 
	else if(x=="void"){t=VOID; return true;} 
	else if(x=="EOF"){t=ENDFILE; return true;} 
	else if(x=="["){t=LSQ; return true;} 
	else if(x=="]"){t=RSQ; return true;}
	else if(x=="("){t=LPAREN; return true;} 
	else if(x==")"){t=RPAREN; return true;} 
	else if(x=="{"){t=LCURL; return true;} 
	else if(x=="}"){t=RCURL; return true;}  
	else if(x==">="){t=GE; return true;} 
	else if(x==">"){t=GT; return true;}
	else if(x=="<="){t=LE; return true;} 
	else if(x=="<"){t=LT; return true;} 
	else if(x=="=="){t=EQ; return true;} 
	else if(x=="!="){t=NOTEQ; return true;} 
	else if(x=="="){t=ASSIGN; return true;} 
	else return false;
}

bool getVn(string x, VN& v){
	if(x=="program"){v=Program; return true;} 
	else if(x=="declarationList"){v=DeList; return true;} 
	else if(x=="declarationList1"){v=DeList1; return true;} 
	else if(x=="declaration"){v=De; return true;} 
	else if(x=="declaration1"){v=De1; return true;} 
	else if(x=="varDeclaration"){v=VarDe; return true;} 
	else if(x=="funDeclaration"){v=FunDe; return true;} 
	else if(x=="typeSpecifier"){v=Type; return true;} 
	else if(x=="type1"){v=Type1; return true;} 
	else if(x=="params"){v=Params; return true;} 
	else if(x=="param"){v=Param; return true;} 
	else if(x=="param1"){v=Param1; return true;} 
	else if(x=="paramList"){v=ParamList; return true;} 
	else if(x=="paramList1"){v=ParamList1; return true;} 
	else if(x=="compoundStmt"){v=CmpdStmt; return true;} 
	else if(x=="localDeclarations"){v=LocalDes; return true;} 
	else if(x=="localDeclarations1"){v=LocalDes1; return true;} 
	
	else if(x=="statement"){v=Stmt; return true;} 
	else if(x=="statement2"){v=Stmt2; return true;} 
	else if(x=="statementList"){v=StmtList; return true;} 
	else if(x=="statementList1"){v=StmtList1; return true;} 
	else if(x=="expression"){v=Exp; return true;} 
	else if(x=="expression1"){v=Exp1; return true;} 
	else if(x=="expressionStmt"){v=ExpStmt; return true;} 
	else if(x=="selectionStmt"){v=SelecStmt; return true;} 
	else if(x=="iterationStmt"){v=IterStmt; return true;} 
	else if(x=="returnStmt"){v=ReturnStmt; return true;} 
	else if(x=="selectionStmt1"){v=SelecStmt1; return true;} 
	else if(x=="paramList1"){v=ParamList1; return true;} 
	else if(x=="returnStmt1"){v=ReturnStmt1; return true;} 
	
	else if(x=="simpleExpression"){v=SimpleExp; return true;} 
	else if(x=="simpleExpression1"){v=SimpleExp1; return true;} 
	else if(x=="additiveExpression"){v=AddExp; return true;} 
	else if(x=="additiveExpression1"){v=AddExp1; return true;} 
	else if(x=="relop"){v=Relop; return true;} 
	else if(x=="addop"){v=Addop; return true;} 
	else if(x=="mulop"){v=Mulop; return true;} 
	else if(x=="term"){v=Term; return true;} 
	else if(x=="term1"){v=Term1; return true;} 
	else if(x=="factor"){v=Factor; return true;} 
	else if(x=="factor1"){v=Factor1; return true;} 
	else if(x=="var"){v=Var; return true;} 
	else if(x=="call"){v=Call; return true;} 
	else if(x=="args"){v=Args; return true;} 
	else if(x=="argList"){v=ArgList; return true;} 
	else if(x=="argList1"){v=ArgList1; return true;} 
	else return false;
}

string vt(TokenType t){
	switch (t){
		case IF: return "if";
		case ELSE: return "else";
		case INT: return "int";
		case RETURN: return "return";
		case WHILE: return "while";
		case VOID: return "void";
		case ID: return "id";
		case ASSIGN: return "assign";
		case EQ: return "==";
		case PLUS: return "+";
		case LPAREN: return "(";
		case RPAREN: return ")";
		case LSQ: return "[";
		case RSQ: return "]";
		case SEMI: return ";";
		case LCURL: return "{";
		case RCURL: return "}";
		case COMMA: return ",";
		case NUM: return "num";
		case NULL_TK: return "ep";
		default: return "x";
	}
}

string getProEp(string vn){
	return vn+" -> ep";
}

void init(){	//input & initialize
	string tvn,texp;
	VN vn;
	vector<string> exp;
	cin>>tvn;
	bool startSym=true;
	while(tvn!="0"){  //input file end with 0
		getVn(tvn,vn); //convert string to VN
		set<TableItem> tbItem;	//insert an empty set to first&follow (value)
		// set<TokenType> ch; //insert an empty set to follow (value)
		first.insert(pair<VN,set<TableItem> >(vn,tbItem));
		if(startSym){	//���뿪ʼ����
			TableItem x;
			x.t=PARSER_START;
			x.p=getProEp(tvn);
			P_Token pt;
			pt.attr.t=PARSER_START;
			pt.isVn=false;
			(x.p_tk).push_back(pt);
			tbItem.insert(x);
		}
		startSym=false;
		follow.insert(pair<VN,set<TableItem> >(vn,tbItem));

		cin>>texp; //����'-'
		cin>>texp; //production's right side PARSER_STARTs
		vector<string> tstring;
		while(texp!="0"){
			tstring.push_back(texp);
//			cout<<texp<<" ";
			cin>>texp;
		}
//		cout<<endl;
		production.insert(pair<string,vector<string> >(tvn,tstring));
		
		cin>>tvn;
	}
}

string getProStr(string Vn,vector<string> v){ //�����Ĳ���ʽin string
	string str=Vn+" -> ";
	for(int i=0;i<v.size();i++){
		str=str+v[i]+" ";
	}
	return str;
}

void TableItemErase(set<TableItem> tbItem,TokenType t){
	set<TableItem>::iterator iter = tbItem.begin(), tIter;
	while (iter!=tbItem.end()){
		TableItem ttbItem=*iter;
		tIter=iter;
		iter++;
		if(ttbItem.t == t){
			tbItem.erase(tIter);
		}
		
	}
}

bool TableItemHasNull(set<TableItem> tbItem){
	set<TableItem>::iterator iter = tbItem.begin();
	while (iter!=tbItem.end()){
		TableItem ttbItem=*iter;
		if(ttbItem.t == NULL_TK){
			return true;
		}
		iter++;
	}
	return false;
}

inline bool operator<(const TableItem& lhs, const TableItem& rhs)
{
  return lhs.t < rhs.t;
}

void getProToken(vector<P_Token> & p,vector<string> vStr){ //���򽫲���ʽ�ұ߱��棬�Ա�parse��ѹջ����
//	cout<<"getProToken-----------"<<endl;
p.clear();
	for(int i=vStr.size()-1 ; i>=0 ; i--){
		TokenType t;
		VN v;
		P_Token pt;
		if(getVn(vStr[i],v)){
			pt.attr.vn=v;
			pt.isVn=true;
		}
		else if(getVt(vStr[i],t)){
			pt.attr.t=t;
			pt.isVn=false;
		}
		p.push_back(pt);
//		cout<<vStr[i]<<" ";
	}
//	cout<<endl<<"getProToken-----------"<<endl;
}

void getProEpToken(vector<P_Token> & p){
	P_Token pt;
	pt.attr.t=NULL_TK;
	pt.isVn=false;
//	cout<<"getEptoken--------"<<endl; 
p.clear();
	p.push_back(pt);
}

void getFirst(){
	bool isChanged = true;
	multimap<string,vector<string> >::iterator it;
	while(isChanged){
		isChanged=false; //��һ��������û�б仯
		for(it=production.begin(); it!=production.end(); it++){ //for every production
			bool insertE=true;
			vector<string> tmpStrSet=it->second; //right side of a production
			
			VN itFirst;
			getVn(it->first,itFirst); //����ʽ��ߵ�VN����string��ʽ->VN��ʽ
//			cout<<"left vn:"<<it->first<<endl;
			set<TableItem> tmpSet=(first.find(itFirst)->second); //current first set
			int beforeLen=tmpSet.size(),afterLen;
			for(int i=0;i<tmpStrSet.size();i++){
				TokenType t;
				VN v; //v:����ʽ�ұ�
				TableItem tbItem;
				if(getVt(tmpStrSet[i],t)){ //terminal
					tbItem.p=getProStr(it->first,tmpStrSet); //����ʽ�����ʽ 
					tbItem.t=t;
					getProToken(tbItem.p_tk,tmpStrSet);
					tmpSet.insert(tbItem);
					insertE = false; 
					break;
				}
				else if(getVn(tmpStrSet[i],v)){  //non-terminal
					set<TableItem> vnSet=(first.find(v)->second);	//��vn��first 
					TableItemErase(vnSet,NULL_TK);	//ȥ��ep 
					
					set<TableItem>::iterator v_it;
					for(v_it=vnSet.begin();v_it!=vnSet.end();v_it++){
						tbItem.p=getProStr(it->first,tmpStrSet);
						tbItem.t=(*v_it).t;
						getProToken(tbItem.p_tk,tmpStrSet);
//						fprintf(listing,"insert: %d>>>>>>>>>>>>\n",tbItem.p_tk.size());
						tmpSet.insert(tbItem);
					}
					
//					tmpSet.insert(vnSet.begin(),vnSet.end());
					
					if(TableItemHasNull(first.find(v)->second)==false){	//v��first����ep
						insertE = false;
						break;
					}
				}
			}
			if(insertE==true){//first�е�ep:����ʱֻ�����ж��Ƿ���Ҫ����follow����
				TableItem ttbItem;
				ttbItem.p="";	
				ttbItem.t=NULL_TK;
				tmpSet.insert(ttbItem);
			}
			afterLen=tmpSet.size();
			if(afterLen>beforeLen){
				isChanged=true;
				(first.find(itFirst)->second)=tmpSet;
			}
		}
	}
	//output first
//	map<VN,set<TableItem> >::iterator firstIt;
//	set<TableItem>::iterator firstSetIt;
//	for(firstIt=first.begin();firstIt!=first.end();firstIt++){
//		cout<<firstIt->first<<"  First:     ";
//		for(firstSetIt=firstIt->second.begin();firstSetIt!=firstIt->second.end();firstSetIt++){
//			cout<<"��vt��"<<vt((*firstSetIt).t)<<endl;
//			for(int i=0;i<(*firstSetIt).p_tk.size();i++){
//				if((*firstSetIt).p_tk[i].isVn){
//					cout<<"(vn)"<<(*firstSetIt).p_tk[i].attr.vn<<"   ";
//				} 
//				else{
//					cout<<"(vt)"<<(*firstSetIt).p_tk[i].attr.t<<"   ";
//				}
//			}
//			cout<<(*firstSetIt).p<<endl;
//		}
//		cout<<endl;
//	}
//	cout<<endl;
}

set<TableItem> copyTableItem(set<TableItem> f,string vn){ //Ҫinsert first into followʱ���Ѳ���ʽtoken��ʽ�����ep token 
	set<TableItem>::iterator it;
	set<TableItem> t;
	for(it=f.begin();it!=f.end();it++){
		TableItem tb;
		tb.t=(*it).t;
		if(tb.t==ELSE && vn=="selectionStmt1"){
			continue;
		}
		tb.p=vn+" -> ep";
		tb.p_tk.clear();
		P_Token ptoken;
		ptoken.attr.t=NULL_TK;
		ptoken.isVn=false;
		tb.p_tk.push_back(ptoken);
		
		t.insert(tb);
	}
	return t;
}

 void getFollow(){
 	//follow
 	bool isChanged = true;
 	multimap<string,vector<string> >::iterator it;
	
 	while(isChanged){
 		isChanged=false;
 		for(it=production.begin();it!=production.end();it++){
 			vector<string> tmpStrSet=it->second;
 			VN itFirst;
 			getVn(it->first,itFirst);	//����ʽ��ߵ�VN����string��ʽ->VN��ʽ
 			// cout<<"tmpStrSet = "<<tmpStrSet<<endl;
 			for(int i=0;i<tmpStrSet.size();i++){ //�ֱ������ʽ�ұߵĴ����ҵķ��ս��
 				bool hasE=false;
 				TokenType t;
 				VN v;
 				if(getVn(tmpStrSet[i],v)){ //non-terminal, v:current VN�� ���´���vn��v
 					set<TableItem> tmpSet=(follow.find(v)->second);
 					int beforeLen=tmpSet.size(),afterLen;
 					if(i!=tmpStrSet.size()-1){ 	//current not the last non-terminal
 						if(getVt(tmpStrSet[i+1],t)){ 	//if the next is a terminal
						 	TableItem x;
							x.t=t;
							x.p=getProEp(tmpStrSet[i]);		//��ǰ�������tmpStrSet[i]��follow����
 							getProEpToken(x.p_tk);
							tmpSet.insert(x);
 						}
 						else{  //the next is non-terminal
 							hasE=true;
 							for(int j=i+1;j<tmpStrSet.size();j++){
 								if(getVt(tmpStrSet[j],t)){ //the next is terminal
 									// set<TableItem> tmpSet1=follow.find(itFirst)->second; //????
									TableItem x;
									x.t=t;
									x.p=getProEp(tmpStrSet[i]);
									getProEpToken(x.p_tk);
 									tmpSet.insert(x);
 									hasE=false;
 									break;
 								}
 								//the next is non-terminal
 								getVn(tmpStrSet[j],v);
 								set<TableItem> tmpSet1=first.find(v)->second; //�����vn��first����(�˴���δ�޸Ķ�Ӧproduction!!!)
						
 								if(TableItemHasNull(tmpSet1)==false){  //j��first����ep
 									hasE=false;
 								}
 								TableItemErase(tmpSet1,NULL_TK);
 								
 								set<TableItem> x;
 								x=copyTableItem(tmpSet1,tmpStrSet[i]);
 								tmpSet.insert(x.begin(),x.end());
								if(hasE==false){
									break;
								}								
 							}							
 						}
 					}
					if(i==tmpStrSet.size()-1 || hasE){ //if the last non-terminal || ��vn�ұ�ȫΪvn����first����ep
						set<TableItem> tmpSet1=follow.find(itFirst)->second; //�������ʽ��ߵ�VN��follow
						set<TableItem> x;
 						x=copyTableItem(tmpSet1,tmpStrSet[i]);
 						tmpSet.insert(x.begin(),x.end());
//						tmpSet.insert(tmpSet1.begin(),tmpSet1.end());
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
//	map<VN,set<TableItem> >::iterator followIt;
//	set<TableItem>::iterator followSetIt;
//	for(followIt=follow.begin();followIt!=follow.end();followIt++){
//		cout<<followIt->first<<"  Follow: ";
//		for(followSetIt=followIt->second.begin();followSetIt!=followIt->second.end();followSetIt++){
//			cout<<"��vt��"<<vt((*followSetIt).t)<<endl;
//			for(int i=0;i<(*followSetIt).p_tk.size();i++){
//				if((*followSetIt).p_tk[i].isVn){
//					cout<<"(vn)"<<(*followSetIt).p_tk[i].attr.vn<<"   ";
//				} 
//				else{
//					cout<<"(vt)"<<(*followSetIt).p_tk[i].attr.t<<"   ";
//				}
//			}
//			cout<<(*followSetIt).p<<endl;
//		}
//		cout<<endl;
//	}
 }
