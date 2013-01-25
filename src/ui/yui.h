#ifndef YUI_H_
#define YUI_H_

#include "ui.h"

#define YUILogComponent "SaX3-UI-Lib-Log"
#include "YUILog.h"
#include "YUI.h"
#include "YWidgetFactory.h"
#include "YLayoutBox.h"
#include "YDialog.h"
#include "YEvent.h"
#include "YPushButton.h"
#include "YLabel.h"
#include "YWidget.h"
#include "YAlignment.h"
#include "YImage.h"
#include "YComboBox.h"
#include "YMultiSelectionBox.h"
#include "YItem.h"
#include "YTable.h"
#include "YTableHeader.h"
#include "YRadioButtonGroup.h"
#include "YRadioButton.h"
#include "YIntField.h"
#include "YCheckBox.h"

#include<iostream>

namespace UI{

	class yDialog;
	class yVLayout;
	class yHLayout;
	class yLabel;
	class yPushButton;
	class yImage;
	class yComboBox;
	class yMultiSelectionBox;
	class yRadioButtonGroup;
	class yIntField;
	class yCheckBox;
//YUIDIALOG
	class yDialog : public Dialog{
		YDialog * dialog;
		YAlignment * alignment;
		YEvent * Event;
		enum EventType
	        {
		        NoEvent = 0,
		        UnknownEvent,
		        WidgetEvent,
		        MenuEvent,
		        KeyEvent,
		        CancelEvent,
		        TimeoutEvent,
		        DebugEvent,
		        InvalidEvent = 0x4242
		};

		public:
		yDialog(int width,int height);
		YAlignment * getElement();
		void wait();
		YWidget* eventWidget();
		int eventReason();
		void redraw();
		~yDialog();
	};
//YUIHLAYOUT
	class yHLayout : public HLayout{
		YLayoutBox * layout;
		public :
			yHLayout(yHLayout *);
			yHLayout(yVLayout *);
			yHLayout(yDialog *);
			YLayoutBox * getElement();
			~yHLayout();
	};
//YUIVLAYOUT
	class yVLayout : public VLayout{
		YLayoutBox * layout;
		public :
			yVLayout(yHLayout *);
			yVLayout(yVLayout *);
			yVLayout(yDialog *);
			YLayoutBox * getElement();
			~yVLayout();
	};
//YUILABEL
	class yLabel : public Label{
		YLabel * label;
		public:
		yLabel(yDialog *,std::string text);
		yLabel(yHLayout *,std::string text);
		yLabel(yVLayout *,std::string text);
		void setValue(std::string);
		~yLabel();
	};

//YUIPUSHBUTTON
	class yPushButton : public PushButton{
		YPushButton * button;
		public:
		yPushButton(yDialog *,std::string text);
		yPushButton(yHLayout *,std::string text);
		yPushButton(yVLayout *,std::string text);
		YPushButton * getElement();
		void setEnabled(bool);
                std::string value();
		~yPushButton();
	};

//YUIIMAGE
	class yImage : public Image{
		YImage * image;
		public:
		yImage(yDialog *,std::string path);
		yImage(yHLayout *,std::string path);
		yImage(yVLayout *,std::string path);
		~yImage();
	};

//YUICOMBOX - NON EDITABLE
	class yComboBox : public ComboBox{
		YComboBox * comboBox;
		public:
		yComboBox(yDialog*,std::string label);
		yComboBox(yHLayout *,std::string label);
		yComboBox(yVLayout *,std::string label);
		
		void addItem(std::string item);
		YComboBox * getElement();
                std::string value();
		void setValue(std::string&);
		void deleteAllItems();
		void setDisabled();
		void setEnabled();
		~yComboBox();

	};	
//YMULTISELECTIONBOX 
	class yMultiSelectionBox : public MultiSelectionBox{
		YMultiSelectionBox * multi;
		YItemCollection slist;
		public:
		yMultiSelectionBox(yDialog*,std::string label);
		yMultiSelectionBox(yHLayout*,std::string label);
		yMultiSelectionBox(yVLayout*,std::string label);
		void addItem(std::string item);
		void selectedItems(std::vector<std::string> &list);
		~yMultiSelectionBox();
	};

//YTABLE
	class yTable : public Table{
		YTable * table;
		YTableHeader * header;
		YItemCollection items;
		typedef std::pair<std::string,std::string> P;
		std::vector<P> i;
		YItemCollection createList();
		public:
		yTable(yDialog*,std::string,std::string,std::string);
		yTable(yHLayout*,std::string,std::string,std::string);
		yTable(yVLayout*,std::string,std::string,std::string);
		void addItem(std::string,std::string);
		void addItem(std::string,std::string,std::string);
		std::vector<P> getItems();
		void deleteSelected();
		YTable * getElement();
		void swap(int);
		~yTable();
	};

//YRADIOBUTTONGROUP
	class yRadioButtonGroup : public RadioButtonGroup{
		YRadioButtonGroup * group;
		YLayoutBox * layout1;
		YLayoutBox * layout2;
		std::vector<YRadioButton *> buttonList;
		
		public:
		yRadioButtonGroup(yDialog*);
		yRadioButtonGroup(yHLayout*);
		yRadioButtonGroup(yVLayout*);
                std::string selectedLabel();
		void addButton(std::string);
		bool isButton(int,YWidget*);
		void setValue(int,int);
		~yRadioButtonGroup();
	};

//YINTFIELD 
	class yIntField : public IntField{
		YIntField * field;
		public:
		yIntField(yDialog*,std::string,int,int,int);
		yIntField(yHLayout*,std::string,int,int,int);
		yIntField(yVLayout*,std::string,int,int,int);
		int value();
		void setDisabled();
		void setEnabled();
		void setValue(int);
		~yIntField();
	};

//YCHECKBOX
	class yCheckBox : public CheckBox{
		YCheckBox * cb;
		public:
		yCheckBox(yDialog*,std::string,bool);
		yCheckBox(yHLayout*,std::string,bool);
		yCheckBox(yVLayout*,std::string,bool);
		bool isChecked();
		void setChecked(bool);
		YCheckBox* getElement();
		~yCheckBox();
	};
}
#endif
