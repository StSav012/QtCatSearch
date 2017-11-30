# QtCatSearch

Spectroscopic data searching tool.

**It's an offline application, and it does not perform searching for new data in the Internet. Everything should be created or downloaded manually from elsewhere.**

## Building

The application can be built with Qt5 toolkit.

## Catalog Files

**Beware:** due to an internal Qt limitation, the size of JSON structures is limited to `1 << 27` bytes. It's hard to estimate how large a file can be. Just beware.

The application currently supports 3 types of catalog files:

### 1. UTF-8 Encoded JSON

The files of the type can be easily created in any plain text editor. The format is quite expensive, and therefore such files are thought to be created manually to store some experimental results.

An example file of the type can be found in the repository.

### 2. Qt Binary JSON

It's Qt's own format. More info is available [here](http://doc.qt.io/qt-5/json.html) and [here](http://doc.qt.io/qt-5/qjsondocument.html). For convenience, the files should have `.qbjs` name extension.

### 3. Zipped Qt Binary JSON

Same as the previous one, the format is not conventional. However, the achievable compression is stunning: 20 to 100 times from plain text based JSON. The name extension of such files should be `.qbjsz`.

## Further Browsing

See [here](https://github.com/StSav012/qtCatDownloader) for a tool to download spectroscopic data and create compatible catalog files.

