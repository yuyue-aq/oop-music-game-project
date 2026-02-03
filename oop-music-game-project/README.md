# RhythmMaster 音乐节奏游戏项目需求文档
## 一、项目概述
### 1.1 项目目标
开发一款音乐节奏游戏，玩家需要在正确的时机按下对应的按键来打击下落的音符，获得高分。

### 1.2 核心功能
- ✅ 音符随音乐节奏下落
- ✅ 实时判定玩家操作（Perfect/Great/Good/Miss）
- ✅ 分数计算和连击系统
- ✅ 音频播放与同步
- ✅ 游戏结果统计

### 1.3 项目特色
- 图形化界面（Qt Widgets）
- 包含多种算法（判定算法、音符管理、分数计算）
- 模块化设计，分工明确
- 可扩展性强
## 二、团队分工 
成员 A：张馨予
成员 B:孙逸
### 2.1 团队结构
| 成员       | 职责                          | 代码量要求   | 技术要求       |
|------------|-------------------------------|--------------|----------------|
| 成员 A     | 基础数据类（4个类）           | 350-400行    | C++基础        |
| 成员 B     | 配置和统计类（4个类）         | 350-400行    | C++基础        |
**总代码量**：约 1500-1800 行
## 三、项目结构
```
RhythmMaster/
│
├── ConsoleApplication4.cpp          # 主程序入口
│
├── Core/                            # 核心数据类（成员A/B）
│   ├── Note.h/cpp                  # 成员A - 音符类
│   ├── Song.h/cpp                  # 成员A - 歌曲类
│   ├── Player.h/cpp                # 成员A - 玩家类
│   ├── Beatmap.h/cpp               # 成员A - 谱面类
│   ├── GameConfig.h/cpp            # 成员B - 配置类
│   ├── HitResult.h/cpp             # 成员B - 判定结果类
│   ├── Statistics.h/cpp            # 成员B - 统计类
│   └── Leaderboard.h/cpp           # 成员B - 排行榜类
│
├── Game/                            # 游戏逻辑（主程序员）
│   ├── ScoreCalculator.h/cpp       # 分数计算算法
│   ├── NoteManager.h/cpp           # 音符管理算法
│   ├── AudioSync.h/cpp             # 音频同步
│   └── BeatmapLoader.h/cpp         # 谱面加载器
│
└── UI/                              # 界面（主程序员）
    ├── MainWindow.h/cpp            # 主窗口
    ├── GameScene.h/cpp             # 游戏场景
    ├── ResultDialog.h/cpp          # 结果界面
    └── SettingsDialog.h/cpp        # 设置界面
```

## 四、成员 A 的详细需求（350-400行）
### 4.1 负责模块
- ✅ Note 类 - 音符数据结构
- ✅ Song 类 - 歌曲信息
- ✅ Player 类 - 玩家数据
- ✅ Beatmap 类 - 谱面数据

### 4.2 Note 类需求（约 100 行）
#### 功能需求：
1. 存储音符的所有属性
2. 提供音符状态管理
3. 支持多种音符类型（单击、长按、滑动）
4. 计算音符位置和生命周期

#### 成员变量：
- int id                    // 音符ID
- double timestamp          // 出现时间
- int lane                  // 轨道（0-3）
- NoteType type             // 类型枚举
- NoteState state           // 状态枚举
- double holdDuration       // 长按时长
- double currentY           // 当前Y坐标
- double opacity            // 不透明度
- bool isSpecial            // 是否特殊音符

#### 方法需求：
```cpp
// 构造函数
Note(int id, double time, int lane, NoteType type);

// Getter方法（9个）
int getId() const;
double getTimestamp() const;
int getLane() const;
// ... 其他getter

// Setter方法（4个）
void setState(NoteState state);
void setCurrentY(double y);
// ... 其他setter

// 功能方法（6个）
void updatePosition(double currentTime, double speed, double judgementY);
bool isExpired(double currentTime, double missWindow) const;
bool isInHitWindow(double currentTime, double hitWindow) const;
void recordHit();
void reset();
Note* clone() const;
```

#### 验收标准：
```cpp
// 测试代码
Note note(1, 1.5, 2, Note::NoteType::TAP);
assert(note.getId() == 1);
assert(note.getTimestamp() == 1.5);
assert(note.getLane() == 2);
```

### 4.3 Song 类需求（约 120 行）
#### 功能需求：
1. 存储歌曲完整元数据
2. 管理难度信息
3. 记录游玩统计数据
4. 支持标签系统

#### 成员变量：
```cpp
// 基础信息（7个）
- int songId
- string title
- string artist
- string album
- Genre genre              // 枚举：POP/ROCK/等
- string audioPath
- string coverImagePath

// 音乐属性（5个）
- double duration
- double bpm
- int offset
- double previewStart
- double previewDuration

// 难度信息（4个）
- Difficulty difficulty    // 枚举：EASY/NORMAL/等
- int noteCount
- int maxCombo
- double starRating

// 统计数据（5个）
- int playCount
- int clearCount
- int fullComboCount
- int highScore
- double bestAccuracy

// 附加信息（4个）
- string composer
- string charter
- bool isUnlocked
- bool isFavorite
```

#### 方法需求：
```cpp
// 构造函数（2个）
Song(基础参数);
Song(完整参数);

// Getter方法（26个）
int getSongId() const;
string getTitle() const;
// ... 所有成员变量的getter

// Setter方法（15个）
void setTitle(const string& title);
void setHighScore(int score);
// ... 可修改属性的setter

// 功能方法（10个）
void incrementPlayCount();
void incrementClearCount();
bool updateHighScore(int newScore);
bool updateBestAccuracy(double accuracy);
void addTag(const string& tag);
void removeTag(const string& tag);
bool hasTag(const string& tag) const;
double getClearRate() const;
string getFormattedDuration() const;
void resetStatistics();
```

#### 验收标准：
```cpp
Song song(1, "Test Song", "Artist", "path.mp3", 
          Song::Difficulty::NORMAL, 180.0, 120.0);
assert(song.getTitle() == "Test Song");
song.incrementPlayCount();
assert(song.getPlayCount() == 1);
```

### 4.4 Player 类需求（约 80 行）
#### 功能需求：
1. 存储玩家信息
2. 管理当前游戏状态
3. 记录判定统计
4. 计算准确率和等级

#### 成员变量：
- string name              // 玩家名称
- int totalScore           // 总分
- int combo                // 当前连击
- int maxCombo             // 最高连击
- int perfectCount         // Perfect次数
- int greatCount           // Great次数
- int goodCount            // Good次数
- int missCount            // Miss次数
- int totalNotes           // 总音符数
- double accuracy          // 准确率

#### 方法需求：
```cpp
// 构造函数
Player(string name);

// Getter方法（10个）
string getName() const;
int getTotalScore() const;
// ... 所有getter

// Setter方法（3个）
void setTotalScore(int score);
void setCombo(int combo);
void setMaxCombo(int max);

// 判定统计方法（4个）
void addPerfect();
void addGreat();
void addGood();
void addMiss();

// 计算方法（3个）
double calculateAccuracy() const;
string getRank() const;        // S/A/B/C/D
int calculateFinalScore() const;

// 其他（1个）
void reset();
```

#### 验收标准：
```cpp
Player player("Alice");
player.addPerfect();
player.addPerfect();
player.addGreat();
assert(player.getPerfectCount() == 2);
assert(player.calculateAccuracy() > 0.8);
```

### 4.5 Beatmap 类需求（约 100 行）
#### 功能需求：
1. 存储谱面数据
2. 管理音符集合
3. 提供音符查询功能
4. 计算谱面难度

#### 成员变量：
- int beatmapId
- string beatmapName
- string songTitle
- string charter
- int difficulty
- vector<Note*> notes      // 所有音符
- double totalDuration
- int noteCount
- int maxCombo
- double density           // 音符密度

#### 方法需求：
```cpp
// 构造函数
Beatmap(int id, string name);

// 音符管理（6个）
void addNote(Note* note);
void removeNote(int noteId);
Note* getNoteById(int id) const;
vector<Note*> getAllNotes() const;
vector<Note*> getNotesInTimeRange(double start, double end) const;
void sortNotesByTime();

// 统计方法（5个）
int getNoteCount() const;
int getMaxCombo() const;
double getDensity() const;          // 每秒音符数
double getAverageBPM() const;
int getDifficulty() const;

// 查询方法（3个）
Note* getNextNote(double currentTime) const;
vector<Note*> getNotesAtTime(double time, double window) const;
bool hasMoreNotes(double currentTime) const;

// 其他（2个）
void clear();
Beatmap* clone() const;
```

#### 验收标准：
```cpp
Beatmap beatmap(1, "Test Map");
Note* note1 = new Note(1, 1.0, 0, Note::NoteType::TAP);
Note* note2 = new Note(2, 2.0, 1, Note::NoteType::TAP);
beatmap.addNote(note1);
beatmap.addNote(note2);
assert(beatmap.getNoteCount() == 2);
```

## 五、成员 B 的详细需求（350-400行）
### 5.1 负责模块
- ✅ GameConfig 类 - 游戏配置
- ✅ HitResult 类 - 判定结果
- ✅ Statistics 类 - 游戏统计
- ✅ Leaderboard 类 - 排行榜

### 5.2 GameConfig 类需求（约 120 行）
#### 功能需求：
1. 存储所有游戏配置参数
2. 提供配置验证
3. 支持配置保存和加载
4. 提供默认配置

#### 成员变量：
```cpp
// 游戏参数（5个）
- double noteSpeed              // 音符速度
- double judgementLineY         // 判定线位置
- double visualOffset           // 视觉偏移
- double audioOffset            // 音频偏移
- bool autoPlay                 // 自动游玩

// 判定窗口（3个，毫秒）
- double perfectWindow
- double greatWindow
- double goodWindow

// 分数设置（4个）
- int perfectScore
- int greatScore
- int goodScore
- int comboMultiplier

// 音量设置（3个，0.0-1.0）
- double volumeMusic
- double volumeEffect
- double volumeMaster

// 视觉设置（4个）
- bool showFPS
- bool showCombo
- bool showAccuracy
- int noteSkin                  // 音符皮肤ID

// 键位设置（4个）
- int keyLane0                  // 默认D
- int keyLane1                  // 默认F
- int keyLane2                  // 默认J
- int keyLane3                  // 默认K
```

#### 方法需求：
```cpp
// 构造函数
GameConfig();                   // 使用默认值

// Getter方法（23个）
double getNoteSpeed() const;
// ... 所有getter

// Setter方法（23个，带验证）
void setNoteSpeed(double speed);        // 验证 > 0
void setVolumeMusic(double vol);        // 验证 0.0-1.0
void setPerfectWindow(double ms);       // 验证 > 0
// ... 所有setter

// 配置管理（4个）
bool saveToFile(const string& filepath) const;
bool loadFromFile(const string& filepath);
void resetToDefault();
bool isValid() const;                   // 验证所有参数

// 辅助方法（3个）
string toString() const;                // 转为字符串
void copyFrom(const GameConfig& other);
bool equals(const GameConfig& other) const;
```

#### 验收标准：
```cpp
GameConfig config;
assert(config.getNoteSpeed() == 500.0);    // 默认值
config.setNoteSpeed(600.0);
assert(config.getNoteSpeed() == 600.0);
config.setVolumeMusic(1.5);                // 无效值
assert(config.getVolumeMusic() <= 1.0);    // 应保持有效范围
```

### 5.3 HitResult 类需求（约 60 行）
#### 功能需求：
1. 记录单次打击结果
2. 存储判定详细信息
3. 计算得分

#### 成员变量：
- string judgement             // "Perfect"/"Great"/"Good"/"Miss"
- int score                    // 本次得分
- double timeDifference        // 时间差（ms）
- int comboAtHit               // 打击时的连击数
- bool isSpecialNote           // 是否特殊音符
- double accuracy              // 单次准确度（0.0-1.0）
- int noteId                   // 相关音符ID

#### 方法需求：
```cpp
// 构造函数（2个）
HitResult(string judge, int score, double timeDiff);
HitResult(string judge, int score, double timeDiff, 
          int combo, bool special);

// Getter方法（7个）
string getJudgement() const;
int getScore() const;
// ... 所有getter

// 判定方法（4个）
bool isPerfect() const;
bool isGreat() const;
bool isGood() const;
bool isMiss() const;

// 其他（2个）
string toString() const;
double getAccuracy() const;
```

### 5.4 Statistics 类需求（约 100 行）
#### 功能需求：
1. 记录完整游戏统计
2. 计算各种指标
3. 生成统计报告

#### 成员变量：
```cpp
// 基础统计（8个）
- int totalNotes
- int notesHit
- int notesMissed
- int perfectCount
- int greatCount
- int goodCount
- int missCount
- int maxCombo

// 分数统计（3个）
- int totalScore
- int baseScore
- int comboBonus

// 时间统计（3个）
- double playTime              // 游玩时长
- double accuracy              // 准确率
- double completionRate        // 完成率

// 判定分布（4个）
- double earlyHitRate          // 早打率
- double lateHitRate           // 晚打率
- double averageTimeDiff       // 平均时间差
- double standardDeviation     // 标准差
```

#### 方法需求
```cpp
// 构造函数
Statistics();

// Getter方法（18个）
int getTotalNotes() const;
// ... 所有getter

// 更新方法（5个）
void addHitResult(const HitResult& result);
void updateMaxCombo(int combo);
void setPlayTime(double time);
void incrementMiss();
void calculateFinalStats();

// 计算方法（7个）
double calculateAccuracy() const;
double calculateCompletionRate() const;
string getRank() const;                    // S/A/B/C/D/F
int calculateFinalScore() const;
double getAverageTimeDifference() const;
double getHitRate() const;
bool isFullCombo() const;

// 报告方法（2个）
string generateReport() const;
void reset();
```

#### 验收标准：
```cpp
Statistics stats;
HitResult hit1("Perfect", 300, 10.0);
HitResult hit2("Great", 100, 50.0);
stats.addHitResult(hit1);
stats.addHitResult(hit2);
stats.calculateFinalStats();
assert(stats.getPerfectCount() == 1);
assert(stats.getAccuracy() > 0.0);
```

### 5.5 Leaderboard 类需求（约 100 行）
#### 功能需求：
1. 存储排行榜数据
2. 管理分数记录
3. 支持排序和查询
4. 持久化存储

#### 内部类 ScoreEntry：
```cpp
struct ScoreEntry {
    string playerName;
    int score;
    double accuracy;
    int maxCombo;
    string date;
    int rank;
    
    // 构造函数和比较运算符
    ScoreEntry(string name, int score, double acc, int combo);
    bool operator<(const ScoreEntry& other) const;
    bool operator>(const ScoreEntry& other) const;
};
```

#### 成员变量：
- vector<ScoreEntry> entries   // 所有记录
- int maxEntries               // 最大记录数（如100）
- string songId                // 关联歌曲ID

#### 方法需求：
```cpp
// 构造函数
Leaderboard(string songId, int maxEntries = 100);

// 记录管理（6个）
bool addEntry(const ScoreEntry& entry);
bool removeEntry(int index);
void clear();
int getEntryCount() const;
ScoreEntry getEntry(int index) const;
vector<ScoreEntry> getAllEntries() const;

// 排序和查询（6个）
void sortByScore();
void sortByAccuracy();
void sortByCombo();
vector<ScoreEntry> getTopN(int n) const;
int findPlayerRank(const string& playerName) const;
ScoreEntry getHighestScore() const;

// 统计方法（4个）
double getAverageScore() const;
double getAverageAccuracy() const;
int getHighestCombo() const;
bool isFull() const;

// 持久化（2个）
bool saveToFile(const string& filepath) const;
bool loadFromFile(const string& filepath);

// 其他（2个）
string toString() const;
void updateRanks();                // 重新计算排名
```

#### 验收标准：
```cpp
Leaderboard board("song_001", 10);
ScoreEntry entry1("Alice", 95000, 0.98, 500);
ScoreEntry entry2("Bob", 92000, 0.95, 450);
board.addEntry(entry1);
board.addEntry(entry2);
board.sortByScore();
assert(board.getEntry(0).playerName == "Alice");
assert(board.findPlayerRank("Alice") == 1);
```

## 六、主程序员的详细需求（800-1000行）
### 6.1 负责模块
#### Game/ 目录 - 算法模块（约 400 行）
1. ScoreCalculator - 分数计算算法（100行）
   - 判定算法（Perfect/Great/Good/Miss）
   - Combo加成计算
   - 准确率计算
   - 难度系数计算
2. NoteManager - 音符管理算法（120行）
   - 音符生命周期管理
   - 活跃音符更新
   - 最近音符查找算法
   - 碰撞检测
3. AudioSync - 音频同步（80行）
   - 音频播放控制
   - 时间同步算法
   - 偏移处理
4. BeatmapLoader - 谱面加载器（100行）
   - 文件解析
   - 数据验证
   - 错误处理

#### UI/ 目录 - 界面模块（约 500 行）
1. MainWindow - 主窗口（150行）
   - 菜单界面
   - 歌曲选择
   - 设置入口
2. GameScene - 游戏场景（250行）
   - 音符绘制
   - 判定线绘制
   - 特效渲染
   - 键盘事件处理
   - 游戏循环
3. ResultDialog - 结果界面（50行）
   - 统计显示
   - 排行榜显示
4. SettingsDialog - 设置界面（50行）
   - 配置修改
   - 键位设置

## 七、验收标准
### 7.1 成员 A 验收标准
- [ ] 所有类编译通过
- [ ] 所有 getter/setter 方法正确
- [ ] 功能方法逻辑正确
- [ ] 代码行数达标（350-400行）
- [ ] 添加清晰注释

### 7.2 成员 B 验收标准
- [ ] 所有类编译通过
- [ ] 配置验证逻辑正确
- [ ] 统计计算准确
- [ ] 代码行数达标（350-400行）
- [ ] 添加清晰注释

### 7.3 主程序员验收标准
- [ ] 项目能正常编译运行
- [ ] 游戏核心功能完整
- [ ] 音频同步正常
- [ ] 判定算法准确
- [ ] 界面美观流畅

### 7.4 项目整体验收
- [ ] 能启动游戏并选择歌曲
- [ ] 音符能随音乐下落
- [ ] 按键判定正确
- [ ] 分数计算准确
- [ ] 显示游戏结果
- [ ] 有完整的 Git 提交记录

## 八、协作规范
### 9.1 代码规范
- 类名：大驼峰（PascalCase）
- 函数名：小驼峰（camelCase）
- 变量名：小驼峰
- 常量：全大写+下划线

### 9.2 注释规范
```cpp
/**
 * @brief 简要描述
 * @param 参数说明
 * @return 返回值说明
 */
```

### 9.3 提交规范
```
[模块名] 功能描述
例如：[Note] 完成音符类的所有方法
```