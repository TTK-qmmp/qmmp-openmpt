#include "openmpthelper.h"
#include "archivereader.h"

OpenMPTHelper::OpenMPTHelper(QIODevice *input)
    : m_input(input)
{

}

OpenMPTHelper::~OpenMPTHelper()
{
    deinit();
}

void OpenMPTHelper::deinit()
{
    if(m_mod)
    {
        openmpt_module_destroy(m_mod);
    }
}

bool OpenMPTHelper::initialize()
{
    {
        QFile *file = static_cast<QFile*>(m_input);
        const QString &path = file ? file->fileName() : QString();
        if(ArchiveReader::isSupported(path))
        {
            const QByteArray &data = ArchiveReader::unpack(path);
            m_mod = openmpt_module_create_from_memory2(data.constData(), data.length(), openmpt_log_func_silent, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
        }
        else
        {
            m_mod = openmpt_module_create2(callbacks, m_input, openmpt_log_func_silent, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
        }
    }

    if(!m_mod)
    {
        return false;
    }

    openmpt_module_select_subsong(m_mod, -1);

    m_length = openmpt_module_get_duration_seconds(m_mod) * 1000;
    m_title = toString(openmpt_module_get_metadata(m_mod, "title"));
    m_patternCount = openmpt_module_get_num_patterns(m_mod);
    m_instrumentCount = openmpt_module_get_num_instruments(m_mod);
    m_sampleCount = openmpt_module_get_num_samples(m_mod);
    m_channelCount = openmpt_module_get_num_channels(m_mod);

    for(int i = 0; i < openmpt_module_get_num_instruments(m_mod); ++i)
    {
        m_instruments += toString(openmpt_module_get_instrument_name(m_mod, i));
        m_instruments += "\n";
    }

    for(int i = 0; i < openmpt_module_get_num_samples(m_mod); ++i)
    {
        m_samples += toString(openmpt_module_get_sample_name(m_mod, i));
        m_samples += "\n";
    }

    m_comment = toString(openmpt_module_get_metadata(m_mod, "message_raw"));
    return true;
}

size_t OpenMPTHelper::stream_read(void *instance, void *buf, std::size_t n)
{
    return VFS(instance)->read(reinterpret_cast<char *>(buf), n);
}

int OpenMPTHelper::stream_seek(void *instance, std::int64_t offset, int whence)
{
    if(VFS(instance)->isSequential())
    {
        return -1;
    }

    qint64 pos;
    switch(whence)
    {
        case OPENMPT_STREAM_SEEK_SET:
            pos = offset;
            break;
        case OPENMPT_STREAM_SEEK_CUR:
            pos = VFS(instance)->pos() + offset;
            break;
        case OPENMPT_STREAM_SEEK_END:
            pos = VFS(instance)->size() + offset;
            break;
        default:
            return -1;
    }

    return VFS(instance)->seek(pos) ? 0 : -1;
}

std::int64_t OpenMPTHelper::stream_tell(void *instance)
{
    return VFS(instance)->isSequential() ? -1 : VFS(instance)->pos();
}

QString OpenMPTHelper::toString(const char *input)
{
    if(input != nullptr)
    {
        QString v(input);
        openmpt_free_string(input);
        return v;
    }
    else
    {
        return QString();
    }
}

QMap<QString, int> OpenMPTHelper::interpolators()
{
    QMap<QString, int> interpolators;
    interpolators.insert(QObject::tr("None"), INTERP_NONE);
    interpolators.insert(QObject::tr("Linear"), INTERP_LINEAR);
    interpolators.insert(QObject::tr("Cubic"), INTERP_CUBIC);
    interpolators.insert(QObject::tr("Windowed sinc"), INTERP_WINDOWED);
    return interpolators;
}

bool OpenMPTHelper::isValidInterpolator(int value)
{
    return interpolators().values().contains(value);
}

void OpenMPTHelper::setInterpolator(int value)
{
    if(isValidInterpolator(value))
    {
        openmpt_module_set_render_param(m_mod, OPENMPT_MODULE_RENDER_INTERPOLATIONFILTER_LENGTH, value);
    }
}

bool OpenMPTHelper::isValidStereoSeparation(int separation)
{
    return separation >= 0 && separation <= 100;
}

void OpenMPTHelper::setStereoSeparation(int separation)
{
    if(isValidStereoSeparation(separation))
    {
        openmpt_module_set_render_param(m_mod, OPENMPT_MODULE_RENDER_STEREOSEPARATION_PERCENT, separation);
    }
}

qint64 OpenMPTHelper::read(unsigned char *data, qint64 maxSize)
{
    maxSize /= sizeof(float) * channels();
    const size_t n = openmpt_module_read_interleaved_float_stereo(m_mod, sampleRate(), maxSize, reinterpret_cast<float *>(data));
    return n * channels() * sizeof(float);
}
