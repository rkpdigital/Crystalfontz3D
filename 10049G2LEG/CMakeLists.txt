project(10049G2)
cmake_minimum_required(VERSION 2.8)

include_directories(${10049G2_SOURCE_DIR}/include ${10049G2_SOURCE_DIR}/settings)

SET(10049G2_SOURCES application/canonical_machine.cpp application/config_app.cpp application/config.cpp application/controller.cpp
                    application/cycle_homing.cpp application/gcode_parser.cpp application/kinematics.cpp application/plan_arc.cpp
                    application/plan_line.cpp  application/planner.cpp platform/hardware.cpp platform/help.cpp platform/main.cpp
                    platform/quicklz.cpp platform/report.cpp platform/stepper.cpp platform/switch.cpp platform/text_parser.cpp
                    platform/util.cpp)

SET(10049G2_HEADERS include/canonical_machine.h include/cfa10049_fiq.h include/config_app.h include/config.h include/controller.h
                    include/gcode_parser.h include/hardware.h include/help.h include/kinematics.h include/plan_arc.h
                    include/plan_line.h include/planner.h include/quicklz.h include/report.h include/settings.h include/stepper.h
                    include/switches.h include/text_parser.h include/tinyg2.h include/util.h include/xio.h
                    settings/settings_3DPrint.h)

SET(SRC_LIST ${10049G2_SOURCES} ${10049G2_HREADERS})

add_executable(${PROJECT_NAME} ${SRC_LIST})

