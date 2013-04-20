/**
 * \file main.cxx
 *
 * \brief Main chooser module
 *
 * Chooser module loads all modules that are available for SaX. It displays simple UI to run them.
 *
 */

#include <iostream>
#include <locale.h>
#include <libintl.h>
#include <string.h>
#include <map>
#include <unistd.h>
#include <stdlib.h>

extern "C"{
#include <augeas.h>
}

#define YUILogComponent "SaX3-launcher"
#include <yui/YUILog.h>
#include <yui/YUI.h>
#include <yui/YApplication.h>
#include <yui/YWidgetFactory.h>
#include <yui/YDialog.h>
#include <yui/YEvent.h>
#include <yui/YLayoutBox.h>
#include <yui/YPushButton.h>

#define _(STRING) gettext(STRING)

using namespace std;

/**
 * \class Chooser
 * \brief Allows you to choose which module to run
 *
 * It scans /usr/share/sax3/modules.d/ to look for desktop files containing
 * information regarding the modules available. All found modules are added to
 * the main dialog and made available to the user to start.
 *
 */

class Chooser {
protected:
	//! Main dialog that shows modules
	YDialog *dialog;
	//! Mapping between widgets IDs and execute actions
	map<YWidgetID *,string> entities;
public:
	//! Constructor
	Chooser() {
		dialog = NULL;
	}
	//! Loads list of modules and prepares UI
	bool load() {
		bool ret = true;
		int i = 0;

		// UI

		// Something to help us create a widgets
		YWidgetFactory *factory;
		factory = YUI::widgetFactory();

		// Prepare basic UI - stretchable and centered
		dialog = factory->createPopupDialog();
		YLayoutBox *horizontal = factory->createHBox(dialog);
		factory->createHStretch(horizontal);
		factory->createHSpacing(horizontal);
		YLayoutBox *vertical = factory->createVBox(horizontal);
		factory->createHSpacing(horizontal);
		factory->createHStretch(horizontal);

		// Augeas
		augeas * aug = NULL;
		aug = aug_init(NULL, NULL, AUG_NO_LOAD);
		if(aug == NULL) {
			yuiError() << _("Augeas init failed!") << endl;
			return false;
		}
		// Load desktop lense
		if(aug_set(aug,"/augeas/load/Desktop/lens", "Desktop.lns") != 0) {
			yuiError() << _("Cannot load desktop lense!") << endl;
			yuiError() << aug_error_message(aug) << endl;
			ret = false;
			goto aug_cleanup;
		}
		// Set path to our modules
		if(aug_set(aug,"/augeas/load/Desktop/incl[last()+1]",
			            "/usr/share/sax3/modules.d/*.desktop") != 0 ) {
			yuiError() << _("Cannot set path to our desktop files!") << endl;
			yuiError() << aug_error_message(aug) << endl;
			ret = false;
			goto aug_cleanup;
		}
		// Load them
		if(aug_load(aug) != 0) {
			yuiError() << _("Cannot reload augeas!") << endl;
			yuiError() << aug_error_message(aug) << endl;
			ret = false;
			goto aug_cleanup;
		}
		// Get all available modules
		int entries;
		char **matches;
		entries = aug_match(aug,"/files/usr/share/sax3/modules.d/*/DesktopEntry/Comment",&matches);
		yuiDebug() << string("Number of modules I have found: ") << entries << endl;
		yuiDebug() << string("Modules I have found: ") << endl;
		if(YUILog::debugLoggingEnabled()) {
			for(i = 0; i<entries;i++) {
				yuiDebug() << matches[i];
				yuiDebug() << endl;
			}
		}

		// Prepare everything
		char* start;
		const char* name;
		const char* help;
		const char* exec;
		const char* icon;
		YIconButton* button;
		factory->createVStretch(vertical);

		// Go through all the modules and load them
      for(i = 0; i < entries; i++) {
			start = strrchr(matches[i],'/');
			start++;
			factory->createVSpacing(vertical, 0.25);

			// Load image
			strncpy(start,"Icon",5);
			yuiDebug() << "Searching for '" << string(matches[i]) << "'" << endl;
			if(aug_get(aug,matches[i],&(icon)) < 1) {
				yuiError() << _("Cannot get icon path!") << endl;
				yuiError() << aug_error_message(aug) << endl;
				ret = false;
				goto matches_cleanup;
			}

			// Load name
			strncpy(start,"Name",5);
			yuiDebug() << "Searching for '" << string(matches[i]) << "'" << endl;
			if(aug_get(aug,matches[i],&(name)) < 1) {
				yuiError() << _("Cannot get name!") << endl;
				yuiError() << aug_error_message(aug) << endl;
				ret = false;
				goto matches_cleanup;
			}

			// Load exec
			strncpy(start,"Exec",5);
			yuiDebug() << "Searching for '" << string(matches[i]) << "'" << endl;
			if(aug_get(aug,matches[i],&(exec)) < 1) {
				yuiError() << _("Cannot get name!") << endl;
				yuiError() << aug_error_message(aug) << endl;
				ret = false;
				goto matches_cleanup;
			}

			// Load help string
			strncpy(start,"Comment",5);
			yuiDebug() << "Searching for '" << string(matches[i]) << "'" << endl;
			if(aug_get(aug,matches[i],&(help)) < 1) {
				yuiError() << _("Cannot get name!") << endl;
				yuiError() << aug_error_message(aug) << endl;
				ret = false;
				goto matches_cleanup;
			}

			// Little debugging info
			yuiDebug() << "Got element '"       << name << "'" << endl;
			yuiDebug() << "Icon is '"           << icon << "'" << endl;
			yuiDebug() << "And executable is '" << exec << "'" << endl;

			// Create button and register appropriate action
			button = factory->createIconButton(vertical, icon, _(name));
			button->setHelpText(_(help));
			entities.insert(make_pair(button->id(), exec));

		}

		// Add spacing and Quit button
		factory->createVSpacing(vertical);
		factory->createPushButton(vertical, _("Quit"))->setRole(YCancelButton);
		factory->createVSpacing(vertical, 0.25);
		factory->createVStretch(vertical);
		dialog->recalcLayout();

		// Deallocate found modules
matches_cleanup:
		for(i = 0; i<entries;i++) {
			free(matches[i]);
		}
		free(matches);

		// Close Augeas
aug_cleanup:
		aug_close(aug);
		return ret;
	}


	//! Runs event processing loop and calls whatever is needed
	void exec() {
		YEvent *event = NULL;
		dialog->open();
		map<YWidgetID * ,string>::iterator it;

		// Wait for event
		while(event = dialog->waitForEvent()) {
			yuiDebug() << "Experienced event type " << YEvent::toString(event->eventType()) << endl;

			// Closed window
			if(event->eventType() == YEvent::CancelEvent) {
				yuiDebug() << "Dialog canceled" << endl;
				break;
			}

			// Quit pressed
			if(((YPushButton*)(event->widget()))->role() == YCancelButton) {
				yuiDebug() << "User quit" << endl;
				break;
			}

			// Module selected
			if((it=entities.find(event->widget()->id())) != entities.end()) {
				dialog->setDisabled();
				system(it->second.c_str());
				dialog->setEnabled();
			}
		}

		// Close dialog
		dialog->destroy();
	}
};

// Main loop
int main(){

	// Set up locales
	setlocale(LC_ALL,"");
	bindtextdomain("sax3","/usr/share/locale");
	textdomain("sax3");
	YUI::app()->setApplicationTitle("SaX 3");

	// Enable debugging
	YUILog::enableDebugLogging();

	// Run it
	Chooser ui;
	if(ui.load())
		ui.exec();

	return 0;
}
