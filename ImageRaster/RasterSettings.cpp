#include "stdafx.h"
#include "RasterSettings.h"

ProfileModel::ProfileModel(QObject* parent) 
	: QAbstractTableModel(parent), setting(this)
{
	connect(&setting, &AppSettings::profileUpdated, this, &ProfileModel::refreshProfile);
	refreshProfile();
}

int ProfileModel::columnCount(const QModelIndex& parent) const {
	return 3;
}

int ProfileModel::rowCount(const QModelIndex& parent) const {
	auto count = setting.checkProfileCount();
	return count;
}

QVariant ProfileModel::data(const QModelIndex& index, int role) const {
	if (index.row() >= setting.checkProfileCount()) return QVariant();
	int c = index.column();
	if (Qt::DisplayRole == role || Qt::EditRole == role) {
		if (0 == c)
			return setting.readProfileName(index.row());
		else if (1 == c)
			return setting.readProfileWidth(index.row());
		else if (2 == c)
			return setting.readProfileHeight(index.row());
	}
	return QVariant();
}

Qt::ItemFlags ProfileModel::flags(const QModelIndex& index) const {
	int c = index.column();
	if (0 == c)
		return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	else if (1 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else if (2 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	return 0;
}

QVariant ProfileModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (Qt::DisplayRole != role) return QVariant();
	if (Qt::Horizontal == orientation) {
		if (0 == section)
			return "Name";
		else if (1 == section)
			return "W";
		else if (2 == section)
			return "H";
	}
	return QString().number(section+1);
}

std::vector<Profile> ProfileModel::storeCurrentProfiles()
{
	std::vector<Profile> pfs;
	for (size_t i = 0; i < setting.checkProfileCount(); ++i)
	{
		auto name = setting.readProfileName(i);
		auto width = setting.readProfileWidth(i);
		auto height = setting.readProfileHeight(i);
		pfs.emplace_back(name, width, height);
	}
	return pfs;
}

bool ProfileModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	int c = index.column();
	auto pfs = storeCurrentProfiles();
	Profile b = pfs[index.row()];
	if (Qt::EditRole == role) {
		if (0 == c)
			b.name = value.toString();
		else if (1 == c)
			b.width = value.toInt();
		else if (2 == c)
			b.height = value.toInt();
		else
			return false;
		
		setting.beginUpdateProfile();
		for (size_t i = 0; i < pfs.size(); ++i)
		{
			if (i == index.row()) pfs[i] = b;
			setting.updateProfile(pfs[i].name, pfs[i].width, pfs[i].height);
		}
		setting.endUpdateProfile();
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

bool ProfileModel::insertRows(int row, int count, const QModelIndex& parent) {
	beginInsertRows(parent, row, row+count-1);
	auto pfs = storeCurrentProfiles();
	setting.beginUpdateProfile();
	for (size_t i = 0; i < pfs.size(); ++i)
	{
		setting.updateProfile(pfs[i].name, pfs[i].width, pfs[i].height);
		if (i == row)
		{
			for (size_t j = 0; j < count; ++j)
				setting.updateProfile("empty", 0.0, 0.0);
		}
	}
	if (row >= pfs.size())
	{
		for (size_t j = 0; j < count; ++j)
			setting.updateProfile("empty", 0.0, 0.0);
	}
	setting.endUpdateProfile();
	endInsertRows();
	return true;
}

bool ProfileModel::removeRows(int row, int count, const QModelIndex& parent) {
	beginRemoveRows(parent, row, row+count-1);
	auto pfs = storeCurrentProfiles();
	setting.beginUpdateProfile();
	for (size_t i = 0; i < pfs.size(); ++i)
	{
		if (i == row)
		{
			for (size_t j = 0; j < count; ++j)
				++i;
		}
		setting.updateProfile(pfs[i].name, pfs[i].width, pfs[i].height);
	}
	setting.endUpdateProfile();
	endRemoveRows();
	return true;
}

Profile ProfileModel::at(int i) {
	Profile p;
	p.name = setting.readProfileName(i);
	p.width = setting.readProfileWidth(i);
	p.height = setting.readProfileHeight(i);
	return p;
}

bool ProfileModel::insertProfile(int row, Profile profile, const QModelIndex& parent) {
	beginInsertRows(parent, row, row);
	auto pfs = storeCurrentProfiles();
	setting.beginUpdateProfile();
	for (size_t i = 0; i < pfs.size(); ++i)
	{
		setting.updateProfile(pfs[i].name, pfs[i].width, pfs[i].height);
		if (i == row)
		{
			setting.updateProfile(profile.name, profile.width, profile.height);
		}
	}
	if (row >= pfs.size()) setting.updateProfile(profile.name, profile.width, profile.height);
	setting.endUpdateProfile();
	endInsertRows();
	return true;
}

void ProfileModel::resetProfile() {
	setting.resetProfile();
}

void ProfileModel::refreshProfile() {
	auto tl = this->index(0, 0);
	auto br = this->index(rowCount(), columnCount());
	emit dataChanged(tl, br);
}

AppSettings::AppSettings(QObject *parent)
	: QObject(parent)
{
	setting = new QSettings{ QSettings::IniFormat, QSettings::UserScope, "Miconos", "Optilab", this };
}

AppSettings::AppSettings(const AppSettings& o)
	: QObject{ o.parent() }, setting{ new QSettings{ QSettings::IniFormat, QSettings::UserScope, "Miconos", "Optilab", this} }
{
}

AppSettings::~AppSettings()
{

}

int AppSettings::readInt(const QString &name, int defval) const {
	int val = setting->value(name, defval).toInt();
	return val;
}

double AppSettings::readDouble(const QString &name, double defval) const {
	double val = setting->value(name, defval).toDouble();
	return val;
}

QString AppSettings::readString(const QString &name, const QString &defval) const {
	QString val = setting->value(name, defval).toString();
	return val;
}

QSizeF AppSettings::readSize(const QString &name, const QSizeF &defval) const {
	QSizeF val = setting->value(name, defval).toSizeF();
	return val;
}

QPointF AppSettings::readPoint(const QString &name, const QPointF &defval) const {
	QPointF val = setting->value(name, defval).toPointF();
	return val;
}

bool AppSettings::readBool(const QString &name, bool defval) const {
	bool val = setting->value(name, defval).toBool();
	return val;
}

void AppSettings::saveSettings(const QString &name, const QVariant &val) {
	setting->setValue(name, val);
	emit settingsChanged();
}

void AppSettings::eraseSettings(const QString &name) {
	setting->remove(name);
	emit settingsChanged();
}

int AppSettings::checkProfileCount() const {
	setting->beginGroup("Profile");
	auto val = readInt("size", 0);
	setting->endGroup();
	return val;
}

int AppSettings::getActiveProfileId() const {
	setting->beginGroup("Profile");
	auto val = readInt("active", 0);
	setting->endGroup();
	return val;
}

void AppSettings::setActiveProfileId(int id) {
	setting->beginGroup("Profile");
	saveSettings("active", id);
	setting->endGroup();
	emit profileIdChanged(id);
}

QString AppSettings::readProfileName(int id) const {
	setting->beginReadArray("Profile");
	setting->setArrayIndex(id);
	auto val = readString("name", "empty");
	setting->endArray();
	return val;
}

double AppSettings::readProfileWidth(int id) const {
	setting->beginReadArray("Profile");
	setting->setArrayIndex(id);
	auto val = readDouble("width", 0.0);
	setting->endArray();
	return val;
}

double AppSettings::readProfileHeight(int id) const {
	setting->beginReadArray("Profile");
	setting->setArrayIndex(id);
	auto val = readDouble("height", 0.0);
	setting->endArray();
	return val;
}

void AppSettings::beginUpdateProfile() {
	profiles.clear();
}

void AppSettings::updateProfile(const QString &name, const double &width, const double &height) {
	Profile profile;
	profile.name = name;
	profile.width = width;
	profile.height = height;
	profiles.push_back(profile);
}

void AppSettings::endUpdateProfile() {
	setting->beginWriteArray("Profile");
	for (size_t i = 0; i < profiles.size(); ++i) {
		setting->setArrayIndex((int)i);
		setting->setValue("name", profiles.at(i).name);
		setting->setValue("width", profiles.at(i).width);
		setting->setValue("height", profiles.at(i).height);
	}
	setting->endArray();
	emit profileUpdated();
}

void AppSettings::resetProfile() {
	beginUpdateProfile();
	updateProfile("4x", 2560.0, 1913);
	updateProfile("10x", 1024.0, 775.0);
	updateProfile("40x", 364.0, 245.0);
	updateProfile("100x", 102.0, 78.0);
	endUpdateProfile();
}

QString AppSettings::readShortcut(const QString &command, const QString& defkey) {
	setting->beginGroup("Shortcuts");
	auto key = readString(command, defkey);
	setting->endGroup();
	return key;
}

void AppSettings::saveShortcut(const QString &command, const QString &key) {
	setting->beginGroup("Shortcuts");
	saveSettings(command, key);
	setting->endGroup();
	emit shortcutUpdated();
}

QString AppSettings::keyCodeToString(int key) {
	QKeySequence seq(key);
	//    qDebug() << seq.toString();
	return seq.toString();
}

void AppSettings::resetShortcut(const QString &command) {
	setting->beginGroup("Shortcuts");
	setting->remove(command);
	setting->endGroup();
	emit shortcutUpdated();
}

void AppSettings::resetAllShortcut() {
	setting->beginGroup("Shortcuts");
	setting->remove("");
	setting->endGroup();
	emit shortcutUpdated();
}

Settings::Settings(const QString& prefix)
	: s(QSettings::IniFormat, QSettings::UserScope, "Miconos", "ImageRaster"), prefix(prefix) 
{ 
}

Settings::Settings(const Settings& o)
	: s{ QSettings::IniFormat, QSettings::UserScope, "Miconos", "ImageRaster" }, prefix{ o.prefix }
{
}

void Settings::setColor1(const QColor& shape_color) {
	s.setValue(prefix + "_Color1", shape_color); 
}

QColor Settings::color1() const { 
	return qvariant_cast<QColor>(s.value(prefix + "_Color1", QColor(Qt::black))); 
}

void Settings::setColor2(const QColor& outline_color) {
	s.setValue(prefix + "_Color2", outline_color); 
}

QColor Settings::color2() const {
	return qvariant_cast<QColor>(s.value(prefix + "_Color2", QColor(Qt::white))); 
}

void Settings::setPenWidth(int width) {
	s.setValue(prefix + "_PenWidth", width); 
}

int Settings::penWidth() const {
	return s.value(prefix + "_PenWidth", 5).toInt(); 
}

void Settings::setFont(const QFont& font) {
	s.setValue(prefix + "_Font", font); 
}

QFont Settings::font() const {
	return qvariant_cast<QFont>(s.value(prefix + "_Font", QFont("Calibri"))); 
}

void Settings::setFontSize(int size) {
	s.setValue(prefix + "_FontSize", size); 
}
int Settings::fontSize() const {
	return s.value(prefix + "_FontSize", 40).toInt(); 
}

void Settings::setHasBackground(bool show) {
	s.setValue(prefix + "_HasBackground", show); 
}

bool Settings::hasBackground() const {
	return s.value(prefix + "_HasBackground", true).toBool(); 
}

void Settings::setSize(int size) {
	s.setValue(prefix + "_Size", size); 
}

int Settings::size() const {
	return s.value(prefix + "_Size", 20).toInt(); 
}

void Settings::setCustom(const QString& key, const QVariant& val) {
	s.setValue(prefix + key, val); 
}

QVariant Settings::custom(const QString& key, const QVariant& default_value) {
	return s.value(prefix + key, default_value); 
}