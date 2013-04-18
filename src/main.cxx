#include <iostream>

#include<locale.h>
#include<libintl.h>
#include<dirent.h>
#include<sys/types.h>
#include<string.h>
#include<list>
#include<unistd.h>
#include<errno.h>

extern "C"{
#include<augeas.h>
}

#include "ui/yuifactory.h"

#define _(STRING) gettext(STRING)

using namespace std;

/*! \class Init
    \brief This class initializes the window layout and the plugin mechanism

    It scans /usr/share/sax3/modules.d/ to look for the modules available and once the modules are found, they are added to the main dialog
    */

class Init{

	DIR * dp;
	struct dirent * ep;
	
	augeas * aug;
	char  *root,*loadpath;
	unsigned int flag;
	const char *value;
	int err;
	char* getEntry;
	vector<UI::yPushButton*> button;
	vector<UI::yImage*> image;
	vector<string> execs;
//	UI::yImage image;UI::yPushButton button;
	protected:
		void makeEntry(char s1[],char s2[],char s3[]){
			strcpy(getEntry,s1);
			strcat(getEntry,s2);
			strcat(getEntry,s3);
		}

	public:
	Init(){
		aug=NULL;root=NULL;loadpath=NULL;flag=0;
		UI::YUIFactory * factory = new UI::YUIFactory();
		UI::yDialog * dialog = factory->createDialog(30,10);
		UI::yVLayout * mainLayout = factory->createVLayout(dialog);
		dp = opendir("/usr/share/sax3/modules.d/");
		aug = aug_init(root,loadpath,flag);
		aug_set(aug,"/augeas/load/Desktop/lens", "Desktop.lns");
		aug_set(aug,"/augeas/load/Desktop/incl[last()+1]","/usr/share/sax3/modules.d/*");
		err = aug_load(aug);
		cout<<"------------------------------------------------------------------<<<"<<err<<">>>";
		if(dp!=NULL){
			cout<<"DP IF";
			while(ep = readdir(dp)){
				if(strcmp(ep->d_name,".") && strcmp(ep->d_name,"..")){
					cout<<"__________________________________________________________"<<err<<endl;
					UI::yHLayout * hLayout = factory->createHLayout(mainLayout);
					getEntry = new char[300];
					makeEntry("/files/usr/share/sax3/modules.d/",ep->d_name,"/*/Icon");
					err = aug_get(aug,getEntry,&value);
					cout<<getEntry;
					if(err==1)
					image.push_back(factory->createImage(hLayout,_(value)));
					delete getEntry;

					getEntry = new char[100];
					makeEntry("/files/usr/share/sax3/modules.d/",ep->d_name,"/*/Name");
					err = aug_get(aug,getEntry,&value);
					if(err==1)
					button.push_back(factory->createPushButton(hLayout,_(value)));
					delete getEntry;	

					getEntry = new char[100];
					makeEntry("/files/usr/share/sax3/modules.d/",ep->d_name,"/*/Exec");
					err = aug_get(aug,getEntry,&value);
					if(err==1)
					execs.push_back(value);
					
//					cout<<value<<endl;
					delete getEntry;
				}
			}

		}	 
		dialog->wait();
		for(int i=0;i<button.size();i++){
			if(dialog->eventWidget()==button[i]->getElement()){
				getEntry = new char[100];
				string temp = button[i]->value();
				temp.erase(temp.find('&'),temp.find('&'));
				cout<<execs[i]<<endl;
				err = execlp(execs[i].c_str(),NULL);//NULL,NULL);
				break;
			}
		}
	}
};

int main(){
	
	setlocale(LC_ALL,"");
	bindtextdomain("sax3","/usr/share/locale");
	textdomain("sax3");

	new Init();
	return 0;
}



