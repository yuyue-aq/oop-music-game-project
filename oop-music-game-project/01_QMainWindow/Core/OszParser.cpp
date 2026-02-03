#include "OszParser.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTemporaryDir>
#include <QProcess>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#endif

/**
 * @brief 解析 OSZ 压缩包文件
 * @param oszPath OSZ 文件路径
 * @return 解析成功返回包含所有谱面信息的 OszPackage 对象，失败返回 nullptr
 */
OszPackage* OszParser::parseOszFile(const std::string& oszPath) {
    // 验证文件存在
    QFile file(QString::fromStdString(oszPath));
    if (!file.exists()) {
        qDebug() << "OSZ file not found:" << QString::fromStdString(oszPath);
        return nullptr;
    }
    
    // 创建包对象
    OszPackage* package = new OszPackage();
    package->packagePath = oszPath;
    
    // 创建临时解压目录
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        qDebug() << "Failed to create temp directory";
        delete package;
        return nullptr;
    }
    
    // 保存解压路径并禁用自动删除（稍后手动清理）
    package->extractPath = tempDir.path().toStdString();
    tempDir.setAutoRemove(false);
    
    // 解压 OSZ 文件
    if (!extractOszFile(oszPath, package->extractPath)) {
        qDebug() << "Failed to extract OSZ file";
        delete package;
        return nullptr;
    }
    
    // 扫描解压目录中的所有 .osu 文件
    QDir dir(QString::fromStdString(package->extractPath));
    QStringList filters;
    filters << "*.osu";
    QFileInfoList osuFiles = dir.entryInfoList(filters, QDir::Files);
    
    if (osuFiles.isEmpty()) {
        qDebug() << "No .osu files found in package";
        delete package;
        return nullptr;
    }
    
    // 解析每个 .osu 文件
    for (const QFileInfo& fileInfo : osuFiles) {
        OsuBeatmap* beatmap = parseOsuFile(fileInfo.absoluteFilePath().toStdString());
        if (beatmap) {
            package->beatmaps.push_back(beatmap);
            
            // 查找对应的音频文件（使用第一个有效的音频文件名）
            if (package->audioFilePath.empty() && !beatmap->audioFilename.empty()) {
                QString audioPath = dir.absoluteFilePath(QString::fromStdString(beatmap->audioFilename));
                if (QFile::exists(audioPath)) {
                    package->audioFilePath = audioPath.toStdString();
                }
            }
        }
    }
    
    // 验证是否成功解析到谱面
    if (package->beatmaps.empty()) {
        qDebug() << "No valid beatmaps found";
        delete package;
        return nullptr;
    }
    
    qDebug() << "Successfully parsed OSZ package with" << package->beatmaps.size() << "beatmaps";
    qDebug() << "Audio file:" << QString::fromStdString(package->audioFilePath);
    
    return package;
}

/**
 * @brief 解压 OSZ 文件到目标目录
 * @param oszPath OSZ 源文件路径
 * @param destPath 目标解压目录
 * @return 解压成功返回 true，失败返回 false
 */
bool OszParser::extractOszFile(const std::string& oszPath, const std::string& destPath) {
    QString oszFile = QString::fromStdString(oszPath);
    QString zipFile = oszFile;
    
    // OSZ 文件本质是 ZIP 格式，需要重命名为 .zip 才能解压
    if (oszFile.endsWith(".osz", Qt::CaseInsensitive)) {
        zipFile = QString::fromStdString(destPath) + "/temp.zip";
        if (!QFile::copy(oszFile, zipFile)) {
            qDebug() << "Failed to copy OSZ file";
            return false;
        }
    }
    
#ifdef Q_OS_WIN
    // Windows 平台：使用 PowerShell 的 Expand-Archive 命令
    QString destDir = QString::fromStdString(destPath);
    
    QProcess process;
    QString command = QString("Expand-Archive -Path '%1' -DestinationPath '%2' -Force")
                          .arg(zipFile).arg(destDir);
    
    process.start("powershell", QStringList() << "-Command" << command);
    
    // 等待解压完成（最多 30 秒）
    if (!process.waitForFinished(30000)) {
        qDebug() << "Extraction timeout";
        return false;
    }
    
    // 检查解压是否成功
    if (process.exitCode() != 0) {
        QString error = QString::fromLocal8Bit(process.readAllStandardError());
        qDebug() << "Extraction failed:" << error;
        return false;
    }
    
    qDebug() << "Extraction successful";
    return true;
#else
    // Linux/Mac 平台：使用 unzip 命令
    QProcess process;
    process.start("unzip", QStringList() << "-o" << zipFile << "-d" << QString::fromStdString(destPath));
    
    if (!process.waitForFinished(30000)) {
        qDebug() << "Extraction timeout";
        return false;
    }
    
    return process.exitCode() == 0;
#endif
}

/**
 * @brief 解析单个 .osu 谱面文件
 * @param osuFilePath .osu 文件的完整路径
 * @return 解析成功返回 OsuBeatmap 对象，失败返回 nullptr
 */
OsuBeatmap* OszParser::parseOsuFile(const std::string& osuFilePath) {
    // 打开文件
    std::ifstream file(osuFilePath);
    if (!file.is_open()) {
        qDebug() << "Failed to open .osu file:" << QString::fromStdString(osuFilePath);
        return nullptr;
    }
    
    // 读取文件全部内容
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    OsuBeatmap* beatmap = new OsuBeatmap();
    
    // ==================== 解析 [General] 节 ====================
    std::string generalSection = getSection(content, "[General]");
    size_t pos = generalSection.find("AudioFilename:");
    if (pos != std::string::npos) {
        std::string line = generalSection.substr(pos);
        line = line.substr(0, line.find('\n'));
        beatmap->audioFilename = trim(line.substr(line.find(':') + 1));
    }
    
    // ==================== 解析 [Metadata] 节 ====================
    std::string metadataSection = getSection(content, "[Metadata]");
    // Lambda 函数：解析键值对格式的元数据
    auto parseMetadata = [&](const std::string& key, std::string& value) {
        size_t p = metadataSection.find(key + ":");
        if (p != std::string::npos) {
            std::string line = metadataSection.substr(p);
            line = line.substr(0, line.find('\n'));
            value = trim(line.substr(line.find(':') + 1));
        }
    };
    
    parseMetadata("Title", beatmap->title);
    parseMetadata("Artist", beatmap->artist);
    parseMetadata("Creator", beatmap->creator);
    parseMetadata("Version", beatmap->version);
    
    // ==================== 解析 [Difficulty] 节 ====================
    std::string difficultySection = getSection(content, "[Difficulty]");
    // Lambda 函数：解析浮点数格式的难度参数
    auto parseDifficulty = [&](const std::string& key, float& value) {
        size_t p = difficultySection.find(key + ":");
        if (p != std::string::npos) {
            std::string line = difficultySection.substr(p);
            line = line.substr(0, line.find('\n'));
            try {
                value = std::stof(trim(line.substr(line.find(':') + 1)));
            } catch (...) {
                value = 5.0f;  // 解析失败时使用默认值
            }
        }
    };
    
    parseDifficulty("HPDrainRate", beatmap->hpDrainRate);         // 血槽消耗速率
    parseDifficulty("CircleSize", beatmap->circleSize);           // 音符尺寸
    parseDifficulty("OverallDifficulty", beatmap->overallDifficulty); // 判定难度
    parseDifficulty("SliderMultiplier", beatmap->sliderMultiplier);   // 滑条速度
    
    // ==================== 解析 [TimingPoints] 节获取 BPM ====================
    beatmap->mainBpm = parseTimingPoints(getSection(content, "[TimingPoints]"));
    
    // ==================== 解析 [HitObjects] 节获取音符 ====================
    beatmap->notes = parseHitObjects(getSection(content, "[HitObjects]"), beatmap->circleSize);
    
    qDebug() << "Parsed beatmap:" << QString::fromStdString(beatmap->title) 
             << "-" << QString::fromStdString(beatmap->version)
             << "| BPM:" << beatmap->mainBpm
             << "| Notes:" << beatmap->notes.size();
    
    return beatmap;
}

/**
 * @brief 解析 TimingPoints 节并提取主要 BPM
 * @param content [TimingPoints] 节的内容
 * @return 主要 BPM 值
 * 
 * TimingPoint 格式: time,beatLength,meter,sampleSet,sampleIndex,volume,uninherited,effects
 * beatLength > 0 时表示每拍的毫秒数，BPM = 60000 / beatLength
 */
double OszParser::parseTimingPoints(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    double mainBpm = 120.0; // 默认 BPM
    
    while (std::getline(stream, line)) {
        line = trim(line);
        // 跳过空行、注释和节标记
        if (line.empty() || line[0] == '/' || line[0] == '[') continue;
        
        auto parts = split(line, ',');
        if (parts.size() >= 2) {
            try {
                double beatLength = std::stod(parts[1]);
                
                // beatLength > 0 表示这是一个真实的 timing point（非继承点）
                if (beatLength > 0) {
                    mainBpm = 60000.0 / beatLength; // 转换为 BPM
                    break; // 使用第一个有效的 timing point
                }
            } catch (...) {
                continue;
            }
        }
    }
    
    return mainBpm;
}

/**
 * @brief 解析 HitObjects 节中的音符数据
 * @param content [HitObjects] 节的内容
 * @param circleSize 音符尺寸参数（用于计算音符位置）
 * @return 解析出的音符对象列表
 * 
 * HitObject 格式: x,y,time,type,hitSound,objectParams,hitSample
 * type: 1=circle, 2=slider, 8=spinner, 128=hold
 * 将 X 坐标 (0-512) 映射到 4 个轨道 (1-4)
 */
std::vector<Note*> OszParser::parseHitObjects(const std::string& content, float circleSize) {
    std::vector<Note*> notes;
    std::istringstream stream(content);
    std::string line;
    
    // OSU 标准宽度和轨道配置
    const int laneCount = 4;              // 4 个轨道
    const float osuWidth = 512.0f;        // OSU 标准宽度
    const float laneWidth = osuWidth / laneCount; // 每个轨道的宽度
    
    int noteCount = 0;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '/' || line[0] == '[') continue;
        
        auto parts = split(line, ',');
        if (parts.size() < 4) continue;
        
        try {
            int x = std::stoi(parts[0]);            // X 坐标
            double timestamp = std::stod(parts[2]); // 时间戳（毫秒）
            int type = std::stoi(parts[3]);         // 音符类型
            
            // 将 X 坐标转换为轨道号 (1-4)
            int lane = static_cast<int>(x / laneWidth) + 1;
            if (lane < 1) lane = 1;
            if (lane > laneCount) lane = laneCount;
            
            // 过滤特殊类型：跳过 Spinner (旋转)
            if (type & 8) {
                continue;
            }
            
            // 所有音符（包括 circle, slider, hold）统一转换为 normal 类型
            notes.push_back(new Note(timestamp, lane, "normal"));
            noteCount++;
        } catch (const std::exception& e) {
            qDebug() << "Failed to parse HitObject:" << QString::fromStdString(line);
            continue;
        }
    }
    
    qDebug() << "Parsed" << noteCount << "normal notes from HitObjects";
    return notes;
}

/**
 * @brief 从 OsuBeatmap 创建 Song 对象
 * @param beatmap 已解析的谱面数据
 * @param audioPath 音频文件路径
 * @return 创建的 Song 对象
 */
Song* OszParser::createSongFromBeatmap(const OsuBeatmap* beatmap, const std::string& audioPath) {
    if (!beatmap) return nullptr;
    
    double bpm = beatmap->mainBpm;
    
    qDebug() << "Creating Song:" << QString::fromStdString(beatmap->title)
             << "| BPM:" << bpm;
    
    return new Song(
        beatmap->title,
        beatmap->artist,
        bpm
    );
}

/**
 * @brief 清理 OSZ 包的临时文件和内存
 * @param package 需要清理的 OszPackage 对象
 */
void OszParser::cleanup(OszPackage* package) {
    if (!package) return;
    
    qDebug() << "Cleaning up OSZ package...";
    
    // 删除临时解压目录及其所有文件
    QDir dir(QString::fromStdString(package->extractPath));
    if (dir.exists()) {
        dir.removeRecursively();
    }
    
    // 释放所有 beatmap 和 note 对象的内存
    for (auto beatmap : package->beatmaps) {
        for (auto note : beatmap->notes) {
            delete note;
        }
        delete beatmap;
    }
    
    delete package;
}

/**
 * @brief 从文件内容中提取指定节的内容
 * @param content 完整文件内容
 * @param sectionName 节名称（如 "[General]"）
 * @return 该节的内容字符串
 */
std::string OszParser::getSection(const std::string& content, const std::string& sectionName) {
    size_t start = content.find(sectionName);
    if (start == std::string::npos) return "";
    
    start += sectionName.length();
    size_t end = content.find("\n[", start); // 查找下一个节的开始
    
    if (end == std::string::npos) {
        return content.substr(start); // 如果是最后一个节，返回到文件末尾
    }
    
    return content.substr(start, end - start);
}

/**
 * @brief 去除字符串首尾的空白字符
 * @param str 原始字符串
 * @return 去除空白后的字符串
 */
std::string OszParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

/**
 * @brief 按指定分隔符分割字符串
 * @param str 原始字符串
 * @param delimiter 分隔符字符
 * @return 分割后的字符串数组（每个元素已 trim）
 */
std::vector<std::string> OszParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    
    return tokens;
}