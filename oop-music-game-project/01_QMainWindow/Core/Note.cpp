//@张馨予
#include "Note.h"
// ==================== 构造函数与克隆方法实现 ====================
Note::Note(double t, int l, std::string type)
    : timestamp(t), lane(l), noteType(type), isHit(false)
{
}
Note* Note::clone() const
{
    return new Note(*this);
}
int Note::getId() const {
    return id;
} 