
#include "Preferences.h"

#include <QSettings>

Preferences::Preferences()
    :_handle(0)
    , _started(false)
    , _readOnly(false)
    , settings("HammerschmidtEDV", "WoodLightSim")
{}

Preferences::~Preferences() {
    end();
}

bool Preferences::begin(const char* name, bool readOnly, const char* partition_label) {
    if (_started) {
        return false;
    }
    _readOnly = readOnly;
    esp_err_t err = ESP_OK;
    settings.beginGroup(partition_label ? partition_label : "default_partition");
    settings.beginGroup(name ? name : "default_group");
    _started = true;
    return true;
}

void Preferences::end() {
    if (!_started) {
        return;
    }
    settings.endGroup();
    settings.endGroup();
    _started = false;
}

/*
 * Clear all keys in opened preferences
 * */

bool Preferences::clear() {
    if (!_started || _readOnly) {
        return false;
    }
    for (auto& key : settings.allKeys())
        settings.remove(key);
    return true;
}

/*
 * Remove a key
 * */

bool Preferences::remove(const char* key) {
    if (!_started || !key || _readOnly) {
        return false;
    }
    settings.remove(key);
    return true;
}

/*
 * Put a key value
 * */

//size_t Preferences::putChar(const char* key, int8_t value) {
//    if (!_started || !key || _readOnly) {
//        return 0;
//    }
//    settings.setValue(key, value);
//    return 1;
//}
//
//size_t Preferences::putUChar(const char* key, uint8_t value) {
//    if (!_started || !key || _readOnly) {
//        return 0;
//    }
//    settings.setValue(key, value);
//    return 1;
//}
//
//size_t Preferences::putShort(const char* key, int16_t value) {
//    if (!_started || !key || _readOnly) {
//        return 0;
//    }
//    settings.setValue(key, value);
//    return 2;
//}
//
//size_t Preferences::putUShort(const char* key, uint16_t value) {
//    if (!_started || !key || _readOnly) {
//        return 0;
//    }
//    settings.setValue(key, value);
//    return 2;
//}

size_t Preferences::putInt(const char* key, int32_t value) {
    if (!_started || !key || _readOnly) {
        return 0;
    }
    settings.setValue(key, value);
    return 4;
}

size_t Preferences::putUInt(const char* key, uint32_t value) {
    if (!_started || !key || _readOnly) {
        return 0;
    }
    settings.setValue(key, value);
    return 4;
}

size_t Preferences::putLong(const char* key, int32_t value) {
    return putInt(key, value);
}

size_t Preferences::putULong(const char* key, uint32_t value) {
    return putUInt(key, value);
}

size_t Preferences::putLong64(const char* key, int64_t value) {
    if (!_started || !key || _readOnly) {
        return 0;
    }
    settings.setValue(key, value);
    return 8;
}

size_t Preferences::putULong64(const char* key, uint64_t value) {
    if (!_started || !key || _readOnly) {
        return 0;
    }
    settings.setValue(key, value);
    return 8;
}

size_t Preferences::putFloat(const char* key, const float_t value) {
    if (!_started || !key || _readOnly) {
        return 0;
    }
    settings.setValue(key, value);
    return 4;
}

size_t Preferences::putDouble(const char* key, const double_t value) {
    if (!_started || !key || _readOnly) {
        return 0;
    }
    settings.setValue(key, value);
    return 8;
}

size_t Preferences::putBool(const char* key, const bool value) {
    if (!_started || !key || _readOnly) {
        return 0;
    }
    settings.setValue(key, value);
    return 4;
}

//size_t Preferences::putString(const char* key, const char* value) {
//    if (!_started || !key || !value || _readOnly) {
//        return 0;
//    }
//    settings.setValue(key, value);
//    return strlen(value);
//}

size_t Preferences::putString(const char* key, const String value) {
    if (!_started || !key || _readOnly) {
        return 0;
    }
    settings.setValue(key, QString::fromStdString(value));
    return value.size();
}

//size_t Preferences::putBytes(const char* key, const void* value, size_t len) {
//    if (!_started || !key || !value || !len || _readOnly) {
//        return 0;
//    }
//    settings.setValue(key, value);
//    return len;
//}

PreferenceType Preferences::getType(const char* key) {
    if (!_started || !key || strlen(key) > 15) {
        return PT_INVALID;
    }
    switch (settings.value(key).type())
    {
    case QVariant::Type::Int: return PT_I32;
    case QVariant::Type::LongLong: return PT_I64;
    case QVariant::Type::UInt: return PT_U32;
    case QVariant::Type::ULongLong: return PT_U64;
    case QVariant::Type::String: return PT_STR;


    default:
        return PT_INVALID;


    }
/*
    int8_t mt1; uint8_t mt2; int16_t mt3; uint16_t mt4;
    int32_t mt5; uint32_t mt6; int64_t mt7; uint64_t mt8;
    size_t len = 0;
    if (nvs_get_i8(_handle, key, &mt1) == ESP_OK) return PT_I8;
    if (nvs_get_u8(_handle, key, &mt2) == ESP_OK) return PT_U8;
    if (nvs_get_i16(_handle, key, &mt3) == ESP_OK) return PT_I16;
    if (nvs_get_u16(_handle, key, &mt4) == ESP_OK) return PT_U16;
    if (nvs_get_i32(_handle, key, &mt5) == ESP_OK) return PT_I32;
    if (nvs_get_u32(_handle, key, &mt6) == ESP_OK) return PT_U32;
    if (nvs_get_i64(_handle, key, &mt7) == ESP_OK) return PT_I64;
    if (nvs_get_u64(_handle, key, &mt8) == ESP_OK) return PT_U64;
    if (nvs_get_str(_handle, key, NULL, &len) == ESP_OK) return PT_STR;
    if (nvs_get_blob(_handle, key, NULL, &len) == ESP_OK) return PT_BLOB;
    return PT_INVALID;
*/
}

bool Preferences::isKey(const char* key) {
    return getType(key) != PT_INVALID;
}

/*
 * Get a key value
 * */

//int8_t Preferences::getChar(const char* key, const int8_t defaultValue) {
//    int8_t value = defaultValue;
//    if (!_started || !key) {
//        return value;
//    }
//    return getType(key) == PT_I32
//        ? settings.value(key).toInt()
//        : defaultValue;
//
//    return value;
//}
//
//uint8_t Preferences::getUChar(const char* key, const uint8_t defaultValue) {
//    uint8_t value = defaultValue;
//    if (!_started || !key) {
//        return value;
//    }
//    return getType(key) == PT_U32
//        ? settings.value(key).toInt()
//        : defaultValue;
//    return value;
//}
//
//int16_t Preferences::getShort(const char* key, const int16_t defaultValue) {
//    int16_t value = defaultValue;
//    if (!_started || !key) {
//        return value;
//    }
//    return getType(key) == PT_I32
//        ? settings.value(key).toInt()
//        : defaultValue;
//    return value;
//}
//
//uint16_t Preferences::getUShort(const char* key, const uint16_t defaultValue) {
//    uint16_t value = defaultValue;
//    if (!_started || !key) {
//        return value;
//    }
//    return getType(key) == PT_U32
//        ? settings.value(key).toInt()
//        : defaultValue;
//    return value;
//}

int32_t Preferences::getInt(const char* key, const int32_t defaultValue) {
    int32_t value = defaultValue;
    if (!_started || !key) {
        return value;
    }
    return getType(key) == PT_I32
        ? settings.value(key).toInt()
        : defaultValue;
    return value;
}

uint32_t Preferences::getUInt(const char* key, const uint32_t defaultValue) {
    uint32_t value = defaultValue;
    if (!_started || !key) {
        return value;
    }
    return getType(key) == PT_U32
        ? settings.value(key).toInt()
        : defaultValue;
    return value;
}

int32_t Preferences::getLong(const char* key, const int32_t defaultValue) {
    return getInt(key, defaultValue);
}

uint32_t Preferences::getULong(const char* key, const uint32_t defaultValue) {
    return getUInt(key, defaultValue);
}

int64_t Preferences::getLong64(const char* key, const int64_t defaultValue) {
    int64_t value = defaultValue;
    if (!_started || !key) {
        return value;
    }
    return getType(key) == PT_I64
        ? settings.value(key).toInt()
        : defaultValue;
    return value;
}

uint64_t Preferences::getULong64(const char* key, const uint64_t defaultValue) {
    if (!_started || !key) {
        return defaultValue;
    }
    return getType(key) == PT_U64
        ? settings.value(key).toInt()
        : defaultValue;
}

float_t Preferences::getFloat(const char* key, const float_t defaultValue) {
    if (!_started || !key) {
        return defaultValue;
    }
    return getType(key) == PT_BLOB
        ? settings.value(key).toFloat()
        : defaultValue;
}

double_t Preferences::getDouble(const char* key, const double_t defaultValue) {
    if (!_started || !key) {
        return defaultValue;
    }
    return getType(key) == PT_BLOB
        ? settings.value(key).toDouble()
        : defaultValue;
}

bool Preferences::getBool(const char* key, const bool defaultValue) {
    if (!_started || !key) {
        return defaultValue;
    }
    return getType(key) == PT_BLOB
        ? settings.value(key).toBool()
        : defaultValue;

}

//size_t Preferences::getString(const char* key, char* value, const size_t maxLen) {
//    size_t len = 0;
//    if (!_started || !key || !value || !maxLen) {
//        return 0;
//    }
//    if (!_started || !key) {
//        return defaultValue;
//    }
//    return getType(key) == PT_STR
//        ? settings.value(key).toString
//        : defaultValue;
//
//    esp_err_t err = nvs_get_str(_handle, key, NULL, &len);
//    if (err) {
//        log_e("nvs_get_str len fail: %s %s", key, nvs_error(err));
//        return 0;
//    }
//    if (len > maxLen) {
//        log_e("not enough space in value: %u < %u", maxLen, len);
//        return 0;
//    }
//    err = nvs_get_str(_handle, key, value, &len);
//    if (err) {
//        log_e("nvs_get_str fail: %s %s", key, nvs_error(err));
//        return 0;
//    }
//    return len;
//}

String Preferences::getString(const char* key, const String defaultValue) {
    if (!_started || !key) {
        return defaultValue;
    }
    return getType(key) == PT_STR
        ? String(settings.value(key).toString().toStdString())
        : defaultValue;
}

//size_t Preferences::getBytesLength(const char* key) {
//    size_t len = 0;
//    if (!_started || !key) {
//        return 0;
//    }
//    return getType(key) == PT_STR
//        ? settings.value(key).toString().length()
//        : 0;
//}
//
//size_t Preferences::getBytes(const char* key, void* buf, size_t maxLen) {
//    size_t len = getBytesLength(key);
//    if (!len || !buf || !maxLen) {
//        return len;
//    }
//    if (len > maxLen) {
//        log_e("not enough space in buffer: %u < %u", maxLen, len);
//        return 0;
//    }
//    esp_err_t err = nvs_get_blob(_handle, key, buf, &len);
//    if (err) {
//        log_e("nvs_get_blob fail: %s %s", key, nvs_error(err));
//        return 0;
//    }
//    return len;
//}
//
//size_t Preferences::freeEntries() {
//    nvs_stats_t nvs_stats;
//    esp_err_t err = nvs_get_stats(NULL, &nvs_stats);
//    if (err) {
//        log_e("Failed to get nvs statistics");
//        return 0;
//    }
//    return nvs_stats.free_entries;
//}