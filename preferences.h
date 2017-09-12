#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "ui_preferences.h"

#include <QDialog>
#include <QSettings>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
	Q_OBJECT

	friend class MainWindow;

public:
	explicit Preferences(QWidget *parent = 0);
	~Preferences();

private slots:

	void finished(int result);

	void on_check_includeTitle_toggled(bool checked);

private:
	Ui::Preferences *ui;

	void loadSettings();

	void saveSettings();
};

#endif // PREFERENCES_H
