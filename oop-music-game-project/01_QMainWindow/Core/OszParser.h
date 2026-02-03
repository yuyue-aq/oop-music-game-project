//@余越
#ifndef OSZPARSER_H
#define OSZPARSER_H

#include <string>
#include <vector>
#include <map>
#include "Note.h"
#include "Song.h"

// OSU 谱面数据结构
struct OsuBeatmap {
    // General
    std::string audioFilename;
    int previewTime;
    
    // Metadata
    std::string title;
    std::string artist;
    std::string creator;
    std::string version;
    
    // Difficulty
    float hpDrainRate;
    float circleSize;
    float overallDifficulty;
    float sliderMultiplier;
    
    // TimingPoints - 改进：存储 BPM 值
    std::vector<double> timingPoints;
    double mainBpm;  // 添加：主要 BPM
    
    // HitObjects (notes)
    std::vector<Note*> notes;
    
    // 构造函数，初始化默认值
    OsuBeatmap() : previewTime(0), hpDrainRate(5.0f), circleSize(4.0f), 
                   overallDifficulty(5.0f),
                   sliderMultiplier(1.4f), mainBpm(120.0) {}
};

// OSZ 包结构
struct OszPackage {
    std::string packagePath;        // OSZ 文件路径
    std::string extractPath;        // 解压目录
    std::string audioFilePath;      // 音频文件路径
    std::vector<OsuBeatmap*> beatmaps; // 包含的所有难度谱面
};

class OszParser {
public:
    // 解析 OSZ 包
    static OszPackage* parseOszFile(const std::string& oszPath);
    
    // 从 OsuBeatmap 创建 Song 对象
    static Song* createSongFromBeatmap(const OsuBeatmap* beatmap, const std::string& audioPath);
    
    // 清理临时文件
    static void cleanup(OszPackage* package);
    
private:
    // 解压 OSZ 文件到临时目录
    static bool extractOszFile(const std::string& oszPath, const std::string& destPath);
    
    // 解析 .osu 文件
    static OsuBeatmap* parseOsuFile(const std::string& osuFilePath);
    
    // 解析 HitObjects 部分
    static std::vector<Note*> parseHitObjects(const std::string& content, float circleSize);
    
    // 解析时间点（改进：返回 BPM）
    static double parseTimingPoints(const std::string& content);
    
    // 获取节(Section)内容
    static std::string getSection(const std::string& content, const std::string& sectionName);
    
    // Trim 字符串
    static std::string trim(const std::string& str);
    
    // 分割字符串
    static std::vector<std::string> split(const std::string& str, char delimiter);
};

#endif // OSZPARSER_H