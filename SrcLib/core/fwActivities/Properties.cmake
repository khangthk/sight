
set( NAME fwActivities )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwCore fwData fwDataCamp fwMath fwMedData fwRuntime fwTools )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    boost/1.67.0@fw4spl/stable
)