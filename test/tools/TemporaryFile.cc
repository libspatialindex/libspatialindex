#include <spatialindex/SpatialIndex.h>
#include <spatialindex/tools/Tools.h>

using namespace SpatialIndex;
using namespace std;

/* 
 * Test the TemporaryFile
 */
int main(int /*argc*/, char** /*argv*/) {
    Tools::TemporaryFile temporaryFile;

    const std::string path = temporaryFile.getFileName();

#if defined ( _MSC_VER )
    const std::string shouldContain = "Temp";
    if (path.find(shouldContain) == std::string::npos) {
        cerr << "Test failed:  TemporaryFile path " << path << " does not contain " << shouldContain << endl;
        return -1;
    }
#else
    const std::string lnxShouldContain = "/tmp/spatialindex-";
    const std::string macShouldContain = "/var/folders/";
    if ( (path.find(lnxShouldContain) == std::string::npos) &&
       (path.find(macShouldContain) == std::string::npos) )
    {
        cerr << "Test failed:  TemporaryFile path " << path << " does not contain " << lnxShouldContain << " nor " << macShouldContain << endl;
        return -1;
    }
#endif

    return 0;
}
