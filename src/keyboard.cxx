#include <iostream>

#include<locale.h>
#include<libintl.h>
#include<dirent.h>
#include<sys/types.h>
#include<string.h>
#include<list>

extern "C"{
#include<augeas.h>
}

#include "ui/yuifactory.h"

#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<vector>
#include<utility>

#define _(STRING) gettext(STRING)

using namespace std;

class simpleMode;
class expertMode;

class keyboard{
	protected:
	map<string,string> layout;
	map<string,string> model;
	map<string,string> variant;
	vector< pair<string,string> >options;
	string s1,line,optionType;
	ifstream baseFile;
	
	augeas * aug;
	char * root,* loadpath;
	unsigned int flag;

	enum{MODEL,LAYOUT,VARIANT,OPTION};
	int type;
	
	UI::YUIFactory * factory;
	UI::yDialog * dialog;
	UI::yVLayout * mainLayout;
	UI::yComboBox * layoutSelect;
	UI::yHLayout * buttonLayout,*upperLayout,*addGroupLayout;
	UI::yPushButton * activateMode,*saveButton,*cancelButton,*addLayoutVariant,*deleteLayoutVariant,*addGroup,*deleteGroup;
	UI::yComboBox * modelSelect,*variantSelect,*groupCategory,*groupOptions;
	UI::yLabel * labelSelect,*showDefaultLayout,*label1;
	UI::yTable * layoutTable,*groupTable;

	UI::yVLayout * upDown1;
	UI::yPushButton *upLayout,*downLayout;
	UI::yHLayout *layoutLayout,*groupLayout;

	bool SIMPLEMODE;

	void drawSimpleMode();
	void drawExpertMode();

	void split();
	void fillUpLayoutSelect();
	void fillUpVariant();
	void fillUpModelSelect();
	void fillUpGroupCategory();
	void fillUpGroupOptions();
	bool writeConf(string &line,bool newNode,string parameter,bool isLastParameter,string extraParam,string value);
	void loadSimpleConf();
	void loadExpertConf();
	vector<string> match(string,string);
	vector<string> parseOption(const char*);
	int colNo;string xcolNo;
	public:
	keyboard();
	void drawLayout();
	bool respondToEvent();
	bool simpleWriteConf();
	bool expertWriteConf();
	void loadConf();
	~keyboard();
};

keyboard::keyboard(){
	type=0;
	baseFile.open("/usr/share/X11/xkb/rules/base.lst",ios::in);
	if(baseFile.is_open()){
			while(baseFile.good()){
				getline(baseFile,line);
				line.erase(0,2);
				if(!line.compare("model")){type = MODEL;continue;}
				if(!line.compare("layout")){type = LAYOUT;continue;}
				if(!line.compare("variant")){type = VARIANT;continue;}
				if(!line.compare("option")){type = OPTION;continue;}

				int pos = line.find_first_of(' ');
				s1 = line.substr(0,pos);line.erase(0,pos);
				pos=line.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
				line.erase(0,pos-1);				
				cout<<s1<<'\t'<<line<<endl;
				if(type==MODEL && line.length()!=0){
					model[line]=s1;
				}
				if(type==LAYOUT && line.length()!=0){
					layout[line]=s1;
				}
				if(type==VARIANT && line.length()!=0){
					variant[line]=s1;
				}
				if(type==OPTION && line.length()!=0){
					options.push_back(make_pair(line,s1));
				}

			};
	}
	SIMPLEMODE = true;
	factory = new UI::YUIFactory();
	aug=NULL;root=NULL;flag=0;loadpath=NULL;
	aug = aug_init(root,loadpath,flag);
	colNo=0;
	if(aug==NULL){
		cout<<"Cannot be opened";
	}

}

void keyboard::drawLayout(){
	if(SIMPLEMODE){
		drawSimpleMode();
	}else{
		drawExpertMode();
	}
}

void keyboard::loadConf(){
	if(SIMPLEMODE){
		loadSimpleConf();
	}else{
		loadExpertConf();
	}
}

void keyboard::drawSimpleMode(){
	factory = new UI::YUIFactory();
	dialog = factory->createDialog(60,10);
	mainLayout = factory->createVLayout(dialog);
	layoutSelect = factory->createComboBox(mainLayout,_("Select your keyboard Layout"));
	showDefaultLayout = factory->createLabel(mainLayout,"No configuration exists");
	buttonLayout = factory->createHLayout(mainLayout);
	activateMode = factory->createPushButton(buttonLayout,_("E&xpert Mode"));
	saveButton = factory->createPushButton(buttonLayout,_("&Ok"));
	cancelButton = factory->createPushButton(buttonLayout,_("&Cancel"));
	fillUpLayoutSelect();
	loadSimpleConf();
}

void keyboard::drawExpertMode(){
	dialog = factory->createDialog(120,40);
	mainLayout = factory->createVLayout(dialog);
	upperLayout = factory->createHLayout(mainLayout);
	layoutSelect = factory->createComboBox(upperLayout,_("Select your keyboard Layout"));
	fillUpLayoutSelect();
	variantSelect = factory->createComboBox(upperLayout,_("Select Layout Variant"));
	fillUpVariant();
	addLayoutVariant = factory->createPushButton(upperLayout,_("Add"));
	layoutLayout = factory->createHLayout(mainLayout);
	layoutTable = factory->createTable(layoutLayout,"Order","Layout","Variant");
	upDown1 = factory->createVLayout(layoutLayout);
	upLayout = factory->createPushButton(upDown1,_("&Up"));
	downLayout = factory->createPushButton(upDown1,_("&Down"));
	deleteLayoutVariant = factory->createPushButton(mainLayout,_("Delete selected Layout & Variant"));
	modelSelect = factory->createComboBox(mainLayout,_("Select your Model"));
	fillUpModelSelect();
	label1 = factory->createLabel(mainLayout,"Advanced Options");
	addGroupLayout = factory->createHLayout(mainLayout);
	groupCategory = factory->createComboBox(addGroupLayout,_("Category"));
	fillUpGroupCategory();
	groupOptions = factory->createComboBox(addGroupLayout,_("Relevant Options"));
	fillUpGroupOptions();
	addGroup = factory->createPushButton(addGroupLayout,_("Add"));
	groupLayout = factory->createHLayout(mainLayout);
	groupTable = factory->createTable(groupLayout,"Group","Option","");
	deleteGroup = factory->createPushButton(mainLayout,_("Delete Selected Group"));
	buttonLayout = factory->createHLayout(mainLayout);
	activateMode = factory->createPushButton(buttonLayout,_("&Simple Mode"));
	saveButton = factory->createPushButton(buttonLayout,_("&Ok"));
	cancelButton = factory->createPushButton(buttonLayout,_("&Cancel"));
	loadExpertConf();
}

keyboard::~keyboard(){
	delete cancelButton;
	delete saveButton;
	delete activateMode;
	delete buttonLayout;
	delete deleteGroup;
	delete groupTable;
	delete groupLayout;
	delete addGroup;
	delete groupOptions;
	delete groupCategory;
	delete addGroupLayout;
	delete label1;
	delete modelSelect;
	delete deleteLayoutVariant;
	delete downLayout;
	delete upLayout;
	delete upDown1;
	delete layoutTable;
	delete layoutLayout;
	delete addLayoutVariant;
	delete variantSelect;
	delete layoutSelect;
	delete upperLayout;
	delete mainLayout;
	delete dialog;
}

bool keyboard::respondToEvent(){
	while(1){
		dialog->wait();
		if(activateMode->getElement()==dialog->eventWidget()){delete dialog;
			SIMPLEMODE ? SIMPLEMODE = false : SIMPLEMODE = true;
			!SIMPLEMODE ? drawExpertMode() : drawSimpleMode(); 
			return true;
		}
		if(saveButton->getElement()==dialog->eventWidget()){
			if(SIMPLEMODE){
				if(simpleWriteConf()){
					cerr<<"Successfully written";
				}else{
					cerr<<"Successfully NOT written";
				}
			}else{
				expertWriteConf();
			}
			return false;
		}	
		if(cancelButton->getElement()==dialog->eventWidget()){
			return false;
		}
		if(!SIMPLEMODE){
			if(layoutSelect->getElement()==dialog->eventWidget()){
				fillUpVariant();
			}
			if(groupCategory->getElement()==dialog->eventWidget()){
				fillUpGroupOptions();
			}	
			if(addLayoutVariant->getElement()==dialog->eventWidget()){
				char * temp = new char [20];
				sprintf(temp,"%d",++colNo);
				xcolNo = temp;
				cout<<xcolNo;
				layoutTable->addItem(xcolNo,layoutSelect->value(),variantSelect->value());
			}
			if(addGroup->getElement()==dialog->eventWidget()){
				groupTable->addItem(groupCategory->value(),groupOptions->value());
			}
			if(deleteGroup->getElement()==dialog->eventWidget()){
				groupTable->deleteSelected();
			}
			if(deleteLayoutVariant->getElement()==dialog->eventWidget()){
				layoutTable->deleteSelected();
				--colNo;
			}
			if(upLayout->getElement()==dialog->eventWidget()){
				layoutTable->swap(-1);
			}
			if(downLayout->getElement()==dialog->eventWidget()){
				layoutTable->swap(1);
			}
		}
	}
}

void keyboard::fillUpLayoutSelect(){
	layoutSelect->deleteAllItems();
	map<string,string>::iterator it;
	for(it=layout.begin();it!=layout.end();it++){
	        layoutSelect->addItem(it->first);
	}
}
void keyboard::fillUpModelSelect(){
	modelSelect->deleteAllItems();
	map<string,string>::iterator it;
	modelSelect->addItem("Default");
	for(it=model.begin();it!=model.end();it++){
		modelSelect->addItem(it->first);
	}
}
void keyboard::fillUpVariant(){
	variantSelect->deleteAllItems();
	map<string,string>::iterator it;int temp;
	variantSelect->addItem("Default");
	for(it=variant.begin();it!=variant.end();it++){
		temp = it->first.find(layout[layoutSelect->value()]);
		if(temp==1){
			variantSelect->addItem(it->first);
		}
	}
}

void keyboard::fillUpGroupCategory(){
	groupCategory->deleteAllItems();
	vector< pair<string,string> >::iterator it;
	for(it=options.begin();it!=options.end();it++){
		if(it->second.find(':')==string::npos)
			groupCategory->addItem(it->first);
	}
}

void keyboard::fillUpGroupOptions(){
	groupOptions->deleteAllItems();
	vector< pair<string,string> >::iterator it;
	for(it=options.begin();it!=options.end();it++){
		if(!groupCategory->value().compare(it->first)){
			++it;
			while(it!=options.end() && it->second.find(':')!=string::npos){
				groupOptions->addItem(it->first);
				++it;
			}
			break;
		}
	}
}

bool keyboard::writeConf(string &line,bool newNode,string parameter,bool isLastParameter,string extraParam,string value){
	string pathParam;int error;
	pathParam.assign(line);
	if(newNode==true){
		pathParam.append("[last()+1]/");
	}else{
		pathParam.append("[last()]/");
	}
	pathParam.append(parameter);
	if(isLastParameter){
		pathParam.append("[last()+1]");
	}else{
		pathParam.append("[last()]");
	}
	pathParam.append(extraParam);
	cout<<pathParam<<endl;
	error = aug_set(aug,pathParam.c_str(),value.c_str());
	
	if(error==-1)
		return false;
	return true;
}
bool keyboard::simpleWriteConf(){
	char **match;int i=0,j=0,pos=0;string line,subPath,pathParam;
	int error;
	string layoutVal = layout[layoutSelect->value()];
	int cnt = aug_match(aug,"/files/etc/X11/xorg.conf.d/*/InputClass/MatchIsKeyboard",&match);
	for(i=0;i<cnt-1;i++){
		if(strcmp(match[i],match[i+1])<0)
			j = i+1;
	}
	if(cnt)
		line.assign(match[j]);
	else
		line.assign("/files/etc/X11/xorg.conf.d/99-saxkeyboard.conf/InputClass");

	subPath.assign("InputClass");
	pos = line.find(subPath);
	line.erase(pos+subPath.length(),line.size());
	writeConf(line,true,"Identifier",false,"","SaXKeyBoardConf") ? cout<<"no error\n" : cout<<"error\n";
	writeConf(line,false,"Option",false,"","XkbLayout") ? cout<<"no error\n" : cout<<"error\n";
	writeConf(line,false,"Option",false,"/value",layoutVal.c_str()) ? cout<<"no error\n" : cout<<"error\n";

	error = aug_save(aug);
	if(error==-1){
		aug_print(aug,stdout,"/augeas//error");
		return false;
	}
	return true;
}

bool keyboard::expertWriteConf(){
	vector< pair<string,string> > lvg;
	vector< pair<string,string> >::iterator it,it1;
	lvg = layoutTable->getItems();
	string l,v,g,m;
	for(it=lvg.begin();it!=lvg.end();it++){
		cout<<layout[it->first]<<'\t'<<variant[it->second]<<endl;
		l.append(layout[it->first]);l.push_back(',');
		v.append(variant[it->second]);v.push_back(',');
	}
	l.erase(l.size()-1);
	v.erase(v.size()-1);
	cout<<l<<'\t'<<v<<endl;
	m = model[modelSelect->value()];
	cout<<m<<endl;
	lvg=groupTable->getItems();
	for(it=lvg.begin();it!=lvg.end();it++){
		for(it1=options.begin();it1!=options.end();it1++){	
			if(it1->first==it->second){
				cout<<it1->first<<'\t'<<it->second<<'\t'<<it1->second<<endl;
				g.append(it1->second);g.push_back(',');
				break;
			}
		}	
	}
	g.erase(g.size()-1);
	cout<<g<<endl;

	char **match;int i=0,j=0,pos=0;string line,subPath,pathParam;
	int error;
	string layoutVal = layout[layoutSelect->value()];
	int cnt = aug_match(aug,"/files/etc/X11/xorg.conf.d/*/InputClass/MatchIsKeyboard",&match);
	for(i=0;i<cnt-1;i++){
		if(strcmp(match[i],match[i+1])<0)
			j = i+1;
	}
	if(cnt)
		line.assign(match[j]);
	else
		line.assign("/files/etc/X11/xorg.conf.d/99-saxkeyboard.conf/InputClass");

	subPath.assign("InputClass");
	pos = line.find(subPath);
	line.erase(pos+subPath.size(),line.size());

	cout<<line<<endl;
	cout<<subPath;

	writeConf(line,true,"Identifier",false,"","SaXKeyBoardConf") ? cout<<"no error\n" : cout<<"error\n";
	writeConf(line,false,"Option",true,"","XkbLayout") ? cout<<"no error\n" : cout<<"error\n";
	writeConf(line,false,"Option",false,"/value",l.c_str()) ? cout<<"no error\n" : cout<<"error\n";
	writeConf(line,false,"Option",true,"","XkbVariant") ? cout<<"no error\n" : cout<<"error\n";
	writeConf(line,false,"Option",false,"/value",v.c_str()) ? cout<<"no error\n" : cout<<"error\n";
	writeConf(line,false,"Option",true,"","XkbOptions") ? cout<<"no error\n" : cout<<"error\n";
	writeConf(line,false,"Option",false,"/value",g.c_str()) ? cout<<"no error\n" : cout<<"error\n";

	error = aug_save(aug);
	if(error==-1){
		aug_print(aug,stdout,"/augeas//error");
		return false;
	}
	return true;
}


vector<string> keyboard::match(string p,string v){
	int cnt;
	const char *pattern = p.c_str();
	const char *value = v.c_str();
	char **matches;
	vector<string> s;
	bool filter = (value != NULL) && (strlen(value) > 0);
	cnt = aug_match(aug, pattern, &matches);
    	if (cnt < 0) {
        	printf("  (error matching %s)\n", pattern);
	        goto done;
    	}
    	if (cnt == 0) {
        	goto done;
    	}
	for (int i=0; i < cnt; i++) {
        	const char *val;
        	aug_get(aug, matches[i], &val);
        	if (val == NULL)
            		val = "(none)";
        	if (filter) {
            		if (!strcmp(value, val)){
                		printf("%s\n", matches[i]);
				s.push_back(matches[i]);
			}
        	} else {
            		printf("%s = %s\n", matches[i], val);
        	}
    	}
	done:
    	for (int i=0; i < cnt; i++)
        	free(matches[i]);
  	free(matches);
	return s;
}

void keyboard::loadSimpleConf(){
	vector<string> matches = match("/files/etc/X11/xorg.conf.d/*/InputClass/Option","XkbLayout");
	if(matches.size()==0){
		cerr<<"No Keyboard file exists, will load the default one"<<endl;
		return;
	}
	const char *match;int cnt;
	string temp = matches.back();
	temp.append("/value");
	cnt = aug_get(aug,temp.c_str(),&match);
	if(cnt==0 || cnt==-1){
		cerr<<"No matching nodes or illegal path expression";
		return;
	}
	string m = match;
	m = m.substr(0,m.find_first_of(','));

	map<string,string>::iterator it;
	for(it=layout.begin();it!=layout.end();it++){
		if(m==it->second){
			cout<<it->first<<endl;
			break;
		}
	}
	string x = "Your Current default selection is "+it->first + " and you can find detailed info in expert mode";
	showDefaultLayout->setValue(x);
}

vector<string> keyboard::parseOption(const char *value){
	vector<string> x;
	string temp;
	for(unsigned i=0;i<=strlen(value);i++){
		if(value[i]==','){
			x.push_back(temp);
			temp.erase();
			++i;
		}
		temp.push_back(value[i]);
	}
	temp.erase(temp.size()-1);
	x.push_back(temp);
	return x;
}

void keyboard::loadExpertConf(){
	vector<string> lMatches = match("/files/etc/X11/xorg.conf.d/*/InputClass/Option","XkbLayout");
	vector<string> vMatches = match("/files/etc/X11/xorg.conf.d/*/InputClass/Option","XkbVariant");
	vector<string> oMatches = match("/files/etc/X11/xorg.conf.d/*/InputClass/Option","XkbOptions");
	const char * match;string temp;int cnt;unsigned i;
	if(lMatches.size()==0){
		cerr<<"No Layout, so need to write a new configuration"<<endl;
		return;
	}
	temp = lMatches.back();
	temp = temp.append("/value");
	cnt = aug_get(aug,temp.c_str(),&match);
	if(cnt==0 ||cnt==-1){
		cerr<<"No Matching nodes";
		return;
	}
	vector<string> l = parseOption(match);
	vector<string> v,o;
	if(vMatches.size()!=0 && (vMatches.back()>lMatches.back())){
		temp = vMatches.back();
		temp.append("/value");
		cnt = aug_get(aug,temp.c_str(),&match);
		if(cnt==0||cnt==-1){
			cerr<<"Error in variants, No Matching Nodes";
			goto skip;
		}
		v = parseOption(match);
	}
	if(oMatches.size()==0 || (lMatches.back()>=oMatches.back()))
		goto skip;
	
	temp = oMatches.back();
	temp.append("/value");
	cnt = aug_get(aug,temp.c_str(),&match);
	if(cnt==0 || cnt==-1){
		cerr<<"Error in groups,No Matching nodes";
		goto skip;
	}
	o  = parseOption(match);

skip:
	
	for(i=0;i<v.size();i++){
		if(v[i]==""){
			v[i] = "Default";
		}
	}
	for(;i<l.size();i++){
		v.push_back("Default");
	}

	map<string,string>::iterator it;
	string var,lay;
	cout<<"Checking for values"<<endl;	
	for(i=0;i<l.size();i++){
		if(l[i]=="")continue;
		for(it=layout.begin();it!=layout.end();it++){
			if(!it->second.compare(l[i])){//|| !it->second.compare(l[i].append(' '))){
				lay = it->first;
				break;
			}
		}
		for(it=variant.begin();it!=variant.end();it++){
			if(v[i]=="Default"){
				cout<<"Default"<<endl;
				var = "Default";
				break;
			}
			 if(it->second==v[i]){
				cout<<it->first<<endl;
				var = it->first;
				break;
			}
		}
		char * temp = new char[5];
		sprintf(temp,"%d",++colNo);
		xcolNo = temp;
		layoutTable->addItem(xcolNo,lay,var);
	}
	vector< pair<string,string> >::iterator it1;
	string gc,gv;
	for(i=0;i<o.size();i++){
		for(it1=options.begin();it1!=options.end();it1++){	
			if(o[i]==it1->second){
				cout<<it1->second;
				gv = it1->first;
				while(it1->second.find(':')!=string::npos){
					--it1;
				}
				cout<<it1->second;
				gc= it1->first;
				groupTable->addItem(gc,gv);
				break;
			}
		}
	}
}	

int main(){
	
	setlocale(LC_ALL,"");
	bindtextdomain("sax3","/usr/share/locale");
	textdomain("sax3");

	keyboard * kb = new keyboard();
	kb->drawLayout();
	kb->loadConf();
	while(kb->respondToEvent());
	delete kb;	

	return 0;
 }



