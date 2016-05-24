#pragma once
#include "qabstractitemmodel.h"

//Profile structure:
struct Profile {
	QString name;
	double width, height;
	Profile() = default;
	Profile(QString n, double w, double h)
		: name{ n }, width{ w }, height{ h } { }
};

class AppSettings : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int profileId READ getActiveProfileId WRITE setActiveProfileId NOTIFY profileIdChanged)
	QSettings* setting;
	std::vector<Profile> profiles;
public:
	AppSettings(QObject *parent = 0);
	AppSettings(const AppSettings&);
	~AppSettings();

signals:
	void settingsChanged();
	void profileIdChanged(int id);
	void profileUpdated();
	void shortcutUpdated();

public slots:
	int readInt(const QString& name, int defval = 0) const;
	double readDouble(const QString& name, double defval = 0.0) const;
	QString readString(const QString& name, const QString& defval = "") const;
	QSizeF readSize(const QString& name, const QSizeF& defval = QSizeF()) const;
	QPointF readPoint(const QString& name, const QPointF& defval = QPointF()) const;
	bool readBool(const QString& name, bool defval = false) const;

	void saveSettings(const QString& name, const QVariant& val);
	void eraseSettings(const QString& name);

	int checkProfileCount() const;
	int getActiveProfileId() const;
	void setActiveProfileId(int id);

	QString readProfileName(int id) const;
	double readProfileWidth(int id) const;
	double readProfileHeight(int id) const;

	void beginUpdateProfile();
	void updateProfile(const QString& name, const double& width, const double& height);
	void endUpdateProfile();

	void resetProfile();

	QString readShortcut(const QString& command, const QString& defkey);
	void saveShortcut(const QString& command, const QString& key);
	QString keyCodeToString(int key);

	void resetShortcut(const QString& command);
	void resetAllShortcut();
};

class Settings
{
	QSettings s;
	QString prefix;
public:
	Settings() = delete;
	Settings(const QString& prefix = "Global");
	Settings(const Settings& o);
	~Settings() = default;

	void setColor1(const QColor& shape_color);
	QColor color1() const;
	void setColor2(const QColor& outline_color);
	QColor color2() const;
	void setPenWidth(int width);
	int penWidth() const;
	void setFont(const QFont& font);
	QFont font() const;
	void setFontSize(int size);
	int fontSize() const;
	void setHasBackground(bool show);
	bool hasBackground() const;

	void setSize(int size);
	int size() const;

	void setCustom(const QString& key, const QVariant& val);
	QVariant custom(const QString& key, const QVariant& default_value = QVariant());
};

class ProfileModel :
	public QAbstractTableModel
{
	Q_OBJECT
	AppSettings setting;
public:
	ProfileModel(QObject* parent = 0);
	~ProfileModel() {};
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

	Profile at(int i);
	bool insertProfile(int row, Profile profile, const QModelIndex& parent = QModelIndex());

public slots:
	void resetProfile();
	void refreshProfile();
private:
	std::vector<Profile> storeCurrentProfiles();
};