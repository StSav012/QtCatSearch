#ifndef SYNONYMS_H
#define SYNONYMS_H

#include "ui_synonyms.h"

#include <QDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QClipboard>
#include <QMenu>
#include <QAction>
#include <QDebug>

namespace Ui {
class Synonyms;
}

class Synonyms;

#include "mainwindow.h"

class Synonyms : public QDialog
{
	Q_OBJECT

	friend class MainWindow;

public:
	explicit Synonyms(QWidget *parent = 0);
	~Synonyms();

private slots:
	void on_text_filter_textChanged(const QString &filter);

	void on_buttonBox_rejected();

private:
	Ui::Synonyms *ui;

	QStringList nameKeys = QStringList()
//						   << QString("moleculesymbol")
						   << QString("name")
						   << QString("trivialname")
						   << QString("structuralformula")
						   << QString("stoichiometricformula");
};

#endif // SYNONYMS_H
