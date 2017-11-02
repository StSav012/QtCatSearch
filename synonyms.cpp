#include "synonyms.h"

Synonyms::Synonyms(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Synonyms) {
	ui->setupUi(this);
}

Synonyms::~Synonyms() {
	delete ui;
}

void Synonyms::on_text_filter_textChanged(const QString &filter) {
	ui->list->clear();
	if (filter.isEmpty()) {
		return;					// nothing to do
	}
	bool matches = false;
	int bestMatchPosition = -1;
	QStringList bestMatchNames;
	foreach (QJsonValue catalogDataItem, MainWindow::catalogData) {
		QJsonObject catalogEntry = catalogDataItem.toObject();
		foreach (QString key, nameKeys) {
			QString name = catalogEntry.value(key).toString().trimmed();
			QStringList names;
			names.append(name);
			names.append(name.split(QString(", "), QString::SkipEmptyParts));
			foreach (QString name, names) {
				name = name.trimmed();
				if (filter.toLower() == name.toLower()) {
					foreach (QString key, nameKeys) {
						QString name = catalogEntry.value(key).toString().trimmed();
						if (!name.isEmpty() && ui->list->findItems(name, Qt::MatchExactly).isEmpty()) {
							ui->list->addItem(name);
						}
					}
					matches = true;
					break;
				}
				else {
					int pos = name.toLower().indexOf(filter.toLower(), Qt::CaseInsensitive);
					if (pos != -1) {
						if (bestMatchPosition < 0 || pos < bestMatchPosition) {
							bestMatchNames.clear();
							bestMatchPosition = pos;
						}
						if (pos == bestMatchPosition && !bestMatchNames.contains(names.first())) {
							bestMatchNames.append(names.first());
						}
					}
				}
				if (matches) {
					break;
				}
			}
		}
	}
	if (!matches) {
		ui->list->addItems(bestMatchNames);
	}
}

void Synonyms::on_buttonBox_rejected() {
	this->reject();
}

void Synonyms::on_list_doubleClicked(const QModelIndex &index) {
	ui->text_filter->setText(ui->list->item(index.row())->text());
}
