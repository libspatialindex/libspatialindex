#include <stdio.h>
#include <spatialindex/capi/sidx_api.h>

int main(int argc, char *argv[]) {
    char* pszVersion = SIDX_Version();
    printf("%s\n", pszVersion);
    return(0);
}
