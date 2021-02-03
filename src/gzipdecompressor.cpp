#include "gzipdecompressor.h"

GzipDecompressor::GzipDecompressor(std::size_t max_bytes)
    : max_(max_bytes)
{
    //
}

void GzipDecompressor::decompress(std::string& output, const char* data, std::size_t size) const
{
    z_stream inflate_s;

    inflate_s.zalloc = Z_NULL;
    inflate_s.zfree = Z_NULL;
    inflate_s.opaque = Z_NULL;
    inflate_s.avail_in = 0;
    inflate_s.next_in = Z_NULL;

    if (inflateInit2(&inflate_s, MAX_WBITS | 16) != Z_OK)
    {
        throw std::runtime_error("Inflate init failed");
    }

    inflate_s.next_in = (Bytef*)data;
    if (size > max_ || (size * 2) > max_)
    {
        inflateEnd(&inflate_s);
        throw std::runtime_error("Size may use more memory than intended when decompressing");
    }
    inflate_s.avail_in = static_cast<unsigned int>(size);
    std::size_t size_uncompressed = 0;
    do
    {
        std::size_t resize_to = size_uncompressed + 2 * size;
        if (resize_to > max_)
        {
            inflateEnd(&inflate_s);
            throw std::runtime_error(
                "Size of output string will use more memory then intended when decompressing");
        }
        output.resize(resize_to);
        inflate_s.avail_out = static_cast<unsigned int>(2 * size);
        inflate_s.next_out = reinterpret_cast<Bytef*>(&output[0] + size_uncompressed);

        int ret = inflate(&inflate_s, Z_FINISH);
        if (ret != Z_STREAM_END && ret != Z_OK && ret != Z_BUF_ERROR)
        {
            std::string error_msg = inflate_s.msg;
            inflateEnd(&inflate_s);
            throw std::runtime_error(error_msg);
        }

        size_uncompressed += (2 * size - inflate_s.avail_out);
    }
    while (inflate_s.avail_out == 0);
    inflateEnd(&inflate_s);
    output.resize(size_uncompressed);
}
