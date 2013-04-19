#include "yui.h"

#include<iostream>
#include<stdio.h>

#define YUILogComponent "SaX3-yui"
#include <yui/YUILog.h>

namespace UI{

	//YDIALOG FUNCTIONS START
	yDialog::yDialog(int width,int height){
		dialog = YUI::widgetFactory()->createPopupDialog();
		alignment = YUI::widgetFactory()->createMinSize(dialog,width,height);
	}
	
	YAlignment * yDialog::getElement(){
		return alignment;
	}

	void yDialog::wait(){
		Event = dialog->waitForEvent();
	}
	YWidget* yDialog::eventWidget(){
		return Event->widget();
	}

	yDialog::~yDialog(){
		delete alignment;
		dialog->destroy();
	}
	void yDialog::redraw(){
		dialog->recalcLayout();
	}
	int yDialog::eventReason(){
		return Event->eventType();
	}
	//YDIALOG FUCNTIONS END


	//YVLAYOUT FUNCTIONS BEGIN
	yVLayout::yVLayout(yDialog * parent){
		layout = YUI::widgetFactory()->createVBox(parent->getElement());
	}

	yVLayout::yVLayout(yHLayout * parent){
		layout=YUI::widgetFactory()->createVBox(parent->getElement());
	}

	yVLayout::yVLayout(yVLayout * parent){
		layout = YUI::widgetFactory()->createVBox(parent->getElement());
	}

	YLayoutBox * yVLayout::getElement(){
		return layout;
	}
	yVLayout::~yVLayout(){
		delete layout;
	}


	//YVLAYOUT FUNCTIONS END

	//YHLAYOUT FUNCTIONS BEGIN
	yHLayout::yHLayout(yDialog * parent){
		layout = YUI::widgetFactory()->createHBox(parent->getElement());
	}

	yHLayout::yHLayout(yHLayout * parent){
		layout=YUI::widgetFactory()->createHBox(parent->getElement());
	}

	yHLayout::yHLayout(yVLayout * parent){
		layout = YUI::widgetFactory()->createHBox(parent->getElement());
	}

	YLayoutBox * yHLayout::getElement(){
		return layout;
	}
	yHLayout::~yHLayout(){
		delete layout;
	}
	//YHLAYOUT FUNCTIONS END

	//YLABEL FUNCTIONS START

	yLabel::yLabel(yDialog * parent,std::string text){
		label = YUI::widgetFactory()->createLabel(parent->getElement(),text);
	}

	yLabel::yLabel(yHLayout * parent,std::string text){
		label = YUI::widgetFactory()->createLabel(parent->getElement(),text);
	}

	yLabel::yLabel(yVLayout * parent,std::string text){
		label = YUI::widgetFactory()->createLabel(parent->getElement(),text);
	}
	void yLabel::setValue(std::string text){
		label->setValue(text);
	}
	yLabel::~yLabel(){
		delete label;
	}

	//YLABEL FUNCTIONS END

	//YPUSHBUTTON FUNCTIONS START
	yPushButton::yPushButton(yDialog * parent,std::string text){
		button = YUI::widgetFactory()->createPushButton(parent->getElement(),text);
	}
	yPushButton::yPushButton(yVLayout * parent,std::string text){
		button = YUI::widgetFactory()->createPushButton(parent->getElement(),text);
	}
	yPushButton::yPushButton(yHLayout * parent,std::string text){
		button = YUI::widgetFactory()->createPushButton(parent->getElement(),text);
	}
	YPushButton* yPushButton::getElement(){
		return button;
	}
        std::string yPushButton::value(){
		return button->label();
	}
	yPushButton::~yPushButton(){
		delete button;
	}
	void yPushButton::setEnabled(bool val){
		button->setEnabled(val);
	}


	//YPUSHBUTTION FUNCTIONS END

	//YIMAGE FUNCTIONS START
	yImage::yImage(yDialog * parent,std::string path){
		image = YUI::widgetFactory()->createImage(parent->getElement(),path);
	}
	yImage::yImage(yHLayout * parent,std::string path){
		image = YUI::widgetFactory()->createImage(parent->getElement(),path);
	}
	yImage::yImage(yVLayout * parent,std::string path){
		image = YUI::widgetFactory()->createImage(parent->getElement(),path);
	}
	yImage::~yImage(){
		delete image;
	}
	//YIMAGE FUNCTIONS END
	
	//YCOMBOBOX FUNCTION STARTS
	yComboBox::yComboBox(yDialog * parent,std::string text){
		comboBox = YUI::widgetFactory()->createComboBox(parent->getElement(),text,false);
		comboBox->setNotify(true);

	}
	yComboBox::yComboBox(yHLayout * parent,std::string text){
		comboBox = YUI::widgetFactory()->createComboBox(parent->getElement(),text,false);
		comboBox->setNotify(true);
	}
	yComboBox::yComboBox(yVLayout* parent,std::string text){
		comboBox = YUI::widgetFactory()->createComboBox(parent->getElement(),text,false);
		comboBox->setNotify(true);
	}
	void yComboBox::addItem(std::string item){
		comboBox->addItem(item);
	}
	YComboBox * yComboBox::getElement(){
		return comboBox;
	}
        std::string yComboBox::value(){
		return comboBox->value();
	}
	void yComboBox::setValue(std::string &value){
		comboBox->setValue(value);
	}
	void yComboBox::deleteAllItems(){
		comboBox->deleteAllItems();
	}
	yComboBox::~yComboBox(){
		delete comboBox;
	}
	void yComboBox::setDisabled(){
		comboBox->setEnabled(false);
	}
	void yComboBox::setEnabled(){
		comboBox->setEnabled(true);
	}


	//YCOMBOBOX FUNCION ENDS
	
	//YMULTISELECTIONBOX FUNCTION STARTS
	yMultiSelectionBox::yMultiSelectionBox(yDialog * parent,std::string label){
		multi = YUI::widgetFactory()->createMultiSelectionBox(parent->getElement(),label);
	}
	yMultiSelectionBox::yMultiSelectionBox(yHLayout * parent,std::string label){
		multi = YUI::widgetFactory()->createMultiSelectionBox(parent->getElement(),label);
	}
	yMultiSelectionBox::yMultiSelectionBox(yVLayout * parent,std::string label){
		multi = YUI::widgetFactory()->createMultiSelectionBox(parent->getElement(),label);
	}
	void yMultiSelectionBox::addItem(std::string item){
		multi->addItem(item);
	}
	yMultiSelectionBox::~yMultiSelectionBox(){
		delete multi;
	}
	void yMultiSelectionBox::selectedItems(std::vector<std::string> &list){
		slist = multi->selectedItems();
		for(unsigned i=0;i<slist.size();i++){
			list.push_back(slist.at(i)->label());
		}		
	}
	
	//YMULTISELECTIONBOX FUNCTION ENDS
	
	//YTABLE FUNCTION STARTS
	yTable::yTable(yDialog* parent,std::string HeaderCol1,std::string HeaderCol2,std::string HeaderCol3=""){
		header = new YTableHeader();
		YUI_CHECK_NEW(header);
		header->addColumn(HeaderCol1);
		header->addColumn(HeaderCol2);
		if(HeaderCol3!="")header->addColumn(HeaderCol3);	
		table=YUI::widgetFactory()->createTable(parent->getElement(),header);
		table->setNotify(true);
		//table->setKeepSorting(false);
	}
	yTable::yTable(yHLayout* parent,std::string HeaderCol1,std::string HeaderCol2,std::string HeaderCol3=""){
		header = new YTableHeader();
		YUI_CHECK_NEW(header);
		header->addColumn(HeaderCol1);
		header->addColumn(HeaderCol2);
		if(HeaderCol3!="")header->addColumn(HeaderCol3);	
		table=YUI::widgetFactory()->createTable(parent->getElement(),header);
		table->setNotify(true);
		//table->setKeepSorting(false);
	}
	yTable::yTable(yVLayout* parent,std::string HeaderCol1,std::string HeaderCol2,std::string HeaderCol3=""){
		header = new YTableHeader();
		YUI_CHECK_NEW(header);
		header->addColumn(HeaderCol1);
		header->addColumn(HeaderCol2);
		if(HeaderCol3!="")header->addColumn(HeaderCol3);	
		table=YUI::widgetFactory()->createTable(parent->getElement(),header);
		table->setNotify(true);
		//table->setKeepSorting(false);
	}
	void yTable::addItem(std::string item1,std::string item2){
		table->addItem(new YTableItem(item1,item2));
		i.push_back(std::make_pair<std::string,std::string> (item1,item2));	
	}
	void yTable::addItem(std::string n,std::string item1,std::string item2){
		table->addItem(new YTableItem(n,item1,item2));
		i.push_back(std::make_pair(item1,item2));
	}
	void yTable::deleteSelected(){
		YItemIterator it = table->itemsBegin();
		std::vector<P>::iterator ii = i.begin();
		while(it!=table->itemsEnd()){
			YItem * item = *it;
			if(*it==table->selectedItem())
				break;
			++ii;++it;
			
		}
		table->deleteAllItems();
		
		i.erase(ii);
		items = createList();
		table->addItems(items);
	}
	void yTable::swap(int pos){
		YItemConstIterator it=table->itemsBegin(),ic=table->itemsEnd()-1;
		std::vector<P>::iterator ii = i.begin(),ir = i.begin();
		std::pair<std::string,std::string> x;
		yuiDebug()<<"NAPANM:"<<table->itemsCount();
		yuiDebug()<<*ic<<'\t'<<table->selectedItem()<<std::endl;
		if(*ic==table->selectedItem() && pos >0) return;

		while(it!=table->itemsEnd()){
			YItem * item = *it;

			if(*it==table->selectedItem()){
				x = std::make_pair(ii->first,ii->second);
				break;
			}
			++ii;++it;++ir;
		}
	//Pos > 0 signifies move down, pos < 0 signifies up
		if(pos>0){
			if(it==table->itemsEnd()) {return;}
			while(it!=table->itemsEnd() && pos!=0){
				++ii;++it;--pos;
			}
		}
		if(pos<0){
			yuiDebug()<<"In HERE";
			if(it==table->itemsBegin()) return;
			while(it!=table->itemsEnd() && pos!=0){
				--ii;--it;++pos;
			}
		}
		ir->first = ii->first;
		ir->second = ii->second;
		ii->first = x.first;
		ii->second = x.second;
		table->deleteAllItems();
		for(ii=i.begin();ii!=i.end();ii++){
			yuiDebug()<<ii->first<<'\t'<<ii->second<<std::endl;
		}
		items = createList();
		table->addItems(items);

	}
	YItemCollection yTable::createList(){
		YItemCollection list;
		std::vector<P>::iterator ii;
		ii = i.begin();
		if(table->columns()==2){
			while(ii!=i.end()){
				yuiDebug()<<"inserting :"<<ii->first<<'\t'<<ii->second<<std::endl;
				list.push_back(new YTableItem(ii->first,ii->second));
				ii++;
			}
		}else{
			int x=0;
			char * s = new char[10];
			while(ii!=i.end()){
				sprintf(s,"%d",++x);
				list.push_back(new YTableItem(s,ii->first,ii->second));
				ii++;
			}
		}
		return list;
	}

	YTable* yTable::getElement(){
		return table;
	}
	std::vector< std::pair<std::string,std::string> > yTable::getItems(){
		return i;
	}

	yTable::~yTable(){
		delete table;
	}
	//YTABLE FUNCTION ENDS
	
	//YRADIOBUTTONGROUP FUNCTION STARTS
	yRadioButtonGroup::yRadioButtonGroup(yDialog *parent){
		layout1 = YUI::widgetFactory()->createVBox(parent->getElement());
		group = YUI::widgetFactory()->createRadioButtonGroup(layout1);
		layout2 = YUI::widgetFactory()->createHBox(group);
	}
	yRadioButtonGroup::yRadioButtonGroup(yHLayout *parent){
		layout1 = YUI::widgetFactory()->createHBox(parent->getElement());
		group = YUI::widgetFactory()->createRadioButtonGroup(layout1);
		layout2 = YUI::widgetFactory()->createHBox(group);
	}
	yRadioButtonGroup::yRadioButtonGroup(yVLayout * parent){
		layout1 = YUI::widgetFactory()->createVBox(parent->getElement());
		group = YUI::widgetFactory()->createRadioButtonGroup(layout1);
		layout2 = YUI::widgetFactory()->createVBox(group);
	}
	void yRadioButtonGroup::addButton(std::string text){
		YRadioButton * b = YUI::widgetFactory()->createRadioButton(layout2,text,true);
		b->setNotify(true);
		buttonList.push_back(b);
	}
        std::string yRadioButtonGroup::selectedLabel(){
		return group->currentButton()->label();
	}
	bool yRadioButtonGroup::isButton(int index,YWidget* w){
		if(w==buttonList[index])
			return true;
		else
			return false;
	}
	void yRadioButtonGroup::setValue(int index,int value){
		buttonList[index]->setValue(value);
	}
	yRadioButtonGroup::~yRadioButtonGroup(){
		delete layout2;
		delete layout1;
		delete group;
	}

	yIntField::yIntField(yDialog* parent,std::string text,int min,int max,int value){
		field = YUI::widgetFactory()->createIntField(parent->getElement(),text,min,max,value);
	}
	yIntField::yIntField(yHLayout* parent,std::string text,int min,int max,int value){
		field = YUI::widgetFactory()->createIntField(parent->getElement(),text,min,max,value);
	}
	yIntField::yIntField(yVLayout* parent,std::string text,int min,int max,int value){
		field = YUI::widgetFactory()->createIntField(parent->getElement(),text,min,max,value);
	}
	int yIntField::value(){
		return field->value();
	}
	void yIntField::setValue(int value){
		field->setValue(value);
	}

	void yIntField::setDisabled(){
		return field->setDisabled();
	}
	void yIntField::setEnabled(){
		return field->setEnabled();
	}
	yIntField::~yIntField(){
		delete field;
	}

	yCheckBox::yCheckBox(yDialog * parent,std::string text,bool checked){
		cb = YUI::widgetFactory()->createCheckBox(parent->getElement(),text,checked);
		cb->setNotify(true);
	}
	yCheckBox::yCheckBox(yHLayout * parent,std::string text,bool checked){
		cb = YUI::widgetFactory()->createCheckBox(parent->getElement(),text,checked);
		cb->setNotify(true);
	}
	yCheckBox::yCheckBox(yVLayout * parent,std::string text,bool checked){
		cb = YUI::widgetFactory()->createCheckBox(parent->getElement(),text,checked);
		cb->setNotify(true);
	}
	bool yCheckBox::isChecked(){
		return cb->isChecked();
	}
	void yCheckBox::setChecked(bool v=true){
		cb->setChecked(v);
	}
	yCheckBox::~yCheckBox(){
		delete cb;
	}
	YCheckBox* yCheckBox::getElement(){
		return cb;
	}
}
