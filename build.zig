const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const ziggame = b.addExecutable(.{
        .name = "ziggame",
        .target = target,
        .optimize = optimize,
    });
    ziggame.linkLibCpp();
    ziggame.linkSystemLibrary("SDL2");
    ziggame.linkSystemLibrary("SDL2_mixer");
    ziggame.linkSystemLibrary("GL");

    ziggame.addCSourceFiles(.{
        .files = &.{
            "src/retromat/gen.cpp",
            "src/retromat/retromat.cpp",
            "src/retromat/sidstyle.cpp",

            "src/agent.cpp",
            "src/agentmanager.cpp",
            "src/arena.cpp",
            "src/background.cpp",
            "src/beam.cpp",
            "src/boids.cpp",
            "src/bonuses.cpp",
            "src/boss.cpp",
            "src/bullets.cpp",
            "src/colours.cpp",
            "src/completionscreen.cpp",
            "src/controller.cpp",
            "src/display.cpp",
            "src/drawing.cpp",
            "src/dudegallery.cpp",
            "src/dudes.cpp",
            "src/edgepuller.cpp",
            "src/effects.cpp",
            "src/fatzapper.cpp",
            "src/gameover.cpp",
            "src/gamestate.cpp",
            "src/highscores.cpp",
            "src/highscorescreen.cpp",
            "src/humanoid.cpp",
            "src/icongen.cpp",
            "src/image.cpp",
            "src/level.cpp",
            "src/leveldef.cpp",
            "src/loadpng.cpp",
            "src/lodepng.cpp",
            "src/log.cpp",
            "src/mempool.cpp",
            "src/menusupport.cpp",
            "src/optionsmenu.cpp",
            "src/optionsscreen.cpp",
            "src/paths.cpp",
            "src/pausemenu.cpp",
            "src/perlinnoise.cpp",
            "src/player.cpp",
            "src/proceduraltextures.cpp",
            "src/scene.cpp",
            "src/sinusoidal_texture.cpp",
            "src/snow.cpp",
            "src/snowblower.cpp",
            "src/soundexplore.cpp",
            "src/soundmgr.cpp",
            "src/spiker.cpp",
            "src/texture.cpp",
            "src/titlescreen.cpp",
            "src/util.cpp",
            "src/vec2.cpp",
            "src/wav.cpp",
            "src/weapons.cpp",
            "src/wobbly.cpp",
            "src/zig.cpp",
            "src/zigconfig.cpp",

        },
        .flags = &.{
            "-std=c++14",
            "-pedantic",
            "-Wall",
            "-W",
            "-Wno-missing-field-initializers",
        },
    });
    b.installArtifact(ziggame);

}
