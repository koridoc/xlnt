// Copyright (c) 2016-2022 Thomas Fussell
// Copyright (c) 2024-2025 xlnt-community
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE
//
// @license: http://www.opensource.org/licenses/mit-license.php
// @author: see AUTHORS file

#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include <detail/xlnt_config_impl.hpp>

namespace xlnt {
namespace detail {

/// <summary>
/// Allows a std::vector to be read through a std::istream.
/// </summary>
class XLNT_API_INTERNAL vector_istreambuf : public std::streambuf
{
    using int_type = std::streambuf::int_type;

public:
    explicit vector_istreambuf(const std::vector<std::uint8_t> &data);

    vector_istreambuf(const vector_istreambuf &) = delete;
    vector_istreambuf &operator=(const vector_istreambuf &) = delete;

private:
    int_type underflow() override;

    int_type uflow() override;

    std::streamsize showmanyc() override;

    std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode) override;

    std::streampos seekpos(std::streampos sp, std::ios_base::openmode) override;

private:
    const std::vector<std::uint8_t> &data_;
    std::size_t position_;
};

/// <summary>
/// Allows a std::vector to be written through a std::ostream.
/// </summary>
class XLNT_API_INTERNAL vector_ostreambuf : public std::streambuf
{
    using int_type = std::streambuf::int_type;

public:
    explicit vector_ostreambuf(std::vector<std::uint8_t> &data);

    vector_ostreambuf(const vector_ostreambuf &) = delete;
    vector_ostreambuf &operator=(const vector_ostreambuf &) = delete;

private:
    int_type overflow(int_type c) override;

    std::streamsize xsputn(const char *s, std::streamsize n) override;

    std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode) override;

    std::streampos seekpos(std::streampos sp, std::ios_base::openmode) override;

private:
    std::vector<std::uint8_t> &data_;
    std::size_t position_;
};

/// <summary>
/// Helper function to read all data from in_stream and store them in a vector.
/// </summary>
XLNT_API_INTERNAL std::vector<std::uint8_t> to_vector(std::istream &in_stream);

/// <summary>
/// Helper function to write all data from bytes into out_stream.
/// </summary>
XLNT_API_INTERNAL void to_stream(const std::vector<std::uint8_t> &bytes, std::ostream &out_stream);

/// <summary>
/// Shortcut function to stream a vector of bytes into a std::ostream.
/// </summary>
XLNT_API_INTERNAL std::ostream &operator<<(std::ostream &out_stream, const std::vector<std::uint8_t> &bytes);

} // namespace detail
} // namespace xlnt
