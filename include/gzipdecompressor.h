/**********************************************
*
* Copyright 2021 Cyril Selyanin
* cyr.selyanin@gmail.com
* https://github.com/larsnoches/web-crawler
*
*/

#ifndef GZIPDECOMPRESSOR_H
#define GZIPDECOMPRESSOR_H

#include <zlib.h>

#include <limits>
#include <stdexcept>
#include <string>

class GzipDecompressor
{
    std::size_t max_;

public:
    // by default refuse operation if compressed data is > 1GB
    GzipDecompressor(std::size_t max_bytes = 1000000000);

    void decompress(std::string& output, const char* data, std::size_t size) const;
};

#endif // GZIPDECOMPRESSOR_H
