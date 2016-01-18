///
/// \mainpage APK Icon Editor
///
/// \section Summary
/// - APK Icon Editor is a free, open-source and cross-platform tool designed to
///   easily change APK resources such as icons, strings, images,
///   application name and version, etc.
///
/// \section Details
/// - Written in: C++/Qt
/// - Source code: [GitHub](https://github.com/kefir500/apk-icon-editor)
/// - License: [GNU GPL v3.0](http://www.gnu.org/licenses/gpl-3.0.html)
///

#include "application.h"

int main(int argc, char *argv[])
{
    Application application(argc, argv);
    return application.exec();
}
