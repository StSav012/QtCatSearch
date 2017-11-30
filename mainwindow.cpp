#include "mainwindow.h"
#include "ui_mainwindow.h"

QJsonArray MainWindow::catalogData;
Preferences* MainWindow::prefs;
Synonyms* MainWindow::syn;
//QStringList MainWindow nameKeys;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow) {
	ui->setupUi(this);

	prefs = new Preferences(this);
	syn = new Synonyms(this);

	presetTable();

	loadSettings();

	bool ok = false;
	if (prefs->ui->check_autoLoadCatalogs->isChecked()) {
		ok = loadCatalogs(catalogFileNames);
		if (ok) {
			ui->statusBar->showMessage(tr("Catalogs loaded."));
		}
		else {
			qWarning() << "failed to load catalogs:" << catalogFileNames;
			ui->statusBar->showMessage(tr("Failed to load a catalog."));
		}
	}
	else {
		ui->statusBar->showMessage(tr("Nothing loaded yet. Load catalogs manually."));
	}
	ui->menu_searchNames->setEnabled(ok);
	ui->button_search->setEnabled(ok);
}

MainWindow::~MainWindow() {
	saveSettings();
	delete syn;
	delete prefs;
	delete ui;
}

void MainWindow::on_spin_frequency_from_editingFinished() {
	double f = ui->spin_frequency_from->value();
	switch (prefs->ui->combo_unit_frequency->currentIndex()) {
		case 0:		// MHz
			frequencyFrom = f;
			break;
		case 1:		// GHz
			frequencyFrom = GHz2MHz(f);
			break;
		case 2:		// 1/cm
			frequencyFrom = revCm2MHz(f);
			break;
		case 3:		// nm
			frequencyFrom = nm2MHz(f);
			break;
		default:
			qWarning("wrong frequency unit index");
	}
}

void MainWindow::on_spin_frequency_to_editingFinished() {
	double f = ui->spin_frequency_to->value();
	switch (prefs->ui->combo_unit_frequency->currentIndex()) {
		case 0:		// MHz
			frequencyTo = f;
			break;
		case 1:		// GHz
			frequencyTo = GHz2MHz(f);
			break;
		case 2:		// 1/cm
			frequencyTo = revCm2MHz(f);
			break;
		case 3:		// nm
			frequencyTo = nm2MHz(f);
			break;
		default:
			qWarning("wrong frequency unit index");
	}
}

void MainWindow::on_spin_frequency_center_editingFinished() {
	double f = ui->spin_frequency_center->value();
	switch (prefs->ui->combo_unit_frequency->currentIndex()) {
		case 0:		// MHz
			frequencyCenter = f;
			break;
		case 1:		// GHz
			frequencyCenter = GHz2MHz(f);
			break;
		case 2:		// 1/cm
			frequencyCenter = revCm2MHz(f);
			break;
		case 3:		// nm
			frequencyCenter = nm2MHz(f);
			break;
		default:
			qWarning("wrong frequency unit index");
	}
}

void MainWindow::on_spin_frequency_deviation_editingFinished() {
	double f = ui->spin_frequency_deviation->value();
	switch (prefs->ui->combo_unit_frequency->currentIndex()) {
		case 0:		// MHz
			frequencyDeviation = f;
			break;
		case 1:		// GHz
			frequencyDeviation = GHz2MHz(f);
			break;
		case 2:		// 1/cm
			frequencyDeviation = revCm2MHz(f);
			break;
		case 3:		// nm
			frequencyDeviation = (nm2MHz(f + frequencyCenter) -
								  nm2MHz(f - frequencyCenter)) / 2.0;
			break;
		default:
			qWarning("wrong frequency unit index");
	}
}

void MainWindow::on_text_substance_textChanged(const QString &arg1) {
	filterSubstancesList(arg1);
}

void MainWindow::on_box_substance_toggled(bool arg1) {
	foreach (QObject *child, ui->box_substance->children()) {
		child->setProperty("Enabled", arg1);
	}
}

void MainWindow::on_spin_temperature_valueChanged(double arg1) {
	switch (prefs->ui->combo_unit_temperature->currentIndex()) {
		case 0:		// K
			temperature = arg1;
			break;
		case 1:		// °C
			temperature = arg1 + 273.15;
			break;
		default:
			qWarning("wrong temperature unit index");
	}

	fillTable();
}

void MainWindow::on_spin_intensity_valueChanged(double arg1) {
	switch (prefs->ui->combo_unit_intensity->currentIndex()) {
		case 0:		// nm²⋅MHz
			minimalIntensity = arg1;
			break;
		case 1:		// cm / molecule
			minimalIntensity = cmPerMolecule2sqNmMHz(arg1);
			break;
		default:
			qWarning("wrong intensity unit index");
	}

	fillTable();
}

void MainWindow::on_button_selectnone_clicked() {
	for (int i = 0; i < ui->list_substance->count(); ++i) {
		QListWidgetItem *item = ui->list_substance->item(i);
		item->setCheckState(Qt::Unchecked);
	}
	selectedSubstances.clear();
}

void MainWindow::on_button_switch_1_clicked() {
	ui->box_frequency->setCurrentIndex(1);
}

void MainWindow::on_button_switch_2_clicked() {
	ui->box_frequency->setCurrentIndex(0);
}

void MainWindow::on_table_customContextMenuRequested(const QPoint &pos) {
	ui->menuEdit->popup(ui->table->viewport()->mapToGlobal(pos));
}

void MainWindow::on_table_itemSelectionChanged() {
	ui->menu_copy->setDisabled(ui->table->selectedItems().isEmpty());
}

void MainWindow::on_menu_load_triggered() {
	ui->statusBar->showMessage(tr("Select a catalog file to load."));
	QStringList newCatalogFileNames = QFileDialog::getOpenFileNames(this, tr("Load Catalog"),
																	catalogFileNames.first(),
																	QString("%1 (*.qbjsz);;%2 (*.qbjs);;%3 (*.json)")
																	.arg(tr("Compressed Binary Qt JSON"),
																		 tr("Binary Qt JSON"),
																		 tr("JSON")));
	if(!newCatalogFileNames.isEmpty()) {
		ui->statusBar->showMessage(tr("Loading..."));
		catalogFileNames = newCatalogFileNames;
		if (loadCatalogs(catalogFileNames)) {
			ui->statusBar->showMessage(tr("Catalogs loaded."));
		}
		else {
			ui->statusBar->showMessage(tr("Failed to load a catalog."));
		}
	}
	else {
		ui->statusBar->clearMessage();
	}
}

void MainWindow::on_menu_reload_triggered() {
	if(!catalogFileNames.isEmpty()) {
		ui->statusBar->showMessage(tr("Loading..."));
		if (loadCatalogs(catalogFileNames)) {
			ui->statusBar->showMessage(tr("Catalogs loaded."));
		}
		else {
			ui->statusBar->showMessage(tr("Failed to load a catalog."));
		}
	}
	else {
		ui->statusBar->clearMessage();
	}
}

void MainWindow::on_menu_save_triggered() {
	static QString saveToFileName;
	QFileDialog saveDialog(this);
	saveDialog.setFileMode(QFileDialog::AnyFile);
	saveDialog.setAcceptMode(QFileDialog::AcceptSave);
//	(this, tr("Save Search Result"),
//						   saveToFileName, tr("Comma Separated Values (*.csv);;"
//											  "Tab Seperated Values (*.tsv)"));
	saveDialog.setFilter(saveDialog.filter() | QDir::Readable);
	saveDialog.setDefaultSuffix(QString("csv"));
	QStringList filters = QStringList() << tr("Comma Separated Values") + QString(" (*.csv)")
										<< tr("Tab Separated Values") + QString(" (*.csv)")
										<< tr("Semicolon Separated Values") + QString(" (*.csv)")
										<< tr("Space Separated Values") + QString(" (*.csv)");
	QList<QChar> seps = { QChar(','), QChar('\t'), QChar(';'), QChar(' ') };
	saveDialog.setNameFilters(filters);
	saveDialog.selectNameFilter(filters.at(csvFileSeparator));
	saveDialog.setDirectory(saveToFileName);
	if (saveDialog.exec()) {
		saveToFileName = saveDialog.selectedFiles().first();
		csvFileSeparator = filters.indexOf(saveDialog.selectedNameFilter());
		QChar sep = seps.at(csvFileSeparator);
		QFile f(saveToFileName);
		if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QString lineEnding = prefs->ui->combo_lineEnding->currentData().toString();
			QTextStream fout(&f);
			QString line;
			QString lineTemplate;
			if (prefs->ui->check_includeTitle->isChecked()) {
				if (prefs->ui->check_commentSign->isChecked()) {
					fout << QChar('#');
				}
				if (prefs->ui->check_quoteText->isChecked()) {
					lineTemplate = QString("\"%2\"%1\"%3\"%1\"%4\"");
				}
				else {
					lineTemplate = QString("%2%1%3%1%4");
				}
				line = lineTemplate.arg(sep);
				for (int c = 0; c < ui->table->columnCount(); ++c) {				// header
					line = line.arg(ui->table->horizontalHeaderItem(c)->text());
				}
				fout << line << lineEnding;
				fout.flush();
			}
			if (prefs->ui->check_quoteText->isChecked()) {
				lineTemplate = QString("\"%2\"%1%3%1%4");
			}
			else {
				lineTemplate = QString("%2%1%3%1%4");
			}
			for (int r = 0; r < ui->table->rowCount(); ++r) {
				line = lineTemplate.arg(sep);
				for (int c = 0; c < ui->table->columnCount(); ++c) {				// data lines
					line = line.arg(ui->table->item(r, c)->text());
				}
				fout << line << lineEnding;
				fout.flush();
			}
		}
	}
}

void MainWindow::on_menu_preferences_triggered() {
	if (prefs->exec() == Preferences::Accepted) {
		fillParameters();
		fillTable();
	}
}

void MainWindow::on_menu_quit_triggered() {
	QApplication::exit();
}

void MainWindow::on_menu_clear_triggered() {
	presetTable();
}

void MainWindow::copySelectedItems(int col) {
	if (col >= ui->table->columnCount()) {
		return;
	}
	QString lineEnding = prefs->ui->combo_lineEnding->currentData().toString();
	QStringList textToCopy;
	foreach (QTableWidgetSelectionRange selection, ui->table->selectedRanges()) {
		for (int row = selection.topRow(); row <= selection.bottomRow(); ++row) {
			textToCopy.append(ui->table->item(row, col)->text());
		}
	}
	QGuiApplication::clipboard()->setText(textToCopy.join(lineEnding));
}

void MainWindow::on_menu_copy_name_triggered() {
	copySelectedItems(0);
}

void MainWindow::on_menu_copy_frequency_triggered() {
	copySelectedItems(1);
}

void MainWindow::on_menu_copy_intensity_triggered() {
	copySelectedItems(2);
}

void MainWindow::on_menu_copy_triggered() {
	QString lineEnding = prefs->ui->combo_lineEnding->currentData().toString();
	QString line;
	QString lineTemplate;
	QString textToCopy;
	QList<QChar> seps = { QChar(','), QChar('\t'), QChar(';'), QChar(' ') };
	QChar sep = seps.at(csvFileSeparator);
	if (prefs->ui->check_includeTitle->isChecked()) {
		if (prefs->ui->check_commentSign->isChecked()) {
			textToCopy.append(QChar('#'));
		}
		if (prefs->ui->check_quoteText->isChecked()) {
			lineTemplate = QString("\"%2\"%1\"%3\"%1\"%4\"");
		}
		else {
			lineTemplate = QString("%2%1%3%1%4");
		}
		line = lineTemplate.arg(sep);
		for (int c = 0; c < ui->table->columnCount(); ++c) {				// header
			line = line.arg(ui->table->horizontalHeaderItem(c)->text());
		}
		textToCopy.append(line);
		textToCopy.append(lineEnding);
	}
	if (prefs->ui->check_quoteText->isChecked()) {
		lineTemplate = QString("\"%2\"%1%3%1%4");
	}
	else {
		lineTemplate = QString("%2%1%3%1%4");
	}
	foreach (QTableWidgetSelectionRange selection, ui->table->selectedRanges()) {
		for (int r = selection.topRow(); r <= selection.bottomRow(); ++r) {
			line = lineTemplate.arg(sep);
			for (int c = 0; c < ui->table->columnCount(); ++c) {				// data lines
				line = line.arg(ui->table->item(r, c)->text());
			}
			textToCopy.append(line);
			textToCopy.append(lineEnding);
		}
	}
	QGuiApplication::clipboard()->setText(textToCopy);
}

void MainWindow::on_menu_selectAll_triggered() {
	ui->table->selectAll();
}

void MainWindow::on_menu_searchNames_triggered() {
	if (!ui->table->selectedRanges().isEmpty()) {
		syn->ui->text_filter->setText(ui->table->item(ui->table->selectedRanges().first().topRow(), 0)->text());
	}
	else {
		syn->ui->text_filter->setText(ui->text_substance->text());
	}
	syn->exec();
}

void MainWindow::on_menu_about_triggered() {
	QMessageBox::about(this, tr("About CatSearch"),
					   QString("<html><p>")
					   + tr("CatSearch is a means of searching through spectroscopy lines catalogs. "
							"It's an offline application.")
					   + QString("</p><p>")
					   + tr("It relies on the data stored in JSON files. Qt has its own JSON format that is preferable "
							"due to its comression and consequently faster reading.")
					   + QString("</p><p>")
					   + tr("One can write his own catalogs as well as convert data from "
							"<a href='https://spec.jpl.nasa.gov/'>JPL</a> and "
							"<a href='http://www.astro.uni-koeln.de/'>CDMS</a> spectroscopy databases "
							"freely available in the Internet or other sources. The only tricky part is to convert "
							"a database into the supported format:")
					   + QString("</p><table><tr><td><tt>catalog</tt></td><td>%1</td></tr>")
					   //: “catalog” item description
					   .arg(tr("is a JSON array filled with specroscopy lines lists for various substances. "
							   "For each substance, there may be some or all of the following JSON values."))
					   + QString("<tr><td><tt>name</tt>,</td><td rowspan=4>%2</td></tr>"
								 "<tr><td><tt>stoichiometricformula</tt>,</td></tr>"
								 "<tr><td><tt>structuralformula</tt>,</td></tr>"
								 "<tr><td>%1 <tt>trivialname</tt></td></tr>")
					   //: “name”, etc. items description; the list is finished with “and”
					   .arg(tr("and"), tr("are better be plain text names or formulae describing a substance. "
										 "Spaces are allowed."))
					   + QString("<tr><td><tt>degreesoffreedom</tt></td><td>%1</td></tr>")
					   //: “degreesoffreedom” item description; do NOT translate what is between <tt> and </tt>
					   .arg(tr("is 0 for atoms, 2 for linear molecules, and 3 for nonlinear molecules."))
					   + QString("<tr><td><tt>lines</tt></td><td>%1</td></tr></table>")
					   //: “lines” item description; do NOT translate what is between <tt> and </tt>
					   .arg(tr("is a JSON array filled with specroscopy lines for the current substance. "
							   "Each line should be described by its <tt>frequency</tt> in MHz, "
							   "<tt>intensity</tt> that is a base 10 logarithm of the integrated intensity "
							   "in units of <nobr>nm²⋅MHz</nobr> at <nobr>300 K</nobr>, and "
							   "the <tt>lowerstateenergy</tt> in <nobr>cm<sup>−1</sup></nobr> "
							   "relative to the ground state."))
					   + QString("<p>")
					   + tr("Other JSON values are currently ignored.")
					   + QString("</p><p>")
					   + tr("Both plain text JSON and binary JSON currently used only by Qt are supported.")
					   + QString("</p><br><p>")
					   + tr("CatSearch is licensed under the %1.")
					   .arg(QString("<a href='https://www.gnu.org/copyleft/lesser.html'>%1</a>")
							.arg(tr("GNU LGPL version 3")))
					   + QString("</p><p>")
					   + tr("The source code is available on %1.").arg(
					   QString("<a href='https://github.com/StSav012/QtCatSearch'>%1</a>").arg(tr("GitHub")))
					   + QString("</p></html>")
					   );
}

void MainWindow::on_menu_aboutqt_triggered() {
	QMessageBox::aboutQt(this);
}

void MainWindow::loadSettings() {
	QSettings s(QSettings::IniFormat, QSettings::UserScope, "SavSoft", "CatSearch");
	s.beginGroup("search");
	catalogFileNames.clear();
	for (int i = s.beginReadArray("catalogfiles"); i; --i) {
		s.setArrayIndex(i-1);
		QString path = s.value("path").toString();
		if (!path.isEmpty()) {
			catalogFileNames.append(path);
		}
	}
	s.endArray();
	if (catalogFileNames.isEmpty()) {			// last resort
		catalogFileNames.push_back(QString("catalog.qbjsz"));
		catalogFileNames.push_back(QString("catalog.qbjs"));
		catalogFileNames.push_back(QString("catalog.json"));
	}
	s.beginGroup("selection");
	ui->text_substance->setText(s.value("filter", ui->text_substance->text()).toString());
	ui->check_saveselection->setChecked(s.value("ispersistent").toBool());
	ui->box_substance->setChecked(s.value("enabled", ui->box_substance->isChecked()).toBool());
	s.endGroup();
	temperature = s.value("temperature", ui->spin_temperature->value()).toDouble();
	minimalIntensity = s.value("intensity", ui->spin_intensity->value()).toDouble();
	s.beginGroup("frequency");
	frequencyFrom = s.value("from", ui->spin_frequency_from->value()).toDouble();
	frequencyTo = s.value("to", ui->spin_frequency_to->value()).toDouble();
	frequencyCenter = s.value("center", ui->spin_frequency_center->value()).toDouble();
	frequencyDeviation = s.value("deviation", ui->spin_frequency_deviation->value()).toDouble();
	s.endGroup();
	s.endGroup();
	s.beginGroup("export");
	csvFileSeparator = s.value("csvSeparator", 1).toInt();
	s.endGroup();
	s.beginGroup("window");
	QDesktopWidget* desktop = QApplication::desktop();
	this->move(0.5*(desktop->width()-this->size().width()),
			   0.5*(desktop->height()-this->size().height()));		// Fallback: Center the window
	QVariant window;
	window = s.value("geometry");
	if (!window.isNull()) {
		restoreGeometry(window.toByteArray());
	}
	window = s.value("state");
	if (!window.isNull()) {
		restoreState(window.toByteArray());
	}
	s.endGroup();
	fillParameters();
}

void MainWindow::saveSettings() {
	QSettings s(QSettings::IniFormat, QSettings::UserScope, "SavSoft", "CatSearch");
	s.beginGroup("search");
	s.beginWriteArray("catalogfiles", catalogFileNames.size());
	for (int i = 0; i < catalogFileNames.size(); ++i) {
		s.setArrayIndex(i);
		s.setValue("path", catalogFileNames.at(i));
	}
	s.endArray();
	s.beginGroup("selection");
	s.setValue("filter", ui->text_substance->text());
	s.setValue("ispersistent", ui->check_saveselection->isChecked());
	s.setValue("enabled", ui->box_substance->isChecked());
	s.endGroup();
	s.setValue("temperature", temperature);
	s.setValue("intensity", minimalIntensity);
	s.beginGroup("frequency");
	s.setValue("from", frequencyFrom);
	s.setValue("to", frequencyTo);
	s.setValue("center", frequencyCenter);
	s.setValue("deviation", frequencyDeviation);
	s.endGroup();
	s.endGroup();
	s.beginGroup("export");
	s.setValue("csvSeparator", csvFileSeparator);
	s.endGroup();
	s.beginGroup("window");
	s.setValue("geometry", saveGeometry());
	s.setValue("state", saveState());
	s.endGroup();
}

void MainWindow::presetTable() {
	ui->table->clearContents();
	ui->menu_save->setDisabled(true);
	ui->table->clearSelection();
	ui->menu_copy->setDisabled(true);
	ui->table->setRowCount(0);
	ui->table->setHorizontalHeaderLabels(QStringList()
										 << tr("Substance")
										 << QString("%1 [%2]").arg(tr("Frequency"),
																   prefs->ui->combo_unit_frequency->currentText())
										 << QString("%1 [%2]").arg(tr("Intensity"),
																   prefs->ui->combo_unit_intensity->currentText()));
	ui->table->resizeColumnsToContents();
	ui->table->horizontalHeaderItem(2)->setStatusTip(tr("The values are a base 10 logarithm of the integrated intensity."));
}

void MainWindow::fillParameters() {
	// frequency
	ui->spin_frequency_from->setSuffix(QString(" ") + prefs->ui->combo_unit_frequency->currentText());
	ui->spin_frequency_to->setSuffix(ui->spin_frequency_from->suffix());
	ui->spin_frequency_center->setSuffix(ui->spin_frequency_from->suffix());
	ui->spin_frequency_deviation->setSuffix(ui->spin_frequency_from->suffix());
	switch (prefs->ui->combo_unit_frequency->currentIndex()) {
		case 0:		// MHz
			ui->spin_frequency_from->setValue(frequencyFrom);
			ui->spin_frequency_to->setValue(frequencyTo);
			ui->spin_frequency_center->setValue(frequencyCenter);
			ui->spin_frequency_deviation->setValue(frequencyDeviation);
			ui->spin_frequency_from->setDecimals(4);
			ui->spin_frequency_to->setDecimals(4);
			ui->spin_frequency_center->setDecimals(4);
			ui->spin_frequency_deviation->setDecimals(4);
			break;
		case 1:		// GHz
			ui->spin_frequency_from->setValue(MHz2GHz(frequencyFrom));
			ui->spin_frequency_to->setValue(MHz2GHz(frequencyTo));
			ui->spin_frequency_center->setValue(MHz2GHz(frequencyCenter));
			ui->spin_frequency_deviation->setValue(MHz2GHz(frequencyDeviation));
			ui->spin_frequency_from->setDecimals(7);
			ui->spin_frequency_to->setDecimals(7);
			ui->spin_frequency_center->setDecimals(7);
			ui->spin_frequency_deviation->setDecimals(7);
			break;
		case 2:		// 1/cm
			ui->spin_frequency_from->setValue(MHz2revCm(frequencyFrom));
			ui->spin_frequency_to->setValue(MHz2revCm(frequencyTo));
			ui->spin_frequency_center->setValue(MHz2revCm(frequencyCenter));
			ui->spin_frequency_deviation->setValue(MHz2revCm(frequencyDeviation));
			ui->spin_frequency_from->setDecimals(8);
			ui->spin_frequency_to->setDecimals(8);
			ui->spin_frequency_center->setDecimals(8);
			ui->spin_frequency_deviation->setDecimals(8);
			break;
		case 3:		// nm
			ui->spin_frequency_from->setValue(MHz2nm(frequencyFrom));
			ui->spin_frequency_to->setValue(MHz2nm(frequencyTo));
			ui->spin_frequency_center->setValue(MHz2nm(frequencyCenter));
			ui->spin_frequency_deviation->setValue(qAbs(MHz2nm(frequencyCenter-frequencyDeviation) -
													MHz2nm(frequencyCenter+frequencyDeviation)) / 2.0);
			ui->spin_frequency_from->setDecimals(8);
			ui->spin_frequency_to->setDecimals(8);
			ui->spin_frequency_center->setDecimals(8);
			ui->spin_frequency_deviation->setDecimals(8);
			break;
		default:
			qWarning("wrong frequency unit index");
	}
	ui->spin_frequency_from->setSingleStep(2.5 * ui->spin_frequency_deviation->value());
	ui->spin_frequency_to->setSingleStep(2.5 * ui->spin_frequency_deviation->value());
	ui->spin_frequency_center->setSingleStep(2.5 * ui->spin_frequency_deviation->value());
	ui->spin_frequency_deviation->setSingleStep(0.25 * ui->spin_frequency_deviation->value());
	// intensity
	ui->spin_intensity->setSuffix(QString(" ") + prefs->ui->combo_unit_intensity->currentText());
	switch (prefs->ui->combo_unit_intensity->currentIndex()) {
		case 0:		// nm²⋅MHz
			ui->spin_intensity->setValue(minimalIntensity);
			break;
		case 1:		// cm / molecule
			ui->spin_intensity->setValue(sqNmMHz2cmPerMolecule(minimalIntensity));
			break;
		default:
			qWarning("wrong intensity unit index");
	}
	// temperature
	ui->spin_temperature->setSuffix(QString(" ") + prefs->ui->combo_unit_temperature->currentText());
	switch (prefs->ui->combo_unit_temperature->currentIndex()) {
		case 0:		// K
			ui->spin_temperature->setValue(temperature);
			ui->spin_temperature->setMinimum(0.0);
			break;
		case 1:		// °C
			ui->spin_temperature->setMinimum(-273.15);
			ui->spin_temperature->setValue(temperature - 273.15);
			break;
		default:
			qWarning("wrong temperature unit index");
	}
}

void MainWindow::fillTable() {
	presetTable();
	ui->table->setSortingEnabled(false);
	for (int i = 0; i < searchResults.size(); ++i) {
		double intensity = searchResults.at(i).intensity(temperature);
		if (intensity >= minimalIntensity) {
			double frequency;
			int _ = ui->table->rowCount();
			ui->table->setRowCount(_+1);
			ui->table->setItem(_, 0,
							   new QTableWidgetItem(searchResults.at(i).name()));

			switch (prefs->ui->combo_unit_frequency->currentIndex()) {
				case 0:		// MHz
					frequency = searchResults.at(i).frequencyMHz();
					break;
				case 1:		// GHz
					frequency = searchResults.at(i).frequencyGHz();
					break;
				case 2:		// 1/cm
					frequency = searchResults.at(i).frequencyRevCm();
					break;
				case 3:		// nm
					frequency = searchResults.at(i).frequencyNm();
					break;
				default:
					frequency = searchResults.at(i).frequency();
					qWarning("wrong frequency unit index");
			}
			ui->table->setItem(_, 1,
							   new QTableWidgetItem(QString::number(frequency, 'g', 10)));

			switch (prefs->ui->combo_unit_intensity->currentIndex()) {
				case 0:		// nm²⋅MHz
					break;	// do nothing
				case 1:		// cm/molec ⋅ 1e18
					intensity = sqNmMHz2cmPerMolecule(intensity);
					break;
				default:
					qWarning("wrong intensity unit index");
			}
			ui->table->setItem(_, 2,
							   new QTableWidgetItem(QString::number(intensity, 'f', 4)));
		}
	}
	ui->table->setSortingEnabled(true);
	ui->table->resizeColumnsToContents();
	ui->table->resizeRowsToContents();
	ui->menu_save->setDisabled(searchResults.isEmpty());
}

/**
 * @brief MainWindow::loadCatalogFile loads a catalog file, unzipping it if necessary
 * @param fileName is the name of the file to read from
 * @param type is the assumed JSON file type to load the file quicker
 * @bug “QJson: Document too large to store…” is silently raised if the JSON document is larger than MAX_INT
 * @returns a QJsonArray structure that is not empty if the file is loaded without an error
 */
QJsonArray MainWindow::loadCatalogFile(const QString &fileName, JsonType type) {
	QFile f(fileName);
	if (!f.open(QIODevice::ReadOnly)) {
		qWarning() << "Couldn't open file to load:" << fileName;
		return QJsonArray();
	}
	QJsonDocument json;
	if (type == JsonType::Binary) {
		json = QJsonDocument::fromBinaryData(f.readAll());
	}
	else if (type == JsonType::Zipped) {
		json = QJsonDocument::fromBinaryData(qUncompress(f.readAll()));
	}
	else if (type == JsonType::Text) {
		json = QJsonDocument::fromJson(f.readAll());
	}
	else {
		QByteArray file = f.readAll();
		if (fileName.endsWith(QString(".qbjsz"))) {
			json = QJsonDocument::fromBinaryData(qUncompress(file));
			if (json.isNull()) {
				json = QJsonDocument::fromJson(file);
			}
		}
		else if (fileName.endsWith(QString(".qbjs"))) {
			json = QJsonDocument::fromBinaryData(file);
			if (json.isNull()) {
				json = QJsonDocument::fromJson(file);
			}
		}
		else {
			json = QJsonDocument::fromJson(file);
			if (json.isNull()) {
				json = QJsonDocument::fromBinaryData(file);
				if (json.isNull()) {
					json = QJsonDocument::fromBinaryData(qUncompress(file));
				}
			}
		}
		if (json.isNull()) {
			qWarning("Unknown JSON type");
			return QJsonArray();
		}
	}
	if (json.isNull()) {
		qWarning("JSON parsing error");
		return QJsonArray();
	}
	QJsonObject catalog = json.object();
	if (!catalog["catalog"].isArray()) {
		qWarning("JSON malformed");
		return QJsonArray();
	}
	QJsonArray catalogFileData = catalog["catalog"].toArray();
	qInfo() << fileName << "loaded (" << catalogFileData.count() << ") records";
	return catalogFileData;
}

/**
 * @brief MainWindow::loadCatalogs loads one or several catalogs and applies filtering to them
 * @param fileNames is the list of the file names to load from
 * @bug “QJson: Document too large to store…” is silently raised if the resulting JSON document is larger than MAX_INT
 * @todo store the data loaded from each file in a separate QJsonArray
 * @returns true if some data is loaded and false otherwise
 */
bool MainWindow::loadCatalogs(const QStringList &fileNames) {
	while (!catalogData.isEmpty()) {
		catalogData.removeLast();
	}
	foreach (QString fileName, fileNames) {
		QJsonArray catalogFileData = loadCatalogFile(fileName);
		foreach (QJsonValue catalogDataItem, catalogFileData) {
			catalogData.append(catalogDataItem);
		}
	}
	filterSubstancesList(QString());
	bool ok = !catalogData.isEmpty();
	ui->menu_searchNames->setEnabled(ok);
	ui->button_search->setEnabled(ok);
	if (ok) {
		// apply filter after everything is loaded
		filterSubstancesList(ui->text_substance->text());
	}
	return ok;
}

/**
 * @brief MainWindow::filterSubstancesList lefts only the matching substance names in the substance list
 * @param filter is the string to match
 */
void MainWindow::filterSubstancesList(const QString &filter) {
	if (ui->check_saveselection->isChecked()) {
		for (int i = 0; i < ui->list_substance->count(); ++i) {
			QListWidgetItem *item = ui->list_substance->item(i);
			if (item->checkState() == Qt::Checked) {
				if (!selectedSubstances.contains(item->text())) {
					selectedSubstances.append(item->text());
				}
			}
			else {
				selectedSubstances.removeAll(item->text());
			}
		}
	}
	else {
		selectedSubstances.clear();
	}
	ui->list_substance->clear();
	foreach (QJsonValue catalogDataItem, catalogData) {
		QJsonObject catalogEntry = catalogDataItem.toObject();
		foreach (QString key, nameKeys) {
			QString name = catalogEntry.value(key).toString().trimmed();
			if (!name.isEmpty() && (filter.isEmpty() || name.contains(filter, Qt::CaseInsensitive))) {
				if (ui->list_substance->findItems(name, Qt::MatchExactly).isEmpty()) {
					QListWidgetItem* item = new QListWidgetItem(name, ui->list_substance);
					item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
					item->setCheckState(selectedSubstances.contains(name)?(Qt::Checked):(Qt::Unchecked));
				}
			}
		}
	}
}

/**
 * @brief MainWindow::callSubstance looks for the first non-empty name for a given substance catalog entry
 * @param catalogEntry is the catalog entry for a substance to find a name of
 * @returns the name of the substance or “??” if none is found
 */
QString MainWindow::callSubstance(const QJsonObject &catalogEntry) {
	QString catalogSubstanceName = "??";
	foreach (QString key, nameKeys) {
		catalogSubstanceName = catalogEntry.value(key).toString().trimmed();
		if (!catalogSubstanceName.isEmpty()) {
			break;
		}
	}
	return catalogSubstanceName;
}

/**
 * @brief MainWindow::on_button_search_clicked contains searching through the loaded catalog(s) routine
 */
void MainWindow::on_button_search_clicked()
{
	ui->statusBar->showMessage(tr("Searching..."));
	searchResults.clear();
	for (int i = 0; i < ui->list_substance->count(); ++i) {
		QListWidgetItem *item = ui->list_substance->item(i);
		if (item->checkState() == Qt::Checked) {
			if (!selectedSubstances.contains(item->text())) {
				selectedSubstances.append(item->text());
			}
		}
		else {
			selectedSubstances.removeAll(item->text());
		}
	}
	if (ui->box_substance->isChecked() && selectedSubstances.isEmpty()) {
		qWarning("nothing to search for");
		ui->statusBar->showMessage(tr("Nothing to search for. Select a substance in the list."), 8000);
		return;
	}
	double fmin, fmax, FREQ;
	if (ui->box_frequency->currentWidget() == ui->page_byrange) {
		if (frequencyFrom < frequencyTo) {
			fmin = frequencyFrom;
			fmax = frequencyTo;
		}
		else {
			fmin = frequencyTo;
			fmax = frequencyFrom;
		}
	}
	else {
		fmin = frequencyCenter - frequencyDeviation;
		fmax = frequencyCenter + frequencyDeviation;
	}
	foreach (QJsonValue catalogDataItem, catalogData) {
		QJsonObject catalogEntry = catalogDataItem.toObject();
		if (catalogEntry.value("lines").isArray()) {
			QJsonArray catalogEntryLines = catalogEntry.value("lines").toArray();
			QString catalogSubstanceName;
			bool matches = false;
			if (ui->box_substance->isChecked()) {
				foreach (QString key, nameKeys) {
					catalogSubstanceName = catalogEntry.value(key).toString().trimmed();
					foreach (QString selectedSubstance, selectedSubstances) {
						if (!catalogSubstanceName.isEmpty() && catalogSubstanceName == selectedSubstance) {
							catalogSubstanceName = callSubstance(catalogEntry);
							matches = true;
							break;
						}
					}
					if (matches) {
						break;
					}
				}
			}
			else {
				matches = true;			// the substance doesn't matter
				catalogSubstanceName = callSubstance(catalogEntry);
			}
			if (matches) {
				qint8 DR = catalogEntry.value("degreesoffreedom").toInt(-1);
				foreach (QJsonValue catalogEntryLinesItem, catalogEntryLines) {
					QJsonObject catalogEntryLine = catalogEntryLinesItem.toObject();
					FREQ = catalogEntryLine.value("frequency").toDouble();
					if (FREQ >= fmin && FREQ <= fmax) {
						double LGINT = catalogEntryLine.value("intensity").toDouble();
						double ELO = catalogEntryLine.value("lowerstateenegry").toDouble(-1.);
						searchResults.append(CatalogEntry(catalogSubstanceName, FREQ, LGINT, DR, ELO));
					}
				}
			}
		}
	}
	fillTable();
	ui->statusBar->showMessage(tr("Ready."));
}
