#ifndef SONG_H
#define SONG_H

#include <string>

class Song {
private:
    std::string title;     // 歌曲标题
    std::string artist;    // 艺术家/作曲者
    double bpm;            // 歌曲节拍数（每分钟）

public:
    // 构造函数
    Song(std::string t, std::string a, double b);
    
    // Getter方法
    std::string getTitle() const { return title; }
    std::string getArtist() const { return artist; }
    double getBpm() const { return bpm; }
};

#endif // SONG_H#endif // SONG_H