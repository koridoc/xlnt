// Copyright (c) 2014-2022 Thomas Fussell
// Copyright (c) 2024 xlnt-community
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

#include <xlnt/styles/protection.hpp>
#include <helpers/test_suite.hpp>
#include <xlnt/xlnt.hpp>

class protection_test_suite : public test_suite
{
public:
    protection_test_suite()
    {
        register_test(test_all);
    }

    void test_all()
    {
        auto prot = xlnt::protection::unlocked_and_visible();
        xlnt_assert(!prot.hidden());
        xlnt_assert(!prot.locked());

        prot = xlnt::protection::locked_and_visible();
        xlnt_assert(!prot.hidden());
        xlnt_assert(prot.locked());

        prot = xlnt::protection::unlocked_and_hidden();
        xlnt_assert(prot.hidden());
        xlnt_assert(!prot.locked());

        prot = xlnt::protection::locked_and_hidden();
        xlnt_assert(prot.hidden());
        xlnt_assert(prot.locked());
    }
};
static protection_test_suite x;
