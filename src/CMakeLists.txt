set(CMAKE_CXX_STANDARD 17)

# 本地 jsoncpp（无需网络）
set(JSONCPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp)
add_library(jsoncpp STATIC
  ${JSONCPP_DIR}/json_reader.cpp
  ${JSONCPP_DIR}/json_value.cpp
  ${JSONCPP_DIR}/json_writer.cpp
)
target_include_directories(jsoncpp PUBLIC ${JSONCPP_DIR})

# 核心库：Land、DataManager
add_library(richman_core STATIC
  Land.cpp
  Player.cpp
  DataManager.cpp
  EventManager.cpp
  DisplayText.cpp
  DisplayManager.cpp
  GameEngine.cpp
)
target_include_directories(richman_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${JSONCPP_DIR}
)
target_link_libraries(richman_core PUBLIC jsoncpp)

# 可执行程序
add_executable(richman main.cpp)
target_link_libraries(richman PRIVATE richman_core)
