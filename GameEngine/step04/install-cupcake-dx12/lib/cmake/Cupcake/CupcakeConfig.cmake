
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was CupcakeConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)

# GLFW is part of the public interface — Engine.h exposes GLFWwindow*
find_dependency(glfw3 REQUIRED)

# Renderer backend baked in at install time — must be found so
# CupcakeTargets.cmake can resolve the imported target it references
set(_cupcake_renderer "DX12")
if(_cupcake_renderer STREQUAL "VULKAN")
    find_dependency(Vulkan REQUIRED)
elseif(_cupcake_renderer STREQUAL "DX12")
    # DX12 uses Windows SDK system libs (d3d12, dxgi) — no find_package needed
endif()
unset(_cupcake_renderer)

include("${CMAKE_CURRENT_LIST_DIR}/CupcakeTargets.cmake")
