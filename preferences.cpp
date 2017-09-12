#include "preferences.h"

Preferences::Preferences(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Preferences) {
	ui->setupUi(this);

	ui->combo_unit_frequency->addItem(tr("MHz"));
	ui->combo_unit_frequency->addItem(tr("GHz"));
	ui->combo_unit_frequency->addItem(tr("cm<sup>−1</sup>"));
	ui->combo_unit_frequency->addItem(tr("nm"));

	ui->combo_unit_intensity->addItem(tr("nm² ⋅ MHz"));
	ui->combo_unit_intensity->addItem(tr("cm / molec"));

	ui->combo_unit_temperature->addItem(tr("K"));
	ui->combo_unit_temperature->addItem(tr("°C"));

	ui->combo_lineEnding->addItem(tr("Line Feed"), QString("\n"));
	ui->combo_lineEnding->addItem(tr("Carriage Return"), QString("\r"));
	ui->combo_lineEnding->addItem(tr("CR+LF"), QString("\r\n"));
	ui->combo_lineEnding->addItem(tr("LF+CR"), QString("\n\r"));

	connect(this, SIGNAL(finished(int)), SLOT(finished(int)));

	loadSettings();
}

Preferences::~Preferences() {
	saveSettings();
	delete ui;
}

void Preferences::loadSettings() {
	QSettings s(QSettings::IniFormat, QSettings::UserScope, "SavSoft", "CatSearch");
	s.beginGroup("units");
	ui->combo_unit_frequency->setCurrentIndex(s.value("frequency", 0).toInt());
	ui->combo_unit_intensity->setCurrentIndex(s.value("intensity", 0).toInt());
	ui->combo_unit_temperature->setCurrentIndex(s.value("temperature", 0).toInt());
	s.endGroup();
	s.beginGroup("start");
	ui->check_autoLoadCatalogs->setChecked(s.value("autoload", true).toBool());
	s.endGroup();
	s.beginGroup("export");
	ui->combo_lineEnding->setCurrentIndex(s.value("ending", 0).toInt());
	ui->check_quoteText->setChecked(s.value("quotation", false).toBool());
	ui->check_includeTitle->setChecked(s.value("title", true).toBool());
	ui->check_commentSign->setChecked(s.value("commentSign", false).toBool());
	s.endGroup();
}

void Preferences::saveSettings() {
	QSettings s(QSettings::IniFormat, QSettings::UserScope, "SavSoft", "CatSearch");
	s.beginGroup("units");
	s.setValue("frequency", ui->combo_unit_frequency->currentIndex());
	s.setValue("intensity", ui->combo_unit_intensity->currentIndex());
	s.setValue("temperature", ui->combo_unit_temperature->currentIndex());
	s.endGroup();
	s.beginGroup("start");
	s.setValue("autoload", ui->check_autoLoadCatalogs->isChecked());
	s.endGroup();
	s.beginGroup("export");
	s.setValue("ending", ui->combo_lineEnding->currentIndex());
	s.setValue("quotation", ui->check_quoteText->isChecked());
	s.setValue("title", ui->check_includeTitle->isChecked());
	s.setValue("commentSign", ui->check_commentSign->isChecked());
	s.endGroup();
}

void Preferences::finished(int result) {
	if (result == Preferences::Accepted) {
		saveSettings();
	}
	else {
		loadSettings();
	}
}

void Preferences::on_check_includeTitle_toggled(bool checked) {
	ui->check_commentSign->setEnabled(checked);
}
