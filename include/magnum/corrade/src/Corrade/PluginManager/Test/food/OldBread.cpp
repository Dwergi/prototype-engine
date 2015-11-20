/*
    This file is part of Corrade.

    Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "../AbstractFood.h"

namespace Corrade { namespace PluginManager { namespace Test {

class OldBread: public AbstractFood {
    public:
        explicit OldBread(AbstractManager& manager, std::string plugin): AbstractFood(manager, std::move(plugin)) {}

        bool isTasty() { return false; }
        int weight() { return 50; }
};

}}}

/* Wrong plugin version */
#undef CORRADE_PLUGIN_VERSION
#define CORRADE_PLUGIN_VERSION 0
CORRADE_PLUGIN_REGISTER(OldBread, Corrade::PluginManager::Test::OldBread,
    "cz.mosra.Corrade.PluginManager.Test.AbstractFood/1.0")
