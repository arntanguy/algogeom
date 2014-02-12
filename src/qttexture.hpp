#ifndef __qttexture__
#define __qttexture__

#include "basetexture.hpp"

class QImage;

namespace cg
{

class QtTexture : public BaseTexture
{
public:
    QtTexture();
    QtTexture(const std::string& path);
    bool load(const QImage& path);
};

}

#endif
