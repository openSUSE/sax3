#include<iostream>
#include<vector>
#include<utility>
#include<fstream>
#include<string.h>
#include<cstdio>
extern "C"{
#include<augeas.h>
}

#define YUILogComponent "SaX3-touchpad"
#include <yui/YUILog.h>

#include "ui/yuifactory.h"

using namespace std;

class touchpad{

	augeas * aug;
	char * root,* loadpath;
	unsigned int flag;
	
	UI::YUIFactory * factory;
	UI::yDialog * dialog;
	UI::yVLayout * vL1;
	UI::yHLayout * hL1,*thL1,*thL2,*thL3;
	UI::yCheckBox * TapButton1,*TapButton2,*TapButton3,*VerticalEdgeScroll,*HorizontalEdgeScroll,*VerticalTwoFingerScroll,*HorizontalTwoFingerScroll,*CircularScroll;
	UI::yComboBox * CircularLocation,*TapButton1Click,*TapButton2Click,*TapButton3Click;
	UI::yPushButton * cancelButton,*okButton;

	bool saveConf();
	bool writeConf(string &line,bool newNode,string parameter,bool isLastParameter,string extraParam,string value);
	
	public:
	touchpad();
	void initUI();
	void respondToEvent();

};

touchpad::touchpad(){
	factory = new UI::YUIFactory();
	aug=NULL;root=NULL;flag=0;loadpath=NULL;
	yuiDebug()<<"Loading AUgeas";
	aug = aug_init(root,loadpath,flag);
	if(aug==NULL){
		yuiDebug()<<"AUGEAS NOT LOADED";
	}
}

void touchpad::initUI(){
	dialog = factory->createDialog(20,20);
	vL1 = factory->createVLayout(dialog);
	thL1 = factory->createHLayout(vL1);
	TapButton1 = factory->createCheckBox(thL1,"Emulate Mouse Button 1",false);
	TapButton1Click = factory->createComboBox(thL1,"No Of Clicks");
	TapButton1Click->setDisabled();
	TapButton1Click->addItem("1");
	TapButton1Click->addItem("2");
	TapButton1Click->addItem("3");
	
	thL2 = factory->createHLayout(vL1);
	TapButton2 = factory->createCheckBox(thL2,"Emulate Mouse Button 2",false);
	TapButton2Click = factory->createComboBox(thL2,"No Of Clicks");
	TapButton2Click->setDisabled();
	TapButton2Click->addItem("2");
	TapButton2Click->addItem("1");
	TapButton2Click->addItem("3");

	thL3 = factory->createHLayout(vL1);
	TapButton3 = factory->createCheckBox(thL3,"Emulate Mouse Button 3",false);
	TapButton3Click = factory->createComboBox(thL3,"No Of Clicks");
	TapButton3Click->setDisabled();
	TapButton3Click->addItem("3");
	TapButton3Click->addItem("2");
	TapButton3Click->addItem("1");

	VerticalEdgeScroll = factory->createCheckBox(vL1,"Enable Vertical Edge Scrolling",true);
	HorizontalEdgeScroll = factory->createCheckBox(vL1,"Enable horizontal Edge Scrolling",true);
	VerticalTwoFingerScroll = factory->createCheckBox(vL1,"Enable 2 finger Vertical Scroll",true);
	HorizontalTwoFingerScroll = factory->createCheckBox(vL1,"Enable 2 finger Horizontal Scroll",true);
	CircularScroll = factory->createCheckBox(vL1,"Enable Circular Scrolling",false);
	CircularLocation = factory->createComboBox(vL1,"Circular Scrolling Location");
	CircularLocation->addItem("Top Edges");
	CircularLocation->addItem("Top Right Corner");
	CircularLocation->addItem("Right Edge ");
	CircularLocation->addItem("Bottom Right Corner");
	CircularLocation->addItem("Bottom Edge");
	CircularLocation->addItem("Bottom Left Corner");
	CircularLocation->addItem("Left Edges");
	CircularLocation->addItem("Top Left Corner");
	CircularLocation->setDisabled();
	hL1 = factory->createHLayout(vL1);
	okButton = factory->createPushButton(hL1,"Ok");
	cancelButton = factory->createPushButton(hL1,"Cancel");
}

void touchpad::respondToEvent(){
	while(1){
		dialog->wait();
		if(cancelButton->getElement()==dialog->eventWidget()){
			break;
		}
		if(okButton->getElement()==dialog->eventWidget()){
			if(TapButton1Click->value().compare(TapButton2Click->value()) && TapButton1Click->value().compare(TapButton3Click->value()) && TapButton1Click->value().compare(TapButton3Click->value()) && TapButton2Click->value().compare(TapButton3Click->value())){
				saveConf();
				break;
			}else continue;
		}
		if(CircularScroll->isChecked())
			CircularLocation->setEnabled();
		else
			CircularLocation->setDisabled();

		if(TapButton1->isChecked()){
			TapButton1Click->setEnabled();

		}else TapButton1Click->setDisabled();

		if(TapButton2->isChecked()){
			TapButton2Click->setEnabled();
		}else TapButton2Click->setDisabled();

		if(TapButton3->isChecked()){
			TapButton3Click->setEnabled();
		}else TapButton3Click->setDisabled();

	};
}

bool touchpad::saveConf(){
	char **match;int i=0,j=0,pos=0;string line,subPath,pathParam;
	int error;
	int cnt = aug_match(aug,"/files/etc/X11/xorg.conf.d/*/InputClass/MatchIsTouchpad",&match);
	for(i=0;i<cnt-1;i++){
		if(strcmp(match[i],match[i+1])<0)
			j = i+1;
	}
	if(cnt)
		line.assign(match[j]);
	else
		line.assign("/files/etc/X11/xorg.conf.d/99-saxtouchpad.conf/InputClass");

	subPath.assign("InputClass");
	pos = line.find(subPath);
	line.erase(pos+subPath.length(),line.size());
	writeConf(line,true,"Identifier",false,"","SaXTouchpadConf") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	writeConf(line,false,"Driver",false,"","synaptics") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	writeConf(line,false,"MatchIsTouchpad",false,"","on") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";

	
	writeConf(line,false,"Option",true,"","TapButton1") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	if(TapButton1->isChecked()){
		writeConf(line,false,"Option",false,"/value","1") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}else{
		writeConf(line,false,"Option",false,"/value",TapButton1Click->value().c_str()) ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}
	
	writeConf(line,false,"Option",true,"","TapButton2") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	if(TapButton1->isChecked()){
		writeConf(line,false,"Option",false,"/value","2") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}else{
		writeConf(line,false,"Option",false,"/value",TapButton2Click->value().c_str()) ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}

	
	writeConf(line,false,"Option",true,"","TapButton3") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	if(TapButton1->isChecked()){
		writeConf(line,false,"Option",false,"/value","3") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}else{
		writeConf(line,false,"Option",false,"/value",TapButton3Click->value().c_str()) ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}

	
	writeConf(line,false,"Option",true,"","VertEdgeScroll") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	if(VerticalEdgeScroll->isChecked()){
		writeConf(line,false,"Option",false,"/value","on") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}else{
		writeConf(line,false,"Option",false,"/value","off") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}

	writeConf(line,false,"Option",true,"","HorizEdgeScroll") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	if(VerticalEdgeScroll->isChecked()){
		writeConf(line,false,"Option",false,"/value","on") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}else{
		writeConf(line,false,"Option",false,"/value","off") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}

	writeConf(line,false,"Option",true,"","VertTwoFingerScroll") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	if(VerticalTwoFingerScroll->isChecked()){
		writeConf(line,false,"Option",false,"/value","on") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}else{
		writeConf(line,false,"Option",false,"/value","off") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}


	writeConf(line,false,"Option",true,"","HorizTwoFingerScroll") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	if(HorizontalTwoFingerScroll->isChecked()){
		writeConf(line,false,"Option",false,"/value","on") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}else{
		writeConf(line,false,"Option",false,"/value","off") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}

	writeConf(line,false,"Option",true,"","CircularScrolling") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	if(CircularScroll->isChecked()){
		writeConf(line,false,"Option",false,"/value","on") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
		writeConf(line,false,"Option",false,"","CircScrollTrigger") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
		if(!CircularLocation->value().compare("All Edges")){
			writeConf(line,false,"Option",false,"/value","0") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
		}
		if(!CircularLocation->value().compare("Top Edges")){
			writeConf(line,false,"Option",false,"/value","1") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
		}
		if(!CircularLocation->value().compare("Top Right Corner")){
			writeConf(line,false,"Option",false,"/value","2") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
		}
		if(!CircularLocation->value().compare("Right Edge ")){
			writeConf(line,false,"Option",false,"/value","3") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
		}
		if(!CircularLocation->value().compare("Bottom Right Corner")){
			writeConf(line,false,"Option",false,"/value","4") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
		}
		if(!CircularLocation->value().compare("Bottom Edge")){
			writeConf(line,false,"Option",false,"/value","5") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
		}
		if(!CircularLocation->value().compare("Bottom Left Corner")){
			writeConf(line,false,"Option",false,"/value","6") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
		}
		if(!CircularLocation->value().compare("Left Edges")){
			writeConf(line,false,"Option",false,"/value","7") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
		}
		if(!CircularLocation->value().compare("Top Left Corner")){
			writeConf(line,false,"Option",false,"/value","8") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
		}

	}else{
		writeConf(line,false,"Option",false,"/value","off") ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";
	}

	error = aug_save(aug);
	if(error==-1){
		aug_print(aug,stdout,"/augeas//error");
		return false;
	}
	return true;

}

bool touchpad::writeConf(string &line,bool newNode,string parameter,bool isLastParameter,string extraParam,string value){
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
	yuiDebug()<<pathParam<<endl;
	error = aug_set(aug,pathParam.c_str(),value.c_str());
	
	if(error==-1)
		return false;
	return true;
}

int main(){
	touchpad * t = new touchpad();
	t->initUI();
	t->respondToEvent();
	delete t;
	return 0;
}	
