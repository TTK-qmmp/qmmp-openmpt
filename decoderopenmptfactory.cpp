#include "decoderopenmptfactory.h"
#include "decoder_openmpt.h"
#include "openmptmetadatamodel.h"
#include "openmpthelper.h"
#include "settingsdialog.h"

#include <QFile>
#include <QSettings>
#include <QMessageBox>

bool DecoderOpenMPTFactory::canDecode(QIODevice *input) const
{
    OpenMPTHelper helper(input);
    return helper.initialize();
}

DecoderProperties DecoderOpenMPTFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("OpenMPT Plugin");
    properties.description = tr("OpenMPT Module Files");
    properties.shortName = "openmpt";
    properties.filters << "*.669";
    properties.filters << "*.ams" << "*.amf";
    properties.filters << "*.dsm" << "*.dmf" << "*.dbm" << "*.digi" << "*.dtm";
    properties.filters << "*.far";
    properties.filters << "*.gdm";
    properties.filters << "*.it" << "*.ice" << "*.imf";
    properties.filters << "*.j2b";
    properties.filters << "*.mod" << "*.mptm" << "*.m15" << "*.mtm" << "*.med" << "*.mdl" << "*.mt2" << "*.mms" << "*.mo3" << "*.mmcmp";
    properties.filters << "*.nst";
    properties.filters << "*.okt" << "*.okta";
    properties.filters << "*.pt36" << "*.ptm" << "*.psm" << "*.plm" << "*.ppm";
    properties.filters << "*.s3m" << "*.stm" << "*.stk" << "*.st26" << "*.sfx" << "*.sfx2" << "*.stp";
    properties.filters << "*.ult" << "*.umx";
    properties.filters << "*.wow";
    properties.filters << "*.xm" << "*.xpk";
    properties.hasSettings = true;
    return properties;
}

Decoder *DecoderOpenMPTFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(path);
    return new DecoderOpenMPT(input);
}

QList<TrackInfo*> DecoderOpenMPTFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *)
{
    TrackInfo *info = new TrackInfo(path);

    if(parts == TrackInfo::Parts())
    {
        return QList<TrackInfo*>() << info;
    }

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        delete info;
        return QList<TrackInfo*>();
    }

    OpenMPTHelper helper(&file);
    if(!helper.initialize())
    {
        delete info;
        return QList<TrackInfo*>();
    }

    if(parts & TrackInfo::MetaData)
    {
        QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
        if(settings.value("OpenMPT/use_file_name", false).toBool())
        {
            info->setValue(Qmmp::TITLE, path.section('/', -1));
        }
        info->setValue(Qmmp::TITLE, helper.title());
    }

    if(parts & TrackInfo::Properties)
    {
        info->setValue(Qmmp::BITRATE, helper.bitrate());
        info->setValue(Qmmp::SAMPLERATE, helper.rate());
        info->setValue(Qmmp::CHANNELS, helper.channels());
        info->setValue(Qmmp::BITS_PER_SAMPLE, helper.depth());
        info->setValue(Qmmp::FORMAT_NAME, "OpenMPT");
        info->setDuration(helper.length());
    }

    file.close();
    return QList<TrackInfo*>() << info;
}

MetaDataModel *DecoderOpenMPTFactory::createMetaDataModel(const QString &path, bool readOnly)
{
    Q_UNUSED(readOnly);
    return new OpenMPTMetaDataModel(path);
}

void DecoderOpenMPTFactory::showSettings(QWidget *parent)
{
    SettingsDialog *s = new SettingsDialog(parent);
    s->show();
}

void DecoderOpenMPTFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About OpenMPT Reader Plugin"),
                        tr("Qmmp OpenMPT Reader Plugin")+"\n"+
                        tr("Written by: Greedysky <greedysky@163.com>"));
}

QString DecoderOpenMPTFactory::translation() const
{
    return QString();
}
