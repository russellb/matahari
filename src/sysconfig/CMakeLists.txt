set(BASE "sysconfig")
set(BASE_LIB "m${BASE}")
set(QMF_AGENT "matahari-qmf-${BASE}d")
set(QMF_CONSOLE "matahari-qmf-${BASE}-consoled")
set(DBUS_AGENT "matahari-dbus-${BASE}d")

# QMF daemon
if(WITH-QMF)
    set(SCHEMA_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/qmf/org/matahariproject/QmfPackage.cpp)
    generate_qmf_schemas(${CMAKE_CURRENT_SOURCE_DIR}/schema.xml ${SCHEMA_SOURCES})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    
    add_executable(${QMF_AGENT} ${BASE}-qmf.cpp ${SCHEMA_SOURCES})
    add_executable(${QMF_CONSOLE} ${BASE}-console.cpp)

    target_link_libraries(${QMF_AGENT} ${BASE_LIB} mcommon_qmf)
    target_link_libraries(${QMF_CONSOLE} mcommon_qmf)

    create_manpage(${QMF_AGENT} ${AGENT_MAN_SECTION} ${AGENT_MAN_DESC})
    create_manpage(${QMF_CONSOLE} ${CONSOLE_MAN_SECTION} ${CONSOLE_MAN_DESC})
    create_service_scripts(${BASE})

    set(BASE_SUB "-console")
    set(DAEMON_TYPE "CONSOLE")
    create_service_scripts(${BASE}${BASE_SUB})

    install(TARGETS ${QMF_AGENT} DESTINATION sbin)
    install(TARGETS ${QMF_CONSOLE} DESTINATION sbin)
endif(WITH-QMF)

# DBus daemon
if(WITH-DBUS)
    # Auto-generated stuff
    generate_dbus_headers(${BASE} ${CMAKE_CURRENT_BINARY_DIR}/org.matahariproject.xml)
    # Create src/dbus directory and generate DBus XML definition files into it
    generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/schema.xml)

    # Check if all properties|statistics|methods have PolicyKit action defined
    check_policies_for_schema(${CMAKE_CURRENT_SOURCE_DIR}/schema.xml)

    # Must be included to find matahari-API-dbus-properties.h
    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    set(DBUS_AGENT_SOURCE
    ${BASE}-dbus.c
    ${BASE}-dbus-glue.h
    ${BASE}-dbus-properties.h
    )

    add_executable(${DBUS_AGENT} ${DBUS_AGENT_SOURCE})

    target_link_libraries(${DBUS_AGENT} ${BASE_LIB} mcommon_dbus ${dbus-glib_LIBRARIES})
    include_directories(${dbus-glib_INCLUDE_DIRS})

    # Install targets
    # TODO: fix hardcoded paths, should go to libexec
    install(TARGETS ${DBUS_AGENT} DESTINATION sbin)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.matahariproject.xml DESTINATION share/dbus-1/interfaces RENAME org.matahariproject.Sysconfig.xml)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.matahariproject.Sysconfig.service DESTINATION share/dbus-1/system-services)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.matahariproject.Sysconfig.conf DESTINATION /etc/dbus-1/system.d)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.matahariproject.Sysconfig.policy DESTINATION share/polkit-1/actions)
endif(WITH-DBUS)
