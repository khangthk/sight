set(NAME ExHybridMarkerTracker)
set(VERSION 0.1)
set(TYPE APP)
set(DEPENDENCIES)
set(REQUIREMENTS
        fwData
        arData
        ctrlCom
        ctrlCamp
        gui
        guiQt
        ioCalibration
        servicesReg
        fwlauncher
        appXml
        uiPreferences
        uiTools
        visuVTK
        visuVTKQt
        visuVTKAdaptor
        visuVTKARAdaptor
        videoCalibration
        videoQt
        videoOpenCV
        videoTools
        hybridMarkerTracker
        media
        maths
        preferences
        )

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExHybridMarkerTrackerConfig)
