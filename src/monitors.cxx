
#include<dirent.h>
#include<stdlib.h>
#include<string>
#include<string.h>
#include<stdio.h>
#include<fstream>
#include<vector>
extern "C"{
#include<augeas.h>
}

#include"ui/yuifactory.h"

using namespace std;

class Monitors{
	vector<string> driverList;
	vector<string> resolutionList;

	augeas * aug;char *root,*loadpath;unsigned int flag;
	
	UI::YUIFactory * factory;
	UI::yDialog * dialog;
	UI::yVLayout * vL1,*vL2,*vL3,*vL4;
	UI::yHLayout * hL1,*hL2,*hL3,*hL4,*hL5;
	UI::yPushButton * ok,*cancel;
	UI::yComboBox * driverCombo,*resolutionCombo,*depthCombo;
	UI::yIntField * horizontalLow,*horizontalHigh;
	UI::yIntField * verticalLow,*verticalHigh;
	UI::yCheckBox * disableDPMS,*enableAdvance,*customCVT;
	UI::yIntField * xAxis,*yAxis,*refreshRate;
	void fillUpDriverCombo();
	void fillUpResolutionCombo();
	void fillUpDepthCombo();
	void separateResolution(string&);
	void saveConf();
	string calculateCVT();
	bool writeConf(string &line,bool newNode,string parameter,bool isLastParameter,string extraParam,string value);
	public:
	Monitors();
	void detectDrivers();
	void detectResolution();
	void initUI();
	bool respondToEvent();
};

string Monitors::calculateCVT(){
	string cvt = "cvt ";
	if(!customCVT->isChecked()){
	string resolution = resolutionCombo->value();
	string x = resolution.substr(0,resolution.find('x'));
	string y = resolution.substr(resolution.find('x')+1,resolution.size());
	cvt.append(x);
	cvt.push_back(' ');
	cvt.append(y);

	}else{
		char * x = new char[10];
		sprintf(x,"%d",xAxis->value());
		cvt.append(x);	
		cvt.push_back(' ');		
		char * y = new char[10];
		sprintf(y,"%d",yAxis->value());
		cvt.append(y);
		cvt.push_back(' ');\
		char * ref = new char[10];
		sprintf(ref,"%d",refreshRate->value());
		cvt.append(ref);
		cvt.push_back(' ');

	}
	cvt.append(" > /tmp/cvt.tmp");
	system(cvt.c_str());
	ifstream file("/tmp/cvt.tmp");
	if(file.is_open()){
		if(file.good()){
			getline(file,cvt);
			getline(file,cvt);
		}
	}
	cvt = cvt.substr(cvt.find(' ')+1,string::npos);
	cout<<cvt;
	return cvt;
}
void Monitors::fillUpDepthCombo(){
	depthCombo->addItem("24");
	depthCombo->addItem("16");
}

void Monitors::fillUpResolutionCombo(){
	for(int i=0;i<resolutionList.size();i++)
		resolutionCombo->addItem(resolutionList[i]);
}

void Monitors::separateResolution(string &temp){
	temp = temp.erase(0,temp.find_first_not_of(' '));
	temp = temp.erase(temp.find_first_of(' '),string::npos);
}

void Monitors::detectResolution(){
	string temp;int flag=0;
	ifstream file("/tmp/sax-xrandr.tmp");
	cout<<"IN DETECT RESOLUTION---->";
	if(file.is_open()){
		cout<<"File opened";
		while(file.good()){
			getline(file,temp);
			if(temp.find(' ')!=0 && flag)
			break;
			if(temp.find(' ')==0){
				flag=1;
				separateResolution(temp);
				resolutionList.push_back(temp);
			}
		}
	}else
		cout<<"Not opened";

}

void Monitors::fillUpDriverCombo(){
	for(int i=0;i<driverList.size();i++)
		driverCombo->addItem(driverList[i]);
}
Monitors::Monitors(){
        factory = new UI::YUIFactory();
        aug=NULL;root=NULL;flag=0;loadpath=NULL;
        cout<<"Loading AUgeas";
        aug = aug_init(root,loadpath,flag);
        if(aug==NULL){
              cout<<"AUGEAS NOT LOADED";
        }
}

void Monitors::detectDrivers(){
	system("rm /tmp/sax*");
	system(" xinit `which xrandr` -- :9 -logfile `mktemp --tmpdir sax3.XXXXX` >  /tmp/sax-xrandr.tmp");
	string temp;
	struct dirent * ep;
	DIR * dp;
	dp = opendir("/tmp/");
	if(dp!=NULL){
//		cerr<<"Directory Opened"<<endl;
		while(ep=readdir(dp)){
			temp = ep->d_name;	
			if(temp.find("sax3")!=-1 && temp.find(".old")==-1)
				break;
		};
	}
	temp =temp.insert(0,"/tmp/");
	cout<<temp;
	ifstream file(temp.c_str());
	string temp1;
	int pos;
	if(file.is_open()){
		while(file.good()){
			getline(file,temp1);
			pos = temp1.find("Matched ");
			if(pos!=-1)
				break;
		}
	}
	
	while(pos!=-1){
		cout<<temp1<<endl;
		pos = temp1.find("Matched ");
		if(pos!=-1){
			string matched = "Matched ";
			int next_pos = pos+matched.size();
			temp1.erase(0,next_pos);
			next_pos = temp1.find(' ');
			temp1.erase(next_pos,string::npos);
			driverList.push_back(temp1);
		}
		getline(file,temp1);
	};
}

void Monitors::initUI(){
	dialog = factory->createDialog(60,12);
	vL1 = factory->createVLayout(dialog);
	driverCombo = factory->createComboBox(vL1,"Select the driver");	
	fillUpDriverCombo();
	vL2 = factory->createVLayout(vL1);
	hL1 = factory->createHLayout(vL2);
	resolutionCombo = factory->createComboBox(hL1,"Select resolution");
	fillUpResolutionCombo();
	depthCombo = factory->createComboBox(hL1,"Depth");
	fillUpDepthCombo();
	enableAdvance = factory->createCheckBox(vL1,"Enable Advanced Settings",false);
	
	hL2 = factory->createHLayout(vL1);
	horizontalLow = factory->createIntField(hL2,"Horizontal Refresh Rate(min value)",20,40,30);
	horizontalLow->setDisabled();
	horizontalHigh = factory->createIntField(hL2,"Horizontal Refresh Rate(max value)",20,40,30);
	horizontalHigh->setDisabled();
	hL3 = factory->createHLayout(vL1);
	verticalLow = factory->createIntField(hL3,"Vertical Refresh Rate(min value)",20,40,30);
	verticalLow->setDisabled();
	verticalHigh = factory->createIntField(hL3,"Vertical Refresh Rate(max value)",20,40,30);
	verticalHigh->setDisabled();
	
	customCVT = factory->createCheckBox(vL1,"I want my own CVT",false);
	hL5 = factory->createHLayout(vL1);
	xAxis = factory->createIntField(hL5,"X Axis",400,1280,4000);
	xAxis->setDisabled();
	yAxis = factory->createIntField(hL5,"Y Axis",400,1280,3000);
	yAxis->setDisabled();
	refreshRate = factory->createIntField(hL5,"Refresh Rate",20,60,200);
	refreshRate->setDisabled();
	hL4 = factory->createHLayout(vL1);
	ok = factory->createPushButton(hL4,"Ok");
	cancel = factory->createPushButton(hL4,"Cancel");
}

bool Monitors::respondToEvent(){
	while(1){
		dialog->wait();
		if(dialog->eventWidget()==enableAdvance->getElement()){
			if(!enableAdvance->isChecked()){
				horizontalLow->setDisabled();
				horizontalHigh->setDisabled();
				verticalHigh->setDisabled();
				verticalLow->setDisabled();
			}else{
				horizontalLow->setEnabled();
				horizontalHigh->setEnabled();
				verticalHigh->setEnabled();
				verticalLow->setEnabled();
			}
		}
		if(dialog->eventWidget()==customCVT->getElement()){
			if(!customCVT->isChecked()){
				xAxis->setDisabled();
				yAxis->setDisabled();
				refreshRate->setDisabled();
			}else{
				xAxis->setEnabled();
				yAxis->setEnabled();
				refreshRate->setEnabled();
			}
		}
		if(dialog->eventWidget()==ok->getElement()){
			saveConf();
			break;
		}
		if(dialog->eventWidget()==cancel->getElement()){
			break;
		}
	};
	return false;
}

void Monitors::saveConf(){
	char **match;int i=0,j=0,pos=0;string line,subPath,pathParam;
        int error;

        int cnt = aug_match(aug,"/files/etc/X11/xorg.conf.d/*/Monitor/*",&match);

        for(i=0;i<cnt-1;i++){
                if(strcmp(match[i],match[i+1])<0)
                        j = i;
        }

        if(cnt)
                line.assign(match[j]);
        else
                line.assign("/files/etc/X11/xorg.conf.d/99-saxmonitors.conf/Monitor");

        subPath.assign("Monitor");
        pos = line.find(subPath);
        line.erase(pos+subPath.length(),line.size());
	
	writeConf(line,true,"Identifier",false,"","SaX3-monitor") ? cout<<"no error\n" : cout<<"error\n";

	if(enableAdvance->isChecked()){
		cout<<"In Enable Advanced";
		string horiz,vert;
		char * temp = new char[10];
		sprintf(temp,"%d",horizontalLow->value());
		horiz = temp;
		horiz.append("-");
		sprintf(temp,"%d",horizontalHigh->value());
		horiz.append(temp);
		writeConf(line,false,"HorizSync",false,"",horiz.c_str());

		sprintf(temp,"%d",verticalLow->value());
		vert = temp;
		vert.append("-");
		sprintf(temp,"%d",horizontalHigh->value());
		vert.append(temp);
		writeConf(line,false,"VertRefresh",false,"",vert.c_str());
	}
	
	string cvt = calculateCVT();
	
	writeConf(line,false,"Modeline",false,"",cvt) ? cout<<"no error\n":cout<<"error\n";

        cnt = aug_match(aug,"/files/etc/X11/xorg.conf.d/*/Device/*",&match);

        for(i=0;i<cnt-1;i++){
	                if(strcmp(match[i],match[i+1])<0)
			                        j = i;
		        }

        if(cnt)
	                line.assign(match[j]);
        else
	                line.assign("/files/etc/X11/xorg.conf.d/99-saxdevice.conf/Device");

        subPath.assign("Device");
        pos = line.find(subPath);
        line.erase(pos+subPath.length(),line.size());
        writeConf(line,true,"Identifier",false,"","SaX3-device") ? cout<<"no error\n" : cout<<"error\n";
	string temp = driverCombo->value();
	cout<<temp;
        writeConf(line,false,"Driver",false,"",temp.c_str()) ? cout<<"no error\n" : cout<<"error\n";

        cnt = aug_match(aug,"/files/etc/X11/xorg.conf.d/*/Screen/*",&match);

        for(i=0;i<cnt-1;i++){
                        if(strcmp(match[i],match[i+1])<0)
                                                j = i;
                        }

        if(cnt)
                        line.assign(match[j]);
        else
                        line.assign("/files/etc/X11/xorg.conf.d/99-saxscreen.conf/Screen");

        subPath.assign("Screen");
        pos = line.find(subPath);
        line.erase(pos+subPath.length(),line.size());

        writeConf(line,true,"Identifier",false,"","SaX3-screen") ? cout<<"no error\n" : cout<<"error\n";
        writeConf(line,false,"Device",false,"","SaX3-device") ? cout<<"no error\n" : cout<<"error\n";
        writeConf(line,false,"Monitor",false,"","SaX3-monitor") ? cout<<"no error\n" : cout<<"error\n";
        writeConf(line,false,"DefaultDepth",false,"",depthCombo->value().c_str()) ? cout<<"no error\n" : cout<<"error\n";
        writeConf(line,false,"Display",true,"/Depth",depthCombo->value().c_str()) ? cout<<"no error\n" : cout<<"error\n";
	string mode = resolutionCombo->value();mode.append("_60.00");
        writeConf(line,false,"Display",false,"/Modes",mode.c_str()) ? cout<<"no error\n" : cout<<"error\n";
	aug_save(aug);
}


bool Monitors::writeConf(string &line,bool newNode,string parameter,bool isLastParameter,string extraParam,string value){
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


int main(){
	Monitors * m = new Monitors();
	m->detectDrivers();
	m->detectResolution();
	m->initUI();
	m->respondToEvent();
	delete m;
	return 0;
}
	
