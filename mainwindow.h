#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtMath>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QDesktopWidget>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QFile>
#include <QFileDialog>
#include <QList>
#include <QString>
#include <QStringList>
#include <QClipboard>
#include <QDebug>

#include "catalogentry.h"

namespace Ui {
class MainWindow;
class Preferences;
class Synonyms;
}

class MainWindow;

#include "preferences.h"
#include "synonyms.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	static QJsonArray catalogData;

	static Preferences *prefs;

	static Synonyms *syn;

private slots:
	void on_spin_frequency_from_editingFinished();

	void on_spin_frequency_to_editingFinished();

	void on_spin_frequency_center_editingFinished();

	void on_spin_frequency_deviation_editingFinished();

	void on_text_substance_textChanged(const QString &arg1);

	void on_box_substance_toggled(bool arg1);

	void on_button_search_clicked();

	void on_spin_temperature_valueChanged(double arg1);

	void on_button_selectnone_clicked();

	void on_menu_load_triggered();

	void on_menu_reload_triggered();

	void on_menu_save_triggered();

	void on_menu_preferences_triggered();

	void on_menu_quit_triggered();

	void on_menu_clear_triggered();

	void on_menu_copy_name_triggered();

	void on_menu_copy_frequency_triggered();

	void on_menu_copy_intensity_triggered();

	void on_menu_copy_triggered();

	void on_menu_selectAll_triggered();

	void on_menu_searchNames_triggered();

	void on_menu_about_triggered();

	void on_menu_aboutqt_triggered();

	void on_spin_intensity_valueChanged(double arg1);

	void on_button_switch_1_clicked();

	void on_button_switch_2_clicked();

	void on_table_customContextMenuRequested(const QPoint &pos);

	void on_table_itemSelectionChanged();

private:
	Ui::MainWindow *ui;

	QStringList nameKeys = QStringList()
//						   << QString("moleculesymbol")
						   << QString("name")
						   << QString("trivialname")
						   << QString("structuralformula")
						   << QString("stoichiometricformula");

	QStringList catalogFileNames;

	QList<CatalogEntry> searchResults;

	int csvFileSeparator;

	QStringList selectedSubstances;

	// internally used values
	double temperature;								// K
	double minimalIntensity;						// nm²⋅MHz
	double frequencyFrom, frequencyTo;				// MHz
	double frequencyCenter, frequencyDeviation;		// MHz

	enum JsonType {
		Text,
		Binary,
		Zipped,
		Auto
	};

	void loadSettings();

	void saveSettings();

	void presetTable();

	void fillTable();

	QJsonArray loadCatalogFile(const QString &fileName, JsonType type = JsonType::Auto);

	bool loadCatalogs(const QStringList &fileNames);

	void filterSubstancesList(const QString &filter);

	void fillParameters();

	void copySelectedItems(int col);
};

#endif // MAINWINDOW_H
