#include <stdio.h>
#include <iostream>
#include "bmp.hpp"

BMPImage::BMPImage(const char* filepath) :
    _data(nullptr), _width(0), _height(0)
{
    FILE* file = fopen(filepath,"rb");
    if (!file)
    {
        printf("Image could not be opened\n");
        return;
    }

    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header

    if (fread(header, 1, 54, file) != 54)
    { // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        fclose(file);
        return;
    }

    if (header[0]!='B' || header[1]!='M')
    {
        printf("Not a correct BMP file\n");
        fclose(file);
        return;
    }

    // Position in the file where the actual data begins
    unsigned int dataPos = *(int*)&(header[0x0A]);
    unsigned int imageSize = *(int*)&(header[0x22]);
    _width = *(int*)&(header[0x12]);
    _height = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0) { imageSize = _width * _height * 3; } // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0) { dataPos = 54; } // The BMP header is done that way

    // Create a buffer
    _data = new unsigned char[imageSize];

    // Read the actual data from the file into the buffer
    fread(_data, sizeof (unsigned char), imageSize, file);

    //Everything is in memory now, the file can be closed
    fclose(file);

}

DDSTexture::DDSTexture(const char* filepath) :
    _data(nullptr), _width(0), _height(0), _nmap(0), _format(0)
{
    unsigned char header[124];

    FILE *fp = fopen(filepath, "rb");

    if (fp == NULL) { return; }

    /* verify the type of file: fc -> filecode */
    char fc[4];
    fread(fc, 4, sizeof (char), fp);
    if (fc[0] != 'D' || fc[1] != 'D' || fc[2] != 'S' || fc[3] != ' ')
    {
        fclose(fp);
        return;
    }

    /* get the surface desc */
    fread(&header, 124, sizeof (unsigned char), fp);

    _height = *(unsigned int*)&(header[8 ]);
    _width = *(unsigned int*)&(header[12]);

    unsigned int linearSize = *(unsigned int*)&(header[16]);

    _nmap = *(unsigned int*)&(header[24]);
    _format = *(unsigned int*)&(header[80]);

    /* how big is it going to be including all mipmaps? */
    unsigned int length = _nmap > 1 ? linearSize * 2 : linearSize;
    _data = (unsigned char*)malloc(length * sizeof(unsigned char));

    fread(_data, sizeof(unsigned char), length, fp);
    fclose(fp);

    // _components = (fourCC == FOURCC_DXT1) ? 3 : 4;

    // switch(fourCC)
    // {
    // case FOURCC_DXT1:
    //     _format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    //     break;
    // case FOURCC_DXT3:
    //     _format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    //     break;
    // case FOURCC_DXT5:
    //     _format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    //     break;
    // default:
    //     _format = 0;
    //     return;
    // }
}
