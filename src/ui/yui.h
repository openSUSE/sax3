#ifndef _YUI_H_
#define _YUI_H_

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

#include<iostream>

namespace UI{

	class yDialog;
	class yVLayout;
	class yHLayout;
	class yLabel;
	class yPushButton;
	class yImage;
	
//YUIDIALOG
	class yDialog : public Dialog{
		YDialog * dialog;
		YAlignment * alignment;
		public:
		yDialog(int width,int height);
		YAlignment * getElement();
		void event();
	};
//YUIHLAYOUT
	class yHLayout : public HLayout{
		YLayoutBox * layout;
		public :
			yHLayout(yHLayout *);
			yHLayout(yVLayout *);
			yHLayout(yDialog *);
			YLayoutBox * getElement();
	};
//YUIVLAYOUT
	class yVLayout : public VLayout{
		YLayoutBox * layout;
		public :
			yVLayout(yHLayout *);
			yVLayout(yVLayout *);
			yVLayout(yDialog *);
			YLayoutBox * getElement();
	};
//YUILABEL
	class yLabel : public Label{
		YLabel * label;
		public:
		yLabel(yDialog *,std::string text);
		yLabel(yHLayout *,std::string text);
		yLabel(yVLayout *,std::string text);
	};

//YUIPUSHBUTTON
	class yPushButton : public PushButton{
		YPushButton * button;
		public:
		yPushButton(yDialog *,std::string text);
		yPushButton(yHLayout *,std::string text);
		yPushButton(yVLayout *,std::string text);

	};

//YUIIMAGE
	class yImage : public Image{
		YImage * image;
		public:
		yImage(yDialog *,std::string path);
		yImage(yHLayout *,std::string path);
		yImage(yVLayout *,std::string path);

	};

}

#endif