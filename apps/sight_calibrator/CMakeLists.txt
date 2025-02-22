sight_add_target(sight_calibrator TYPE APP)

add_dependencies(
    sight_calibrator
    sightrun
    module_app
    module_service
    module_sync
    data
    module_ui
    module_ui_qt
    module_ui_icons
    module_io_vision
    module_io_video
    module_io_activity
    module_io_igtl
    module_io_session
    module_activity
    activity_navigation_calibration
    module_data
    module_memory
    module_geometry_vision
    module_geometry
    module_navigation_calibration
)

if(SIGHT_ENABLE_REALSENSE)
    add_dependencies(sight_calibrator module_io_realsense)
endif()

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)
# Main application's configuration to launch
module_param(module_app PARAM_LIST config parameters PARAM_VALUES SightCalibrator_AppCfg SightCalibrator_AppCfgParam)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/uit)
endif()
