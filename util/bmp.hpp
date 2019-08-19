#ifndef BMP_HPP_
#define BMP_HPP_

class BMPImage
{
public:
    BMPImage(const char* filepath);
    ~BMPImage() { if (_data) { delete[] _data; } }

    inline size_t width() const { return _width; }
    inline size_t height() const { return _height; }
    inline size_t length() const { return _width * _height * 3; }
    inline unsigned char* data() const { return _data; }

private:
    unsigned char* _data;
    size_t _width;
    size_t _height;
};

class DDSTexture
{
public:
    DDSTexture(const char* filepath);
    ~DDSTexture() { if (_data) { delete[] _data; } }

    inline size_t width() const { return _width; }
    inline size_t height() const { return _height; }
    inline size_t nmap() const { return _nmap; }
    inline size_t format() const { return _format; }
    inline unsigned char* data() const { return _data; }

private:
    unsigned char* _data;
    size_t _width;
    size_t _height;
    size_t _nmap;

    unsigned int _format;
};

#endif
