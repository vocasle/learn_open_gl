cmake_minimum_required(VERSION 3.20)
project(learn_open_gl)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(LIBS glfw glm glad stb_image minaudio)

if (!WIN32)
find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})
list(APPEND LIBS GL)
list(APPEND LIBS dl)
endif()

add_subdirectory(thirdparties)

set(CHAPTERS
        1.getting_started
        2.lighting
        3.model_loading
        4.advanced_opengl
        5.advanced_lighting
        6.pbr
        7.in_practice
        )

set(1.getting_started
        1.1.hello_window
        1.2.hello_window_clear
        2.1.hello_triangle
#        2.2.hello_triangle_indexed
#        2.3.hello_triangle_exercise1$0L2+_kd(21-jD9@
#        2.4.hello_triangle_exercise2
#        2.5.hello_triangle_exercise3
#        3.1.shaders_uniform
#        3.2.shaders_interpolation
        3.3.shaders_class
        4.1.textures
#        4.2.textures_combined
#        4.4.textures_exercise2
#        4.5.textures_exercise3
#        4.6.textures_exercise4
        5.1.transformations
#        5.2.transformations_exercise2
#        6.1.coordinate_systems
#        6.2.coordinate_systems_depth
#        6.3.coordinate_systems_multiple
#        7.1.camera_circle
#        7.2.camera_keyboard_dt
#        7.3.camera_mouse_zoom
#        7.4.camera_class
        )

set(2.lighting
        1.colors
#        2.1.basic_lighting_diffuse
        2.2.basic_lighting_specular
        3.1.materials
#        3.2.materials_exercise1
        4.1.lighting_maps_diffuse_map
#        4.2.lighting_maps_specular_map
#        4.4.lighting_maps_exercise4
#        5.1.light_casters_directional
#        5.2.light_casters_point
#        5.3.light_casters_spot
#        5.4.light_casters_spot_soft
#        6.multiple_lights
        )

set(3.model_loading
#        1.model_loading
        )

set(4.advanced_opengl
#        1.1.depth_testing
#        1.2.depth_testing_view
#        2.stencil_testing
#        3.1.blending_discard
#        3.2.blending_sort
#        5.1.framebuffers
#        5.2.framebuffers_exercise1
#        6.1.cubemaps_skybox
#        6.2.cubemaps_environment_mapping
#        8.advanced_glsl_ubo
#        9.1.geometry_shader_houses
#        9.2.geometry_shader_exploding
#        9.3.geometry_shader_normals
#        10.1.instancing_quads
#        10.2.asteroids
#        10.3.asteroids_instanced
#        11.1.anti_aliasing_msaa
#        11.2.anti_aliasing_offscreen
        )

set(5.advanced_lighting
#        1.advanced_lighting
#        2.gamma_correction
#        3.1.1.shadow_mapping_depth
#        3.1.2.shadow_mapping_base
#        3.1.3.shadow_mapping
#        3.2.1.point_shadows
#        3.2.2.point_shadows_soft
#        4.normal_mapping
#        5.1.parallax_mapping
#        5.2.steep_parallax_mapping
#        5.3.parallax_occlusion_mapping
#        6.hdr
#        7.bloom
#        8.1.deferred_shading
#        8.2.deferred_shading_volumes
#        9.ssao
        )

set(6.pbr
#        1.1.lighting
#        1.2.lighting_textured
#        2.1.1.ibl_irradiance_conversion
#        2.1.2.ibl_irradiance
#        2.2.1.ibl_specular
#        2.2.2.ibl_specular_textured
        )

set(7.in_practice
#        1.debugging
#        2.text_rendering
#        3.2d_game
        )

foreach(CHAPTER ${CHAPTERS})
    foreach(DEMO ${${CHAPTER}})
        file(GLOB SOURCE
                "src/${CHAPTER}/${DEMO}/*.h"
                "src/${CHAPTER}/${DEMO}/*.cpp"
                "src/${CHAPTER}/${DEMO}/*.vs"
                "src/${CHAPTER}/${DEMO}/*.fs"
                "src/${CHAPTER}/${DEMO}/*.gs"
                "src/${CHAPTER}/${DEMO}/*.shader"
                "src/*.h"
                "src/*.cpp"
                )
        set(NAME "${CHAPTER}__${DEMO}")
        add_executable(${NAME} ${SOURCE})
        target_link_libraries(${NAME} PRIVATE ${LIBS})
        target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CHAPTER}")

        # copy shader files to build directory
        file(GLOB SHADERS
                "src/${CHAPTER}/${DEMO}/*.frag"
                "src/${CHAPTER}/${DEMO}/*.vert"
                )

        file(GLOB ASSETS
            "src/${CHAPTER}/${DEMO}/*.png",
            "src/${CHAPTER}/${DEMO}/*.jpg"
        )

        foreach(SHADER ${SHADERS})
            add_custom_command(
                TARGET ${NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} ${CMAKE_BINARY_DIR}/${CHAPTER}
            )
        endforeach()

        foreach(ASSET ${ASSETS})
            file(COPY ${ASSET} DESTINATION ${CMAKE_BINARY_DIR}/${CHAPTER}/assets)
        endforeach()
    endforeach()
endforeach()