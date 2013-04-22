#include <iostream>
#include <locale.h>
#include <libintl.h>
#include <vector>
#include <utility>
#include <fstream>
#include <string.h>
#include <cstdio>
#include <sys/types.h>
#include <dirent.h>
extern "C" {
#include <augeas.h>
}

#include "ui/yuifactory.h"

#define YUILogComponent "SaX3-mouse"
#include <yui/YUILog.h>
#include <yui/YApplication.h>

#define _(STRING) gettext(STRING)

using namespace std;

class Mouse
{

   class Details
   {
      string product;
      string vendor;
      string name;
      string protocol;
      string device;
      int InvX;
      int InvY;
      int AngleOffset;
      int Emulate3Buttons;
      int Emulate3Timeout;
      int EmulateWheel;
      int EmulateWheelTimeout;
   public:
      void setProduct(string);
      void setVendor(string);
      void setName(string);
      void setDevice(string);
      void setEmulate3(int);
      void setEmulate3Timeout(int);
      void setEmulateWheel(int);
      void setEmulateWheelTimeout(int);
      void setInvX(int);
      void setInvY(int);
      void setAngleOffset(int);
      string getName();
      string getVendor();
      string getProduct();
      int getEmulate3();
      int getEmulate3Timeout();
      int getEmulateWheel();
      int getEmulateWheelTimeout();
      int getAngleOffset();
      int getInvX();
      int getInvY();
      Details();
   };

   string fileName,line;
   vector<Details*> d;

   augeas * aug;
   char * root,* loadpath;
   unsigned int flag;

   UI::YUIFactory * factory;
   UI::yDialog *dialog;
   UI::yHLayout * hl1,*enableButton3Layout,*enableWheelLayout,*buttonLayout;
   UI::yVLayout * vl1;
   UI::yPushButton *okButton,*cancelButton;
   UI::yRadioButtonGroup * button3,*wheel;
   UI::yLabel * button3Label,*enableButton3Label,*wheelLabel,*enableWheelLabel;
   UI::yIntField * timeout,*wheeltimeout,*AngleOffset;
   UI::yCheckBox * InvX,*InvY;
   UI::yComboBox * mouseList;
   void getProductVendor();
   void getName();
   void fillUpMouseList();
   bool writeConf(string &line,bool newNode,string parameter,bool isLastParameter,string extraParam,string value);
   void loadState();
   void saveState();
public:
   void autodetect();
   void initUI();
   bool saveConf();
   bool respondToEvent();
   Mouse();
   ~Mouse();
};

void Mouse::Details::setAngleOffset(int v)
{
   AngleOffset = v;
}

int Mouse::Details::getAngleOffset()
{
   return AngleOffset;
}

void Mouse::Details::setInvX(int v)
{
   InvX = v;
}

int Mouse::Details::getInvX()
{
   return InvX;
}

void Mouse::Details::setInvY(int v)
{
   InvY = v;
}

int Mouse::Details::getInvY()
{
   return InvY;
}

void Mouse::Details::setEmulateWheel(int v)
{
   EmulateWheel = v;
}

int Mouse::Details::getEmulateWheel()
{
   return EmulateWheel;
}

void Mouse::Details::setEmulateWheelTimeout(int v)
{
   EmulateWheelTimeout = v;
}

int Mouse::Details::getEmulateWheelTimeout()
{
   return EmulateWheelTimeout;
}

void Mouse::Details::setEmulate3(int v)
{
   Emulate3Buttons = v;
}

int Mouse::Details::getEmulate3()
{
   return Emulate3Buttons;
}

void Mouse::Details::setEmulate3Timeout(int v)
{
   Emulate3Timeout = v;
}

int Mouse::Details::getEmulate3Timeout()
{
   return Emulate3Timeout;
}

string Mouse::Details::getProduct()
{
   return product;
}

string Mouse::Details::getVendor()
{
   return vendor;
}

string Mouse::Details::getName()
{
   return name;
}

void Mouse::Details::setProduct(string p)
{
   product = p;
   for(unsigned i=0; i<4-product.size(); i++)
      product.insert(product.begin(),'0');
}

void Mouse::Details::setVendor(string v)
{
   vendor = v;
   for(unsigned i=0; i<4-vendor.size(); i++) {
      vendor.insert(vendor.begin(),'0');
   }
}

void Mouse::Details::setName(string n)
{
   name = n;
   Emulate3Buttons = 0;
   EmulateWheel = 0;
}

void Mouse::Details::setDevice(string d)
{
   device = d;
}

Mouse::Details::Details()
{
   protocol = "Auto";
}

void Mouse::loadState()
{
   unsigned i;
   for(i=0; i<d.size(); i++)
      if(d[i]->getName()==mouseList->value())
         break;
   int b3 = d[i]->getEmulate3();
   if(b3==0) {
      button3->setValue(0,0);
      button3->setValue(1,1);
      timeout->setDisabled();
   } else {
      button3->setValue(1,0);
      button3->setValue(0,1);
      timeout->setEnabled();
   }
   timeout->setValue(d[i]->getEmulate3Timeout());

   int w = d[i]->getEmulateWheel();
   if(w==0) {
      wheel->setValue(0,0);
      wheel->setValue(1,1);
      wheeltimeout->setDisabled();
   } else {
      wheel->setValue(1,0);
      wheel->setValue(0,1);
      wheeltimeout->setEnabled();
   }
   wheeltimeout->setValue(d[i]->getEmulateWheelTimeout());
   if(d[i]->getInvX()) {
      InvX->setChecked(true);
   } else {
      InvX->setChecked(false);
   }

   if(d[i]->getInvY()) {
      InvY->setChecked(true);
   } else {
      InvY->setChecked(false);
   }
   AngleOffset->setValue(d[i]->getAngleOffset());
}

void Mouse::saveState()
{
   unsigned i;
   for(i=0; i<d.size(); i++)
      if(d[i]->getName()==mouseList->value())
         break;
   string b3 = button3->selectedLabel();

   b3 = b3.erase(b3.find('&'),b3.find('&')+1);
   if(!b3.compare("Yes")) {
      d[i]->setEmulate3(1);
   } else {
      d[i]->setEmulate3(0);
   }
   d[i]->setEmulate3Timeout(timeout->value());

   string w = wheel->selectedLabel();
   w = w.erase(w.find('&'),w.find('&'));

   if(!w.compare("Yes")) {
      d[i]->setEmulateWheel(1);
   } else {
      d[i]->setEmulateWheel(0);
   }
   d[i]->setEmulateWheelTimeout(wheeltimeout->value());
   if(InvX->isChecked()) {
      yuiDebug()<<"In here";
      d[i]->setInvX(1);
   } else {
      d[i]->setInvX(0);
   }
   if(InvY->isChecked()) {
      d[i]->setInvY(1);
   } else {
      d[i]->setInvY(0);
   }
   d[i]->setAngleOffset(AngleOffset->value());
}
void Mouse::getProductVendor()
{
   string pv = line.substr(line.find_first_of('/')+1,line.find_last_of('/'));
   string v = pv.substr(0,pv.find_first_of('/'));
   d.back()->setVendor(v);
   pv.erase(0,pv.find_first_of('/')+1);
   string p = pv.substr(0,pv.find_first_of('/'));
   d.back()->setProduct(p);
}

void Mouse::getName()
{
   int x = line.find_last_of('"')-6;
   string n = line.substr(line.find_first_of('"')+1,x);
   d.back()->setName(n);
}


void Mouse::autodetect()
{
   DIR* base;
   struct dirent* to_test;
   ifstream myfile;
   string device;

   base = opendir("/sys/class/input");

   while((to_test = readdir(base)) != NULL) {
      yuiDebug()<<"Testing possible mouse " << to_test->d_name << endl;
      if(strncmp(to_test->d_name, "mouse", 5)==0) {
         fileName  = "/sys/class/input/";
         fileName += to_test->d_name;
         fileName += "/device/uevent";
         yuiDebug()<<"Got mouse " << fileName.c_str() << endl;
         myfile.open(fileName.c_str());
         if(myfile.is_open()) {
            d.push_back(new Details());
            // TODO: read correct device
            device = "/dev/input/";
            device+= to_test->d_name;
            d.back()->setDevice(device);
            while(myfile.good()) {
               getline(myfile,line);
               if(!line.find("PRODUCT"))
                  getProductVendor();
               if(!line.find("NAME"))
                  getName();
            }
         }
         myfile.close();
      }
   }

   closedir(base);
}

void Mouse::initUI()
{
   dialog = factory->createDialog(40,10);

   vl1 = factory->createVLayout(dialog);
   hl1 = factory->createHLayout(vl1);

   mouseList = factory->createComboBox(vl1,"Auto Detected Mouse");
   fillUpMouseList();
   button3Label = factory->createLabel(vl1,"3 button Emulation Options");
   enableButton3Layout = factory->createHLayout(vl1);
   enableButton3Label = factory->createLabel(enableButton3Layout,"Enable 3 Buttons");
   button3 = factory->createRadioButtonGroup(enableButton3Layout);
   button3->addButton("Yes");
   button3->addButton("No");
   timeout = factory->createIntField(vl1,"3 Button Timeout",0,1000,50);
   timeout->setDisabled();
   wheelLabel = factory->createLabel(vl1,"Wheel Emulation Options");
   enableWheelLayout = factory->createHLayout(vl1);
   enableWheelLabel = factory->createLabel(enableWheelLayout,"Enable Wheel Emulation");
   wheel = factory->createRadioButtonGroup(enableWheelLayout);
   wheel->addButton("Yes");
   wheel->addButton("No");
   wheeltimeout = factory->createIntField(vl1,"Wheel Button Timeout",0,1000,200);
   wheeltimeout->setDisabled();
   InvX = factory->createCheckBox(vl1,"Invert X Axis",false);
   InvY = factory->createCheckBox(vl1,"Invert Y Axis",false);

   AngleOffset = factory->createIntField(vl1,"Angle Offset in degrees",0,360,0);
   buttonLayout = factory->createHLayout(vl1);
   okButton = factory->createPushButton(buttonLayout,"Save");
   cancelButton = factory->createPushButton(buttonLayout,"Close");
}
Mouse::~Mouse()
{
   delete cancelButton;
   delete okButton;
   delete buttonLayout;
   delete AngleOffset;
   delete InvY;
   delete InvX;
   delete wheeltimeout;
   delete wheel;
   delete enableWheelLabel;
   delete enableWheelLayout;
   delete wheelLabel;
   delete button3;
   delete enableButton3Label;
   delete enableButton3Layout;
   delete button3Label;
   delete mouseList;
   delete hl1;
   delete vl1;
   delete dialog;
}

void Mouse::fillUpMouseList()
{
   for(unsigned i=0; i<d.size(); i++) {
      if(d[i]->getName().find("ETPS")==string::npos && d[i]->getName().find("ALPS")==string::npos)
         mouseList->addItem(d[i]->getName());
   }
}

Mouse::Mouse()
{
   factory = new UI::YUIFactory();
   aug=NULL;
   root=NULL;
   flag=0;
   loadpath=NULL;
   yuiDebug()<<"Loading AUgeas";
   aug = aug_init(root,loadpath,flag);
   if(aug==NULL) {
      yuiDebug()<<"AUGEAS NOT LOADED";
   }

}

bool Mouse::respondToEvent()
{

   while(1) {
      dialog->wait();
      if(cancelButton->getElement()==dialog->eventWidget()) {
         yuiDebug()<<"Exitting";
         break;
      }
      if(okButton->getElement()==dialog->eventWidget()) {
         saveConf();
      }
      if(button3->isButton(0,dialog->eventWidget())) {
         timeout->setEnabled();
      }
      if(button3->isButton(1,dialog->eventWidget())) {
         timeout->setDisabled();
      }
      if(wheel->isButton(0,dialog->eventWidget())) {
         wheeltimeout->setEnabled();
      }
      if(wheel->isButton(1,dialog->eventWidget())) {
         wheeltimeout->setDisabled();
      }
      if(mouseList->getElement()==dialog->eventWidget()) {
         loadState();
      }
      saveState();
   };
   return true;
}

bool Mouse::saveConf()
{
   char **match;
   int i=0,j=0,pos=0;
   string line,subPath,pathParam;
   int error;

   int cnt = aug_match(aug,"/files/etc/X11/xorg.conf.d/*/InputClass/*",&match);

   for(i=0; i<cnt-1; i++) {
      if(strcmp(match[i],match[i+1])<0)
         j = i;
   }

   if(cnt)
      line.assign(match[j]);
   else
      line.assign("/files/etc/X11/xorg.conf.d/99-saxmouse.conf/InputClass");


   subPath.assign("InputClass");
   pos = line.find(subPath);
   line.erase(pos+subPath.length(),line.size());
   yuiDebug()<<endl<<line<<endl;

   string Identifier = mouseList->value();
   Identifier.append("-SaX-MouseConfig");
   yuiDebug()<<Identifier;
   writeConf(line,true,"Identifier",false,"",Identifier.c_str()) ? yuiDebug()<<"no error\n" : yuiDebug()<<"error\n";

   for(unsigned i=0; i<d.size(); i++) {
      if(mouseList->value()==d[i]->getName()) {
         break;
      }
   }

   writeConf(line,false,"MatchVendor",false,"",d[i]->getVendor().c_str()) ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";
   writeConf(line,false,"MatchProduct",false,"",d[i]->getProduct().c_str()) ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";
   writeConf(line,false,"MatchIsPointer",false,"","on") ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";
   writeConf(line,false,"Driver",false,"","mouse") ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";

   if(button3->selectedLabel()=="&Yes") {
      writeConf(line,false,"Option",true,"","Emulate3Buttons") ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";
      writeConf(line,false,"Option",false,"/value","on")?yuiDebug()<<"NoError\n":yuiDebug()<<"Error\n";
      writeConf(line,false,"Option",true,"","ChordMiddle") ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";
      writeConf(line,false,"Option",false,"/value","on")?yuiDebug()<<"NoError\n":yuiDebug()<<"Error\n";
      char * s = new char[4];
      sprintf(s,"%d",timeout->value());
      writeConf(line,false,"Option",true,"","Emulate3Timeout") ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";
      writeConf(line,false,"Option",false,"/value",s)?yuiDebug()<<"NoError\n":yuiDebug()<<"Error\n";

   }
   if(wheel->selectedLabel()=="Y&es") {
      writeConf(line,false,"Option",true,"","EmulateWheel") ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";
      writeConf(line,false,"Option",false,"/value","on")?yuiDebug()<<"NoError\n":yuiDebug()<<"Error\n";
      char * s = new char[4];
      sprintf(s,"%d",wheeltimeout->value());
      writeConf(line,false,"Option",true,"","EmulateWheelTimeout") ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";
      writeConf(line,false,"Option",false,"/value",s)?yuiDebug()<<"NoError\n":yuiDebug()<<"Error\n";

   }


   if(InvX->isChecked()) {
      writeConf(line,false,"Option",true,"","InvX") ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";
      writeConf(line,false,"Option",false,"/value","on")?yuiDebug()<<"NoError\n":yuiDebug()<<"Error\n";
   }
   if(InvY->isChecked()) {
      writeConf(line,false,"Option",true,"","InvY") ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";
      writeConf(line,false,"Option",false,"/value","on")?yuiDebug()<<"NoError\n":yuiDebug()<<"Error\n";
   }

   char *s = new char[3];
   sprintf(s,"%d",AngleOffset->value());
   writeConf(line,false,"Option",true,"","AngleOffset") ? yuiDebug()<<"No Error\n" : yuiDebug()<<"Error\n";
   writeConf(line,false,"Option",false,"/value",s)?yuiDebug()<<"NoError\n":yuiDebug()<<"Error\n";
   error = aug_save(aug);

   if(error==-1) {
      aug_print(aug,stdout,"/augeas//error");
      return false;
   }
   return true;
}

bool Mouse::writeConf(string &line,bool newNode,string parameter,bool isLastParameter,string extraParam,string value)
{
   string pathParam;
   int error;
   pathParam.assign(line);
   if(newNode==true) {
      pathParam.append("[last()+1]/");
   } else {
      pathParam.append("[last()]/");
   }
   pathParam.append(parameter);
   if(isLastParameter) {
      pathParam.append("[last()+1]");
   } else {
      pathParam.append("[last()]");
   }
   pathParam.append(extraParam);
   yuiDebug()<<pathParam<<endl;
   error = aug_set(aug,pathParam.c_str(),value.c_str());

   if(error==-1)
      return false;
   return true;
}


int main()
{
   // Set up locales
   setlocale(LC_ALL,"");
   bindtextdomain("sax3","/usr/share/locale");
   textdomain("sax3");
   YUI::app()->setApplicationTitle(_("SaX 3: Mouse configuration"));

   // Enable debugging
   YUILog::enableDebugLogging();

   Mouse * m = new Mouse();
   m->autodetect();
   m->initUI();
   m->respondToEvent();
   delete m;
   return 0;
}
