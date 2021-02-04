set( NAME Tuto17SimpleAR )
set( VERSION 0.5 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_io_base             # Start the module, load file location or window module_io_base
    module_ui_qt                   # Start the module, load module_ui_qt implementation of module_ui_base
    module_viz_ogre                # Start the module, allow to use viz_ogre
    module_viz_ogreQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_ui_base
    module_ui_flaticon

    # Reader
    module_io_file
    module_io_vtk
    module_io_atoms

    # Grabber
    videoQt

    # Services
    ioCalibration
    ctrlCamp
    videoTools
    trackerAruco
    registrationCV
    videoCalibration
    maths
    ctrlCom

    # Generic Scene
)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        module_ui_qt-0.1/flatdark.rcc
        module_ui_qt-0.1/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto17SimpleAR_AppCfg
) # Main application's configuration to launch
