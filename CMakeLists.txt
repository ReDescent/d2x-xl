cmake_minimum_required(VERSION 3.0)
project(D2X-XL)

set(2d_sources
    game/2d/bitblt.cpp
    game/2d/bitmap.cpp
    game/2d/canvas.cpp
    game/2d/font.cpp
    game/2d/ibitblt.cpp
    game/2d/palette.cpp
    game/2d/pcx.cpp
    game/2d/pixel.cpp
    game/2d/rle.cpp
    game/2d/scalec.cpp
    game/2d/string.cpp
    game/2d/tmerge.cpp
)

set(3d_sources
    game/3d/ase2model.cpp
    game/3d/buildmodel.cpp
    game/3d/draw.cpp
    game/3d/fastmodels.cpp
    game/3d/globvars.cpp
    game/3d/hitbox.cpp
    game/3d/interp.cpp
    game/3d/matrix.cpp
    game/3d/oof2model.cpp
    game/3d/pof2model.cpp
    game/3d/points.cpp
    game/3d/rod.cpp
    game/3d/setup.cpp
    game/3d/shadows.cpp
    game/3d/transformation.cpp
)

set(ai_sources
    game/ai/ai.cpp
    game/ai/aianimate.cpp
    game/ai/aiboss.cpp
    game/ai/aifire.cpp
    game/ai/aiframe.cpp
    game/ai/aiinit.cpp
    game/ai/ailib.cpp
    game/ai/aimove.cpp
    game/ai/aipath.cpp
    game/ai/aisave.cpp
    game/ai/aisnipe.cpp
    game/ai/escort.cpp
    game/ai/thief.cpp
    game/ai/d1_ai.cpp
)

set(audio_sources
    game/audio/linux/hmiplay.cpp
    game/audio/linux/hmpfile.cpp
    game/audio/win32/midi.cpp
    game/audio/audio.cpp
    game/audio/objectaudio.cpp
    game/audio/songs.cpp
    game/audio/soundthreads.cpp
)

set(cockpit_sources
    game/cockpit/cockpit.cpp
    game/cockpit/cockpitdata.cpp
    game/cockpit/crosshairs.cpp
    game/cockpit/genericcockpit.cpp
    game/cockpit/genericdrawers.cpp
    game/cockpit/hud.cpp
    game/cockpit/hudicons.cpp
    game/cockpit/hudmsgs.cpp
    game/cockpit/statusbar.cpp
    game/cockpit/targetindicators.cpp
    game/cockpit/utilities.cpp
)

set(console_sources
    game/console/console.cpp
    game/console/cvar.cpp
)

set(effects_sources
    game/effects/fireball.cpp
    game/effects/glare.cpp
    game/effects/lightning.cpp
    game/effects/objsmoke.cpp
    game/effects/shrapnel.cpp
    game/effects/sparkeffect.cpp
    game/effects/particle_shader.cpp
    game/effects/particle_buffer.cpp
    game/effects/particles.cpp
    game/effects/particle_emitter.cpp
    game/effects/particle_system.cpp
    game/effects/particle_manager.cpp
    game/effects/particle_images.cpp
    game/effects/waypoint.cpp
    game/effects/postprocessing.cpp
    game/effects/lightning_node.cpp
    game/effects/lightning_emitter.cpp
    game/effects/lightning_manager.cpp
    game/effects/omega_lightning.cpp
    game/effects/glow.cpp
)

set(gameio_sources
    game/gameio/addon_bitmaps.cpp
    game/gameio/createmesh.cpp
    game/gameio/loadgeometry.cpp
    game/gameio/loadobjects.cpp
    game/gameio/hoard.cpp
    game/gameio/facegrid.cpp
    game/gameio/iff.cpp
    game/gameio/loadgame.cpp
    game/gameio/loadgamedata.cpp
    game/gameio/loadmodeldata.cpp
    game/gameio/loadsounds.cpp
    game/gameio/loadtextures.cpp
    game/gameio/mission.cpp
    game/gameio/paging.cpp
    game/gameio/piggy.cpp
    game/gameio/playerprofile.cpp
    game/gameio/savegame.cpp
    game/gameio/tga.cpp
    game/gameio/IpToCountry.cpp
)

set(gamemodes_sources
    game/gamemodes/entropy.cpp
    game/gamemodes/monsterball.cpp
)

set(input_sources
    game/input/event.cpp
    game/input/input.cpp
    game/input/joy.cpp
    game/input/key.cpp
    game/input/mouse.cpp
    game/input/systemkeys.cpp
)

set(io_sources
    game/io/linux/findfile.cpp
    game/io/args.cpp
    game/io/cfile.cpp
    game/io/hogfile.cpp
)

set(libmve_sources
    game/libmve/decoder16.cpp
    game/libmve/decoder8.cpp
    game/libmve/mve_audio.cpp
    game/libmve/mvelib.cpp
    game/libmve/mveplay.cpp
)

set(lighting_sources
    game/lighting/dynlight.cpp
    game/lighting/grayscaleshaders.cpp
    game/lighting/headlight.cpp
    game/lighting/light.cpp
    game/lighting/lightcluster.cpp
    game/lighting/lightmap.cpp
    game/lighting/lightmapshaders.cpp
    game/lighting/lightprecalc.cpp
    game/lighting/lightrender.cpp
    game/lighting/lightshaders.cpp
)

set(main_sources
    game/main/briefings.cpp
    game/main/cheats.cpp
    game/main/cmd.cpp
    game/main/config.cpp
    game/main/credits.cpp
    game/main/crypt.cpp
    game/main/dialheap.cpp
    game/main/dropobject.cpp
    game/main/effects.cpp
    game/main/endlevel.cpp
    game/main/findpath.cpp
    game/main/findsegment.cpp
    game/main/flightpath.cpp
    game/main/producers.cpp
    game/main/game.cpp
    game/main/gameargs.cpp
    game/main/gamecntl.cpp
    game/main/gamedata.cpp
    game/main/gamefolders.cpp
    game/main/gameoptions.cpp
    game/main/gametime.cpp
    game/main/gamestates.cpp
    game/main/highscores.cpp
    game/main/descent.cpp
    game/main/kconfig.cpp
    game/main/meshedge.cpp
    game/main/mglobal.cpp
    game/main/movie.cpp
    game/main/newdemo.cpp
    game/main/scores.cpp
    game/main/segmath.cpp
    game/main/segment.cpp
    game/main/setup.cpp
    game/main/side.cpp
    game/main/skybox.cpp
    game/main/slew.cpp
    game/main/trigger.cpp
    game/main/terrain.cpp
    game/main/texmerge.cpp
    game/main/text.cpp
    game/main/textdata.cpp
    game/main/trackobject.cpp
    game/main/vclip.cpp
    game/main/wall.cpp
)

set(maths_sources
    game/maths/fixc.cpp
    game/maths/PerlinNoise.cpp
    game/maths/rand.cpp
    game/maths/SimplexNoise.cpp
    game/maths/tables.cpp
    game/maths/vecmat.cpp
)

set(menus_sources
    game/menus/cockpitmenu.cpp
    game/menus/configmenu.cpp
    game/menus/detailsmenu.cpp
    game/menus/effectsmenu.cpp
    game/menus/entropymenu.cpp
    game/menus/fileselector.cpp
    game/menus/gameplaymenu.cpp
    game/menus/inputdevicemenu.cpp
    game/menus/listbox.cpp
    game/menus/mainmenu.cpp
    game/menus/menubackground.cpp
    game/menus/menu.cpp
    game/menus/menuitem.cpp
    game/menus/messagebox.cpp
    game/menus/miscmenu.cpp
    game/menus/monsterballmenu.cpp
    game/menus/msgbox.cpp
    game/menus/netgamebrowser.cpp
    game/menus/netgamehelp.cpp
    game/menus/netgameinfo.cpp
    game/menus/netmenu.cpp
    game/menus/netplayerbrowser.cpp
    game/menus/newgamemenu.cpp
    game/menus/physicsmenu.cpp
    game/menus/rendermenu.cpp
    game/menus/screenresmenu.cpp
    game/menus/smokemenu.cpp
    game/menus/soundmenu.cpp
)

set(misc_sources
    game/misc/error.cpp
    game/misc/hash.cpp
    game/misc/strio.cpp
    game/misc/strutil.cpp
)

set(model_sources
    game/models/aseread.cpp
    game/models/hiresmodels.cpp
    game/models/oofmath.cpp
    game/models/oofread.cpp
    game/models/pof.cpp
    game/models/polymodel.cpp
)

set(network_sources
    game/network/linux/ipx_kali.cpp
    game/network/linux/ukali.cpp
    game/network/linux/ipx_mcast4.cpp
    game/network/linux/linuxnet.cpp
    game/network/linux/ipx_bsd.cpp
    game/network/autodl.cpp
    game/network/banlist.cpp
    game/network/multi.cpp
    game/network/multibot.cpp
    game/network/multimsg.cpp
    game/network/netmisc.cpp
    game/network/network.cpp
    game/network/network_handshake.cpp
    game/network/network_init.cpp
    game/network/network_join.cpp
    game/network/network_lib.cpp
    game/network/network_phandler.cpp
    game/network/network_process.cpp
    game/network/network_read.cpp
    game/network/network_send.cpp
    game/network/network_sync.cpp
    game/network/network_thread.cpp
    game/network/tracker.cpp
    game/network/udp_interface.cpp
)

set(objects_sources
    game/objects/boss.cpp
    game/objects/createobject.cpp
    game/objects/criticalhit.cpp
    game/objects/effectobject.cpp
    game/objects/hostage.cpp
    game/objects/marker.cpp
    game/objects/object.cpp
    game/objects/objectio.cpp
    game/objects/object_physics.cpp
    game/objects/player.cpp
    game/objects/playerdeath.cpp
    game/objects/powerup.cpp
    game/objects/reactor.cpp
    game/objects/robot.cpp
    game/objects/updateobject.cpp
)

set(ogl_sources
    game/ogl/clientstates.cpp
    game/ogl/drawbuffer.cpp
    game/ogl/fbuffer.cpp
    game/ogl/flushbuffers.cpp
    game/ogl/gr.cpp
    game/ogl/ogl_bitmap.cpp
    game/ogl/ogl_cartoonize.cpp
    game/ogl/ogl_color.cpp
    game/ogl/ogl_defs.cpp
    game/ogl/ogl_draw.cpp
    game/ogl/ogl_fastrender.cpp
    game/ogl/ogl_hudstuff.cpp
    game/ogl/ogl_lib.cpp
    game/ogl/oglmatrix.cpp
    game/ogl/ogl_palette.cpp
    game/ogl/ogl_render.cpp
    game/ogl/ogl_shader.cpp
    game/ogl/ogl_texcache.cpp
    game/ogl/ogl_texture.cpp
    game/ogl/pbuffer.cpp
    game/ogl/rendertextures.cpp
    game/ogl/screenshot.cpp
    game/ogl/sdlgl.cpp
    game/ogl/shadowmap.cpp
    game/ogl/stereo.cpp
)

set(physics_sources
    game/physics/collide.cpp
    game/physics/hitbox_collision.cpp
    game/physics/physics.cpp
    game/physics/slowmotion.cpp
    game/physics/sphere_collision.cpp
    game/physics/visibility.cpp
)

set(render_sources
    game/render/automap.cpp
    game/render/cameras.cpp
    game/render/fastrender.cpp
    game/render/gpgpu_lighting.cpp
    game/render/lighttrails.cpp
    game/render/morph.cpp
    game/render/objeffects.cpp
    game/render/objrender.cpp
    game/render/oofrender.cpp
    game/render/polymodelrender.cpp
    game/render/radar.cpp
    game/render/renderframe.cpp
    game/render/renderlib.cpp
    game/render/render_lighting.cpp
    game/render/rendermine.cpp
    game/render/rendersetup.cpp
    game/render/rendershadows.cpp
    game/render/rendershields.cpp
    game/render/renderthreads.cpp
    game/render/sphere.cpp
    game/render/thrusterflames.cpp
    game/render/transprender.cpp
    game/render/visculling.cpp
)

set(weapon_sources
    game/weapons/controlweapon.cpp
    game/weapons/createweapon.cpp
    game/weapons/dropweapon.cpp
    game/weapons/fireweapon.cpp
    game/weapons/fusion.cpp
    game/weapons/omega.cpp
    game/weapons/pickupweapon.cpp
    game/weapons/seismic.cpp
    game/weapons/weaponhandlers.cpp
    game/weapons/weaponorder.cpp
)

set( descent_sources
    ${2d_sources}
    ${3d_sources}
    ${ai_sources}
    ${audio_sources}
    ${cockpit_sources}
    ${console_sources}
    ${effects_sources}
    ${gameio_sources}
    ${gamemodes_sources}
    ${input_sources}
    ${io_sources}
    ${libmve_sources}
    ${lighting_sources}
    ${main_sources}
    ${maths_sources}
    ${menus_sources}
    ${misc_sources}
    ${model_sources}
    ${network_sources}
    ${objects_sources}
    ${ogl_sources}
    ${physics_sources}
    ${render_sources}
    ${weapon_sources}
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-strict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-strict-aliasing")

#local includes
include_directories(game/include)
include_directories(game/input/include)
include_directories(game/effects)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	include_directories(game/network/linux/include)
endif()

add_executable(d2x-xl ${descent_sources})
target_compile_definitions(d2x-xl PRIVATE
    -DHAVE_STRUCT_TIMESPEC
    -DNATIVE_IPX
    -DKALINIX
)

if (${CMAKE_BUILD_TYPE} MATCHES "Release")
    target_compile_definitions(d2x-xl PRIVATE -DRELEASE)
elseif (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    # BUG: this does too much weird shit
    # target_compile_definitions(d2x-xl PRIVATE -DDBG)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#finds
find_package(OpenMP REQUIRED)
find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(SDL2_mixer REQUIRED)

target_link_libraries(d2x-xl
    ${EXTRA_LIBRARIES}
    SDL2::Main
    SDL2::Image
    SDL2::Net
    SDL2::Mixer
    GLEW::GLEW
    OpenMP::OpenMP_CXX
    ZLIB::ZLIB
    OpenGL::GLU
)

install(TARGETS d2x-xl DESTINATION .)
