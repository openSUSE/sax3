#ifndef UIF_H_
#define UIF_H_

#include<iostream>
#include "ui.h"

namespace UI
{
class YUIFactory;
class UIFactory
{
public:

   virtual Dialog * createDialog(int,int) = 0;

   virtual HLayout * createHLayout(HLayout *)=0;
   virtual HLayout * createHLayout(VLayout *)=0;
   virtual HLayout * createHLayout(Dialog *)=0;


   virtual VLayout * createVLayout(HLayout *)=0;
   virtual VLayout * createVLayout(VLayout *)=0;
   virtual VLayout * createVLayout(Dialog *)=0;

   virtual Label * createLabel(Dialog *,std::string) = 0;
   virtual Label * createLabel(HLayout *,std::string) = 0;
   virtual Label * createLabel(VLayout *,std::string) = 0;

   virtual PushButton * createPushButton(Dialog *,std::string)=0;
   virtual PushButton * createPushButton(HLayout *,std::string)=0;
   virtual PushButton * createPushButton(VLayout *,std::string)=0;

   virtual Image * createImage(Dialog *,std::string)=0;
   virtual Image * createImage(HLayout *,std::string)=0;
   virtual Image * createImage(VLayout *,std::string)=0;

   virtual ComboBox * createComboBox(Dialog *,std::string)=0;
   virtual ComboBox * createComboBox(HLayout *,std::string)=0;
   virtual ComboBox * createComboBox(VLayout *,std::string)=0;

   virtual MultiSelectionBox * createMultiSelectionBox(Dialog *,std::string)=0;
   virtual MultiSelectionBox * createMultiSelectionBox(HLayout*,std::string)=0;
   virtual MultiSelectionBox * createMultiSelectionBox(VLayout *,std::string)=0;

   virtual Table * createTable(Dialog *,std::string,std::string,std::string)=0;
   virtual Table * createTable(HLayout *,std::string,std::string,std::string)=0;
   virtual Table * createTable(VLayout *,std::string,std::string,std::string)=0;

   virtual RadioButtonGroup * createRadioButtonGroup(HLayout *)=0;
   virtual RadioButtonGroup * createRadioButtonGroup(VLayout *)=0;
   virtual RadioButtonGroup * createRadioButtonGroup(Dialog *)=0;

   virtual  IntField * createIntField(HLayout *,std::string,int,int,int)=0;
   virtual  IntField * createIntField(VLayout *,std::string,int,int,int)=0;
   virtual  IntField * createIntField(Dialog *,std::string,int,int,int)=0;

   virtual  CheckBox * createCheckBox(HLayout *,std::string,bool)=0;
   virtual  CheckBox * createCheckBox(VLayout *,std::string,bool)=0;
   virtual  CheckBox * createCheckBox(Dialog *,std::string,bool)=0;

};

}


#endif
