//@张馨予
#ifndef NOTE_H
#define NOTE_H

#include <string>

class Note {
private:
    double timestamp;      // 音符出现的时间戳（毫秒）
    int lane;              // 音符所在轨道（1-N）
    std::string noteType;  // 音符类型（如"normal"/"hold"/"slide"）
    bool isHit;            // 音符是否被成功打击
    int id;
public:
    // 构造函数
    Note(double t, int l, std::string type);
    virtual Note* clone() const;
    // Getter方法
    double getTimestamp() const { return timestamp; }
    int getLane() const { return lane; }
    std::string getNoteType() const { return noteType; }
    bool getIsHit() const { return isHit; }
    int getId() const;

    // Setter方法
    void setIsHit(bool hit) { isHit = hit; }
};

#endif // NOTE_H