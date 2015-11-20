/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

struct ContextGLTest: AbstractOpenGLTester {
    explicit ContextGLTest();

    void constructCopyMove();

    void isVersionSupported();
    void supportedVersion();
    void isExtensionSupported();
    void isExtensionDisabled();
};

ContextGLTest::ContextGLTest() {
    addTests({&ContextGLTest::constructCopyMove,

              &ContextGLTest::isVersionSupported,
              &ContextGLTest::supportedVersion,
              &ContextGLTest::isExtensionSupported,
              &ContextGLTest::isExtensionDisabled});
}

void ContextGLTest::constructCopyMove() {
    /* Only move-construction allowed */
    CORRADE_VERIFY(!(std::is_constructible<Context, const Context&>{}));
    CORRADE_VERIFY((std::is_constructible<Context, Context&&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Context, const Context&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Context, Context&&>{}));
}

void ContextGLTest::isVersionSupported() {
    const Version v = Context::current()->version();
    CORRADE_VERIFY(Context::current()->isVersionSupported(v));
    CORRADE_VERIFY(Context::current()->isVersionSupported(Version(Int(v)-1)));
    CORRADE_VERIFY(!Context::current()->isVersionSupported(Version(Int(v)+1)));

    /* No assertions should be fired */
    MAGNUM_ASSERT_VERSION_SUPPORTED(v);
    MAGNUM_ASSERT_VERSION_SUPPORTED(Version(Int(v)-1));
}

void ContextGLTest::supportedVersion() {
    const Version v = Context::current()->version();

    /* Selects first supported version (thus not necessarily the highest) */
    CORRADE_VERIFY(Context::current()->supportedVersion({Version(Int(v)+1), v, Version(Int(v)-1)}) == v);
    CORRADE_VERIFY(Context::current()->supportedVersion({Version(Int(v)+1), Version(Int(v)-1), v}) == Version(Int(v)-1));
}

void ContextGLTest::isExtensionSupported() {
    #ifndef MAGNUM_TARGET_GLES
    if(Context::current()->isExtensionSupported<Extensions::GL::GREMEDY::string_marker>())
        CORRADE_SKIP(Extensions::GL::GREMEDY::string_marker::string() + std::string(" extension should not be supported, can't test"));

    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_filter_anisotropic>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_filter_anisotropic::string() + std::string(" extension should be supported, can't test"));

    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>())
        CORRADE_SKIP(Extensions::GL::ARB::explicit_attrib_location::string() + std::string(" extension should be supported, can't test"));

    /* Test that we have proper extension list parser */
    std::string extensions(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
    CORRADE_VERIFY(extensions.find(Extensions::GL::EXT::texture_filter_anisotropic::string()) != std::string::npos);
    CORRADE_VERIFY(extensions.find(Extensions::GL::GREMEDY::string_marker::string()) == std::string::npos);

    /* This is disabled in GL < 3.2 to work around GLSL compiler bugs */
    CORRADE_VERIFY(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>(Version::GL310));
    CORRADE_VERIFY(Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>(Version::GL320));
    #else
    CORRADE_SKIP("No useful extensions to test on OpenGL ES");
    #endif
}

void ContextGLTest::isExtensionDisabled() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::GL::ARB::vertex_array_object::string() + std::string(" extension should be supported, can't test"));

    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>())
        CORRADE_SKIP(Extensions::GL::ARB::explicit_attrib_location::string() + std::string(" extension should be supported, can't test"));

    /* This is not disabled anywhere */
    CORRADE_VERIFY(!Context::current()->isExtensionDisabled<Extensions::GL::ARB::vertex_array_object>());

    /* This is disabled in GL < 3.2 to work around GLSL compiler bugs */
    CORRADE_VERIFY(Context::current()->isExtensionDisabled<Extensions::GL::ARB::explicit_attrib_location>(Version::GL310));
    CORRADE_VERIFY(!Context::current()->isExtensionDisabled<Extensions::GL::ARB::explicit_attrib_location>(Version::GL320));
    #else
    CORRADE_SKIP("No useful extensions to test on OpenGL ES");
    #endif
}

}}

MAGNUM_GL_TEST_MAIN(Magnum::Test::ContextGLTest)
