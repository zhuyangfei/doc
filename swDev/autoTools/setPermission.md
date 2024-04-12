# set file permission
Permission for compilation artifacts should be set in Makefile, so permission will not be affectd by different kind of package tools.

## set in Cmake
install(FILES
        HWR/Startup/run.sh
        DESTINATION bin/aiml
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

## set in automake
